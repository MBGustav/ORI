#include "BinPlusTree.h"
#include <iostream>
#include <vector>

#include <algorithm>
#include <utility>

BinPlusTree::Node::Node(bool leaf)
    : isLeaf(leaf), parent(nullptr), next(nullptr), keyCount(0), childCount(0)
{
    data.clear();
    for (std::size_t i = 0; i < MAX_KEYS + 1; ++i) {
        keys[i].clear();
    }
    for (std::size_t i = 0; i < MAX_KEYS + 2; ++i) {
        children[i] = nullptr;
    }
}


void BinPlusTree::printTree() const
{
    if (!root) {
        return;
    }

    std::vector<const Node*> currentLevel;
    currentLevel.push_back(root);

    while (!currentLevel.empty()) {
        std::vector<const Node*> nextLevel;

        for (const Node* node : currentLevel) {
            if (node) {
                std::cout << "[";
                for (std::size_t i = 0; i < node->keyCount; ++i) {
                    std::cout << node->keys[i];
                    if (i + 1 < node->keyCount) {
                        std::cout << ", ";
                    }
                }
                std::cout << "] ";

                if (!node->isLeaf) {
                    for (std::size_t i = 0; i < node->childCount; ++i) {
                        nextLevel.push_back(node->children[i]);
                    }
                }
            } else {
                std::cout << "[null] ";
            }
        }

        std::cout << "\n";
        currentLevel = std::move(nextLevel);
    }
}

BinPlusTree::BinPlusTree()
    : root(nullptr)
{
}

BinPlusTree::BinPlusTree(BinPlusTree&& other) noexcept
    : root(other.root)
{
    other.root = nullptr;
}

BinPlusTree::BinPlusTree(const BinPlusTree& other)
    : root(nullptr)
{
    Node* prevLeaf = nullptr;
    root = cloneNode(other.root, nullptr, prevLeaf);
}

bool BinPlusTree::insertNode(const Index& value)
{
    if (value.key.empty()) {
        return false;
    }

    return insertNode(value.key);
}

bool BinPlusTree::searchNode(const Index& value) const
{
    return !value.key.empty() && searchNode(value.key);
}

bool BinPlusTree::deleteNode(const Index& value)
{
    return !value.key.empty() && deleteNode(value.key);
}

BinPlusTree& BinPlusTree::operator=(BinPlusTree&& other) noexcept
{
    if (this != &other) {
        clear(root);
        root = other.root;
        other.root = nullptr;
    }
    return *this;
}

BinPlusTree& BinPlusTree::operator=(const BinPlusTree& other)
{
    if (this != &other) {
        clear(root);
        root = nullptr;
        Node* prevLeaf = nullptr;
        root = cloneNode(other.root, nullptr, prevLeaf);
    }
    return *this;
}

BinPlusTree::~BinPlusTree()
{
    clear(root);
    root = nullptr;
}

bool BinPlusTree::empty() const
{
    return root == nullptr;
}

void BinPlusTree::clear(Node* node)
{
    if (!node) {
        return;
    }

    if (!node->isLeaf) {
        for (std::size_t i = 0; i < node->childCount; ++i) {
            clear(node->children[i]);
        }
    }

    delete node;
}

BinPlusTree::Node* BinPlusTree::cloneNode(const Node* node, Node* parent, Node*& prevLeaf)
{
    if (!node) {
        return nullptr;
    }

    Node* copy = new Node(node->isLeaf);
    copy->keyCount = node->keyCount;
    copy->childCount = node->childCount;
    copy->data = node->data;
    copy->parent = parent;
    copy->next = nullptr;

    for (std::size_t i = 0; i < node->keyCount; ++i) {
        copy->keys[i] = node->keys[i];
    }

    if (node->isLeaf) {
        if (prevLeaf) {
            prevLeaf->next = copy;
        }
        prevLeaf = copy;
    } else {
        for (std::size_t i = 0; i < node->childCount; ++i) {
            copy->children[i] = cloneNode(node->children[i], copy, prevLeaf);
        }
    }

    return copy;
}

std::string BinPlusTree::firstKey(const Node* node)
{
    if (!node) {
        return {};
    }

    const Node* current = node;
    while (current && !current->isLeaf) {
        if (current->childCount == 0 || current->children[0] == nullptr) {
            return {};
        }
        current = current->children[0];
    }

    if (!current || current->keyCount == 0) {
        return {};
    }

    return current->keys[0];
}

void BinPlusTree::refreshKeys(Node* node)
{
    if (!node) {
        return;
    }

    if (node->isLeaf) {
        node->data = node->keyCount > 0 ? node->keys[0] : std::string();
        node->childCount = 0;
        return;
    }

    for (std::size_t i = 0; i < node->childCount; ++i) {
        if (node->children[i]) {
            node->children[i]->parent = node;
        }
    }

    node->keyCount = node->childCount > 0 ? node->childCount - 1 : 0;
    node->data = node->childCount > 0 ? firstKey(node->children[0]) : std::string();

    for (std::size_t i = 0; i < node->keyCount; ++i) {
        node->keys[i] = firstKey(node->children[i + 1]);
    }

    for (std::size_t i = node->keyCount; i < MAX_KEYS + 1; ++i) {
        node->keys[i].clear();
    }
}

std::size_t BinPlusTree::childIndex(const Node* node, const std::string& value)
{
    std::size_t left = 0;
    std::size_t right = node ? node->keyCount : 0;

    while (left < right) {
        const std::size_t middle = left + (right - left) / 2;
        if (value < node->keys[middle]) {
            right = middle;
        } else {
            left = middle + 1;
        }
    }

    return left;
}

bool BinPlusTree::searchNode(const std::string& value) const
{
    return searchNode(root, value);
}

bool BinPlusTree::searchNode(const Node* node, const std::string& value) const
{
    if (!node) {
        return false;
    }

    if (node->isLeaf) {
        const auto begin = node->keys;
        return std::binary_search(begin, begin + node->keyCount, value);
    }

    const std::size_t index = childIndex(node, value);
    if (index >= node->childCount || node->children[index] == nullptr) {
        return false;
    }

    return searchNode(node->children[index], value);
}

bool BinPlusTree::insertNode(const std::string& value)
{
    if (searchNode(value)) {
        return false;
    }

    Node* splitRight = nullptr;
    if (!insertNode(root, value, splitRight)) {
        return false;
    }

    if (splitRight != nullptr) {
        Node* newRoot = new Node(false);
        newRoot->children[0] = root;
        newRoot->children[1] = splitRight;
        newRoot->childCount = 2;

        if (root) {
            root->parent = newRoot;
        }
        splitRight->parent = newRoot;

        refreshKeys(newRoot);
        root = newRoot;
    }

    return true;
}

bool BinPlusTree::insertNode(Node*& node, const std::string& value, Node*& splitRight)
{
    splitRight = nullptr;

    if (!node) {
        node = new Node(true);
        node->keys[0] = value;
        node->keyCount = 1;
        node->data = value;
        return true;
    }

    if (node->isLeaf) {
        std::size_t position = 0;
        while (position < node->keyCount && node->keys[position] < value) {
            ++position;
        }

        for (std::size_t i = node->keyCount; i > position; --i) {
            node->keys[i] = node->keys[i - 1];
        }

        node->keys[position] = value;
        ++node->keyCount;

        if (node->keyCount <= MAX_KEYS) {
            refreshKeys(node);
            return true;
        }

        Node* right = new Node(true);
        const std::size_t mid = node->keyCount / 2;
        const std::size_t rightCount = node->keyCount - mid;

        for (std::size_t i = 0; i < rightCount; ++i) {
            right->keys[i] = node->keys[mid + i];
            node->keys[mid + i].clear();
        }

        right->keyCount = rightCount;
        node->keyCount = mid;

        right->next = node->next;
        node->next = right;
        right->parent = node->parent;

        refreshKeys(node);
        refreshKeys(right);

        splitRight = right;
        return true;
    }

    const std::size_t index = childIndex(node, value);
    if (index >= node->childCount) {
        return false;
    }

    Node* childSplit = nullptr;
    if (!insertNode(node->children[index], value, childSplit)) {
        return false;
    }

    if (childSplit != nullptr) {
        for (std::size_t i = node->childCount; i > index + 1; --i) {
            node->children[i] = node->children[i - 1];
        }

        node->children[index + 1] = childSplit;
        ++node->childCount;
        childSplit->parent = node;
    }

    refreshKeys(node);

    if (node->childCount <= MAX_KEYS + 1) {
        return true;
    }

    Node* right = new Node(false);
    const std::size_t mid = node->childCount / 2;
    const std::size_t rightChildren = node->childCount - mid;

    for (std::size_t i = 0; i < rightChildren; ++i) {
        right->children[i] = node->children[mid + i];
        if (right->children[i]) {
            right->children[i]->parent = right;
        }
        node->children[mid + i] = nullptr;
    }

    right->childCount = rightChildren;
    node->childCount = mid;

    refreshKeys(node);
    refreshKeys(right);

    splitRight = right;
    return true;
}

void BinPlusTree::searchPrefix(const std::string& prefix, std::vector<std::string>& out) const
{
    if (!root || prefix.empty()) {
        return;
    }

    // Descend to the leftmost leaf that can hold a key >= prefix. Every key that
    // begins with prefix sorts at or after that leaf, and keys are unique.
    const Node* leaf = root;
    while (leaf && !leaf->isLeaf) {
        const std::size_t index = childIndex(leaf, prefix);
        if (index >= leaf->childCount || leaf->children[index] == nullptr) {
            return;
        }
        leaf = leaf->children[index];
    }

    while (leaf != nullptr) {
        for (std::size_t i = 0; i < leaf->keyCount; ++i) {
            const std::string& key = leaf->keys[i];
            if (key.size() >= prefix.size() &&
                key.compare(0, prefix.size(), prefix) == 0) {
                out.push_back(key);
            } else if (key > prefix) {
                return;  // moved past the prefix range
            }
        }
        leaf = leaf->next;
    }
}

bool BinPlusTree::deleteNode(const std::string& value)
{
    if (!deleteNode(root, value)) {
        return false;
    }

    // Deleting keys can remove whole leaves; rebuild the leaf chain so the
    // `next` links stay valid (used by searchPrefix and cloneNode).
    rechainLeaves();
    return true;
}

void BinPlusTree::rechainLeaves()
{
    Node* prev = nullptr;
    rechainLeaves(root, prev);
    if (prev != nullptr) {
        prev->next = nullptr;
    }
}

void BinPlusTree::rechainLeaves(Node* node, Node*& prev)
{
    if (!node) {
        return;
    }

    if (node->isLeaf) {
        if (prev != nullptr) {
            prev->next = node;
        }
        prev = node;
        return;
    }

    for (std::size_t i = 0; i < node->childCount; ++i) {
        rechainLeaves(node->children[i], prev);
    }
}

bool BinPlusTree::deleteNode(Node*& node, const std::string& value)
{
    if (!node) {
        return false;
    }

    if (node->isLeaf) {
        std::size_t position = 0;
        while (position < node->keyCount && node->keys[position] < value) {
            ++position;
        }

        if (position >= node->keyCount || node->keys[position] != value) {
            return false;
        }

        for (std::size_t i = position; i + 1 < node->keyCount; ++i) {
            node->keys[i] = node->keys[i + 1];
        }

        if (node->keyCount > 0) {
            node->keys[node->keyCount - 1].clear();
            --node->keyCount;
        }

        if (node->keyCount == 0) {
            delete node;
            node = nullptr;
            return true;
        }

        refreshKeys(node);
        return true;
    }

    const std::size_t index = childIndex(node, value);
    if (index >= node->childCount) {
        return false;
    }

    if (!deleteNode(node->children[index], value)) {
        return false;
    }

    if (node->children[index] == nullptr) {
        for (std::size_t i = index; i + 1 < node->childCount; ++i) {
            node->children[i] = node->children[i + 1];
        }
        if (node->childCount > 0) {
            node->children[node->childCount - 1] = nullptr;
            --node->childCount;
        }
    }

    if (node->childCount == 0) {
        delete node;
        node = nullptr;
        return true;
    }

    refreshKeys(node);

    if (node == root && !node->isLeaf && node->childCount == 1) {
        Node* child = node->children[0];
        if (child) {
            child->parent = nullptr;
        }
        node->children[0] = nullptr;
        delete node;
        node = child;
        root = node;
    }

    return true;
}
