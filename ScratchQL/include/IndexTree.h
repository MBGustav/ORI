#pragma once

#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>

#include "baseline_common.h"
#include "BinPlusTree.h"

/*
 * IndexTree.h
 *
 * A column index that maps a column value to the RecordID(s) of the rows that
 * hold it. It is built on top of BinPlusTree, which stores unique string keys,
 * so the pair (column value, RecordID) is encoded into a single unique key:
 *
 *   <value length as decimal> ':' <value bytes> <RecordID as 20 digits>
 *
 * The length prefix makes the value boundary unambiguous even when the value
 * itself contains ':' or digits, and the fixed-width RecordID suffix makes the
 * key unique per row. All rows that share a value therefore share a key prefix,
 * which is what search() enumerates.
 *
 * Because the RecordID is part of the key, duplicate column values are
 * supported: each row gets its own entry and lookup returns every matching row.
 */
class IndexTree {
public:
    IndexTree() = default;
    IndexTree(const IndexTree&) = default;
    IndexTree& operator=(const IndexTree&) = default;
    IndexTree(IndexTree&&) noexcept = default;
    IndexTree& operator=(IndexTree&&) noexcept = default;

    // Records that @p id holds @p value. Returns false if that exact pair is
    // already present.
    bool insert(const std::string& value, RecordID id) {
        return tree_.insertNode(encodeKey(value, id));
    }

    // Removes the (value, id) pair. Returns false if it was not present.
    bool remove(const std::string& value, RecordID id) {
        return tree_.deleteNode(encodeKey(value, id));
    }

    // Appends the RecordID of every row holding @p value to @p out (ascending).
    // Returns the number of matches.
    std::size_t search(const std::string& value, std::vector<RecordID>& out) const {
        std::vector<std::string> encoded;
        tree_.searchPrefix(encodePrefix(value), encoded);

        for (const std::string& key : encoded) {
            out.push_back(decodeId(key));
        }
        return encoded.size();
    }

    bool contains(const std::string& value) const {
        std::vector<std::string> encoded;
        tree_.searchPrefix(encodePrefix(value), encoded);
        return !encoded.empty();
    }

    bool empty() const {
        return tree_.empty();
    }

    void printTree() const {
        tree_.printTree();
    }

private:
    // uint64_t has at most 20 decimal digits, so the id suffix is fixed width.
    static constexpr std::size_t kIdWidth = 20;

    static std::string encodePrefix(const std::string& value) {
        std::string prefix;
        prefix.reserve(value.size() + 21);
        prefix += std::to_string(value.size());
        prefix += ':';
        prefix += value;
        return prefix;
    }

    static std::string encodeKey(const std::string& value, RecordID id) {
        std::string key = encodePrefix(value);

        char idBuffer[kIdWidth + 1];
        std::snprintf(
            idBuffer, sizeof(idBuffer), "%020llu",
            static_cast<unsigned long long>(id)
        );
        key += idBuffer;
        return key;
    }

    static RecordID decodeId(const std::string& key) {
        return static_cast<RecordID>(std::stoull(key.substr(key.size() - kIdWidth)));
    }

    BinPlusTree tree_;
};
