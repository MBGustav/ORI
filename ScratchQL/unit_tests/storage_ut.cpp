// Simple unit tests for FileManager, PageCodec and BinPlusTree.
//
// How to build/run (from the repository root):
//   g++ -std=c++17 -I ScratchQL/include \
//       ScratchQL/unit_tests/storage_ut.cpp ScratchQL/src/BinPlusTree.cpp \
//       -o /tmp/storage_ut && /tmp/storage_ut
//
// The program prints [PASSED]/[FAILED] per check and returns non-zero if any
// check fails.
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "FileManager.h"
#include "PageCodec.h"
#include "BinPlusTree.h"

namespace {

int failures = 0;

void check(bool condition, const std::string& label) {
    if (condition) {
        std::cout << "[PASSED] " << label << "\n";
    } else {
        ++failures;
        std::cout << "[FAILED] " << label << "\n";
    }
}

// ---------------------------------------------------------------- FileManager

void testFileManager() {
    std::cout << "\n== FileManager ==\n";

    const std::string path = "filemanager_ut.bin";
    std::remove(path.c_str());

    const std::vector<uint8_t> written = {0x10, 0x20, 0x30, 0x40, 0x50};
    const uint64_t offset = 16;

    {
        FileManager writer;
        check(!writer.isOpen(), "default constructed file is not open");

        writer.open(
            path,
            std::ios::out | std::ios::binary | std::ios::trunc
        );
        check(writer.isOpen(), "open() reports an open file");

        writer.write(offset, written.data(), written.size());
        writer.close();
        check(!writer.isOpen(), "close() closes the file");
    }

    {
        FileManager reader(path, std::ios::in | std::ios::binary);
        check(reader.isOpen(), "constructor opens an existing file");

        std::vector<uint8_t> read(written.size(), 0);
        reader.read(offset, read.data(), read.size());

        check(read == written, "write/read round-trip preserves bytes");
    }

    {
        FileManager closed;
        bool threw = false;
        try {
            uint8_t byte = 0;
            closed.read(0, &byte, 1);
        } catch (const std::exception&) {
            threw = true;
        }
        check(threw, "reading from an unopened file throws");
    }

    std::remove(path.c_str());
}

// ------------------------------------------------------------------ PageCodec

void testPageCodec() {
    std::cout << "\n== PageCodec ==\n";

    PageCodec codec;

    {
        Page leaf;
        leaf.type = page_layout::NODE_TYPE_LEAF;
        leaf.keys = {5, 6, 7};

        const std::size_t needed = PageCodec::encodedSize(leaf);
        check(needed == page_layout::PAGE_HEADER_SIZE + 3 * page_layout::PAGE_KEY_SIZE,
              "encodedSize matches header + keys");

        std::vector<uint8_t> buffer(needed, 0);
        codec.encode(leaf, buffer.data(), buffer.size());

        Page decoded = codec.decode(buffer.data(), buffer.size());

        check(decoded.type == page_layout::NODE_TYPE_LEAF, "leaf type survives round-trip");
        check(decoded.key_count == 3, "key_count is stored in the header");
        check(decoded.child_count == 0, "child_count is stored in the header");
        check(decoded.keys == leaf.keys, "keys survive round-trip");
        check(decoded.children.empty(), "leaf has no children");
    }

    {
        Page internal;
        internal.type = page_layout::NODE_TYPE_INTERNAL;
        internal.keys = {100, 200};
        internal.children = {1, 2, 3};

        std::vector<uint8_t> buffer(PageCodec::encodedSize(internal), 0);
        codec.encode(internal, buffer.data(), buffer.size());

        Page decoded = codec.decode(buffer.data(), buffer.size());

        check(decoded.children == internal.children, "children survive round-trip");
        check(decoded.keys == internal.keys, "internal keys survive round-trip");
        check(decoded.child_count == 3, "internal child_count is correct");
    }

    {
        Page empty;
        std::vector<uint8_t> buffer(PageCodec::encodedSize(empty), 0);
        codec.encode(empty, buffer.data(), buffer.size());

        Page decoded = codec.decode(buffer.data(), buffer.size());
        check(decoded.keys.empty() && decoded.children.empty(),
              "empty page round-trips");
    }

    {
        Page page;
        page.keys = {1, 2, 3};
        std::vector<uint8_t> small(page_layout::PAGE_HEADER_SIZE, 0);

        bool threw = false;
        try {
            codec.encode(page, small.data(), small.size());
        } catch (const std::exception&) {
            threw = true;
        }
        check(threw, "encode into too-small buffer throws");
    }

    {
        std::vector<uint8_t> truncated(page_layout::PAGE_HEADER_SIZE, 0);
        truncated[1] = 5;  // declares 5 keys but provides no key bytes

        bool threw = false;
        try {
            codec.decode(truncated.data(), truncated.size());
        } catch (const std::exception&) {
            threw = true;
        }
        check(threw, "decode of a truncated page throws");
    }
}

// ---------------------------------------------------------------- BinPlusTree

void testBinPlusTree() {
    std::cout << "\n== BinPlusTree ==\n";

    {
        BinPlusTree tree;
        check(tree.empty(), "a new tree is empty");
        check(!tree.searchNode("missing"), "searching an empty tree returns false");
    }

    {
        BinPlusTree tree;

        const std::vector<std::string> values = {
            "delta", "alpha", "charlie", "bravo", "echo"
        };

        for (const std::string& value : values) {
            check(tree.insertNode(value), "insert " + value);
        }

        check(!tree.empty(), "tree is not empty after inserts");
        check(!tree.insertNode("alpha"), "duplicate insert is rejected");

        for (const std::string& value : values) {
            check(tree.searchNode(value), "search finds " + value);
        }

        check(!tree.searchNode("foxtrot"), "search misses absent key");
        check(!tree.searchNode(""), "search ignores empty key");
        check(!tree.insertNode(""), "insert ignores empty key");
    }

    {
        BinPlusTree tree;

        // Enough sequential keys to force node splits.
        const int count = 100;
        for (int i = 0; i < count; ++i) {
            tree.insertNode("key-" + std::to_string(i));
        }

        bool allFound = true;
        for (int i = 0; i < count; ++i) {
            if (!tree.searchNode("key-" + std::to_string(i))) {
                allFound = false;
                break;
            }
        }
        check(allFound, "all inserted keys are searchable after splits");
        check(!tree.searchNode("key-1000"), "absent key stays absent after splits");
    }

    {
        BinPlusTree tree;

        tree.insertNode("one");
        tree.insertNode("two");
        tree.insertNode("three");

        check(tree.deleteNode("two"), "delete an existing key");
        check(!tree.searchNode("two"), "deleted key is gone");
        check(tree.searchNode("one") && tree.searchNode("three"),
              "remaining keys are preserved");
        check(!tree.deleteNode("two"), "deleting a missing key returns false");

        check(tree.deleteNode("one"), "delete first key");
        check(tree.deleteNode("three"), "delete last key");
        check(tree.empty(), "tree becomes empty after deleting every key");
    }

    {
        BinPlusTree tree;
        for (int i = 0; i < 50; ++i) {
            tree.insertNode("n-" + std::to_string(i));
        }
        for (int i = 0; i < 50; ++i) {
            tree.deleteNode("n-" + std::to_string(i));
        }
        check(tree.empty(), "tree is empty after deleting a large batch");
    }

    {
        // Index-aware API.
        BinPlusTree tree;
        check(tree.insertNode(Index{"pk", 42}), "insert an Index");
        check(tree.searchNode(Index{"pk", 42}), "search an Index by key");
        check(tree.deleteNode(Index{"pk", 42}), "delete an Index");
        check(!tree.searchNode("pk"), "Index key is gone after delete");
    }
}

}  // namespace

int main() {
    testFileManager();
    testPageCodec();
    testBinPlusTree();

    std::cout << "\n";
    if (failures == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cout << failures << " check(s) failed.\n";
    return 1;
}
