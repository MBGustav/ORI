#pragma once

#include <cstddef>
#include <string>
#include "baseline_common.h"
/**
 * Ordered string index used by the SQL layer.
 *
 * The implementation behaves like a B+ tree for lookup and insertion, while
 * keeping deletion intentionally simple: nodes are split on overflow and
 * compacted only when a subtree becomes empty.
 */
class BinPlusTree {
public:
    BinPlusTree();
    ~BinPlusTree();

    BinPlusTree(const BinPlusTree& other);
    BinPlusTree& operator=(const BinPlusTree& other);
    BinPlusTree(BinPlusTree&& other) noexcept;
    BinPlusTree& operator=(BinPlusTree&& other) noexcept;

    bool insertNode(const Index& value);
    bool insertNode(const std::string& value);
    bool searchNode(const Index& value) const;
    bool searchNode(const std::string& value) const;
    bool deleteNode(const Index& value);
    bool deleteNode(const std::string& value);
    bool empty() const;
    void printTree() const;
private:
    struct Node;

    static constexpr std::size_t MAX_KEYS = 4;

    Node* root;

    bool insertNode(Node*& node, const std::string& value, Node*& splitRight);

    bool searchNode(const Node* node, const std::string& value) const;
    bool deleteNode(Node*& node, const std::string& value);

    void clear(Node* node);
    static Node* cloneNode(const Node* node, Node* parent, Node*& prevLeaf);
    void refreshKeys(Node* node);
    static std::size_t childIndex(const Node* node, const std::string& value);
    static std::string firstKey(const Node* node);
};

struct BinPlusTree::Node {
    bool isLeaf;
    std::string data;
    Node* parent;
    Node* next;
    std::string keys[MAX_KEYS + 1];
    Node* children[MAX_KEYS + 2];
    std::size_t keyCount;
    std::size_t childCount;

    explicit Node(bool leaf = true);
};
