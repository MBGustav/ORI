#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "baseline_common.h"
#include "PageCodec.h"
#include "PageManager.h"

/*
 * RecordStore.h
 *
 * Stores a table's rows in a B+Tree whose leaves hold the records themselves.
 * Key = RecordID (monotonic, 0-based). Internal nodes only route the search.
 *
 * Persistence uses the page layer:
 *   - page 0            : meta page (PageType::... META) -> root page id
 *   - internal nodes    : NODE_TYPE_INTERNAL pages (uint64 keys + child page ids)
 *   - leaf nodes        : NODE_TYPE_LEAF pages (key + record bytes + next leaf)
 *
 * The whole tree lives in memory while open; every insert rewrites the pages so
 * the on-disk copy always mirrors the tree. On construction the tree is rebuilt
 * by reading the pages back from disk.
 */
class RecordStore {
public:
    explicit RecordStore(std::string filePath)
        : path_(std::move(filePath)), nextPage_(1), nextKey_(0), count_(0) {
        ensureFileExists();
        pages_.open(path_, std::ios::in | std::ios::out | std::ios::binary);
        load();
    }

    RecordStore(const RecordStore&) = delete;
    RecordStore& operator=(const RecordStore&) = delete;

    ~RecordStore() {
        freeTree(root_);
    }

    RecordID insert(const std::vector<Value>& values) {
        Record record{values};

        if (PageCodec::encodedLeafEntrySize(record)
                + page_layout::LEAF_PAGE_HEADER_SIZE > PAGE_SIZE) {
            throw std::runtime_error("RecordStore: record does not fit in a leaf page");
        }

        const RecordID key = nextKey_++;

        if (root_ == nullptr) {
            root_ = new Node(true);
            root_->pageId = allocatePage();
            root_->keys.push_back(key);
            root_->records.push_back(record);
        } else {
            Node* leaf = findLeaf(key);
            insertLeafEntry(leaf, key, record);
            if (PageCodec::encodedLeafSize(leaf->keys, leaf->records) > PAGE_SIZE) {
                splitLeaf(leaf);
            }
        }

        ++count_;
        persist();
        return key;
    }

    std::size_t size() const {
        return count_;
    }

    bool empty() const {
        return count_ == 0;
    }

    const Record& at(RecordID id) const {
        if (id >= count_) {
            throw std::out_of_range("RecordStore::at: invalid RecordID");
        }

        const Node* node = root_;
        while (node != nullptr && !node->leaf) {
            const std::size_t idx = static_cast<std::size_t>(
                std::upper_bound(node->keys.begin(), node->keys.end(), id)
                - node->keys.begin());
            node = node->children[idx];
        }

        if (node == nullptr) {
            throw std::out_of_range("RecordStore::at: invalid RecordID");
        }

        const std::size_t pos = static_cast<std::size_t>(
            std::lower_bound(node->keys.begin(), node->keys.end(), id)
            - node->keys.begin());

        if (pos >= node->keys.size() || node->keys[pos] != id) {
            throw std::out_of_range("RecordStore::at: invalid RecordID");
        }

        return node->records[pos];
    }

    const std::string& path() const {
        return path_;
    }

    uint64_t pageCount() {
        return pages_.pageCount();
    }

private:
    struct Node {
        bool leaf;
        PageID pageId;
        Node* parent;
        Node* next;                      // next leaf (sequential access)
        std::vector<RecordID> keys;      // leaf: row ids; internal: separators
        std::vector<Node*> children;     // internal only
        std::vector<Record> records;     // leaf only

        explicit Node(bool isLeaf)
            : leaf(isLeaf), pageId(0), parent(nullptr), next(nullptr) {}
    };

    // Matches the B+Tree order used by BinPlusTree: at most 4 separator keys
    // (5 children) before an internal node splits.
    static constexpr std::size_t MAX_INTERNAL_KEYS = 4;

    void ensureFileExists() {
        std::ifstream probe(path_, std::ios::binary);
        if (probe.good()) {
            return;
        }

        std::ofstream create(path_, std::ios::binary);
        if (!create) {
            throw std::runtime_error("RecordStore: cannot create " + path_);
        }
    }

    PageID allocatePage() {
        return nextPage_++;
    }

    Node* findLeaf(RecordID key) {
        Node* node = root_;
        while (node != nullptr && !node->leaf) {
            const std::size_t idx = static_cast<std::size_t>(
                std::upper_bound(node->keys.begin(), node->keys.end(), key)
                - node->keys.begin());
            node = node->children[idx];
        }
        return node;
    }

    void splitLeaf(Node* leaf) {
        const std::size_t totalBytes =
            PageCodec::encodedLeafSize(leaf->keys, leaf->records);
        const std::size_t half = totalBytes / 2;

        std::size_t acc = page_layout::LEAF_PAGE_HEADER_SIZE;
        std::size_t mid = 0;
        for (; mid < leaf->keys.size(); ++mid) {
            acc += PageCodec::encodedLeafEntrySize(leaf->records[mid]);
            if (acc >= half) {
                ++mid;
                break;
            }
        }
        if (mid == 0) {
            mid = 1;
        }
        if (mid >= leaf->keys.size()) {
            mid = leaf->keys.size() - 1;
        }

        Node* right = new Node(true);
        right->pageId = allocatePage();
        right->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
        right->records.assign(leaf->records.begin() + mid, leaf->records.end());

        leaf->keys.resize(mid);
        leaf->records.resize(mid);

        right->next = leaf->next;
        leaf->next = right;

        const RecordID promote = right->keys.front();
        insertIntoParent(leaf, promote, right);
    }

    void insertLeafEntry(Node* leaf, RecordID key, const Record& record) {
        const std::size_t pos = static_cast<std::size_t>(
            std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key)
            - leaf->keys.begin());
        leaf->keys.insert(leaf->keys.begin() + pos, key);
        leaf->records.insert(leaf->records.begin() + pos, record);
    }

    void insertIntoParent(Node* left, RecordID key, Node* right) {
        Node* parent = left->parent;

        if (parent == nullptr) {
            Node* newRoot = new Node(false);
            newRoot->pageId = allocatePage();
            newRoot->keys.push_back(key);
            newRoot->children.push_back(left);
            newRoot->children.push_back(right);
            left->parent = newRoot;
            right->parent = newRoot;
            root_ = newRoot;
            return;
        }

        std::size_t idx = 0;
        while (idx < parent->children.size() && parent->children[idx] != left) {
            ++idx;
        }

        parent->keys.insert(parent->keys.begin() + idx, key);
        parent->children.insert(parent->children.begin() + idx + 1, right);
        right->parent = parent;

        if (parent->keys.size() > MAX_INTERNAL_KEYS) {
            splitInternal(parent);
        }
    }

    void splitInternal(Node* node) {
        const std::size_t mid = node->keys.size() / 2;
        const RecordID promote = node->keys[mid];

        Node* right = new Node(false);
        right->pageId = allocatePage();
        right->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
        right->children.assign(node->children.begin() + mid + 1, node->children.end());
        for (Node* child : right->children) {
            child->parent = right;
        }

        node->keys.resize(mid);
        node->children.resize(mid + 1);

        insertIntoParent(node, promote, right);
    }

    void persist() {
        uint8_t buffer[PAGE_SIZE] = {0};

        Page meta;
        meta.type = page_layout::NODE_TYPE_META;
        if (root_ != nullptr) {
            meta.keys.push_back(root_->pageId);
        }

        PageCodec codec;
        codec.encode(meta, buffer, PAGE_SIZE);
        pages_.writePage(0, buffer);

        if (root_ != nullptr) {
            persistNode(root_);
        }
    }

    void persistNode(Node* node) {
        uint8_t buffer[PAGE_SIZE] = {0};

        if (node->leaf) {
            const PageID nextId = node->next != nullptr ? node->next->pageId : 0;
            PageCodec::encodeLeaf(node->keys, node->records, nextId, buffer, PAGE_SIZE);
        } else {
            Page page;
            page.type = page_layout::NODE_TYPE_INTERNAL;
            page.keys = node->keys;
            page.children.reserve(node->children.size());
            for (Node* child : node->children) {
                page.children.push_back(child->pageId);
            }

            PageCodec codec;
            codec.encode(page, buffer, PAGE_SIZE);
        }

        pages_.writePage(node->pageId, buffer);

        if (!node->leaf) {
            for (Node* child : node->children) {
                persistNode(child);
            }
        }
    }

    void load() {
        const uint64_t pageTotal = pages_.pageCount();
        nextPage_ = pageTotal == 0 ? 1 : pageTotal;

        if (pageTotal == 0) {
            return;
        }

        std::vector<uint8_t> buffer(PAGE_SIZE, 0);
        pages_.readPage(0, buffer.data());

        PageCodec codec;
        Page meta = codec.decode(buffer.data(), PAGE_SIZE);
        if (meta.keys.empty()) {
            return;
        }

        root_ = readNode(static_cast<PageID>(meta.keys[0]), nullptr);

        count_ = 0;
        Node* prev = nullptr;
        rechainLeaves(root_, prev);
        nextKey_ = count_;
    }

    Node* readNode(PageID id, Node* parent) {
        std::vector<uint8_t> buffer(PAGE_SIZE, 0);
        pages_.readPage(id, buffer.data());

        Node* node = new Node(buffer[0] == page_layout::NODE_TYPE_LEAF);
        node->pageId = id;
        node->parent = parent;

        if (node->leaf) {
            PageID nextId = 0;
            PageCodec::decodeLeaf(buffer.data(), PAGE_SIZE, node->keys, node->records, nextId);
        } else {
            PageCodec codec;
            Page page = codec.decode(buffer.data(), PAGE_SIZE);
            node->keys = page.keys;
            node->children.reserve(page.children.size());
            for (const PageID childId : page.children) {
                node->children.push_back(readNode(childId, node));
            }
        }

        return node;
    }

    void rechainLeaves(Node* node, Node*& prev) {
        if (node == nullptr) {
            return;
        }

        if (node->leaf) {
            node->next = nullptr;
            if (prev != nullptr) {
                prev->next = node;
            }
            prev = node;
            count_ += node->keys.size();
            return;
        }

        for (Node* child : node->children) {
            rechainLeaves(child, prev);
        }
    }

    static void freeTree(Node* node) {
        if (node == nullptr) {
            return;
        }
        if (!node->leaf) {
            for (Node* child : node->children) {
                freeTree(child);
            }
        }
        delete node;
    }

    std::string path_;
    PageManager pages_;
    Node* root_ = nullptr;
    PageID nextPage_;
    RecordID nextKey_;
    std::size_t count_;
};
