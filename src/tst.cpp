#include "tst.hpp"

// tiny tst implementation for autocomplete stuff

// basic ctor/dtor
TernarySearchTree::TernarySearchTree() : root(nullptr), node_count(0) {}
TernarySearchTree::~TernarySearchTree() { destroy(root); root = nullptr; node_count = 0; }

void TernarySearchTree::insert(const std::string& word) {
    if (word.empty()) return; // ignore empty
    root = insert(root, word.c_str(), node_count);
}

bool TernarySearchTree::contains(const std::string& word) const {
    if (word.empty()) return false;
    return contains(root, word.c_str());
}

std::vector<std::string> TernarySearchTree::autocomplete(const std::string& prefix, std::size_t max_results) const {
    std::vector<std::string> results;
    if (!root || prefix.empty() || max_results == 0) return results;

    const Node* node = findPrefixNode(root, prefix.c_str());
    if (!node) return results;

    std::string current = prefix;
    if (node->isEnd) {
        results.push_back(prefix);
        if (results.size() >= max_results) return results;
    }
    collect(node->eq, current, results, max_results);
    return results;
}

std::size_t TernarySearchTree::nodeCount() const { return node_count; }

bool TernarySearchTree::empty() const { return root == nullptr; }

// interface-aligned convenience methods
bool TernarySearchTree::search(const std::string& word) const { return contains(word); }

std::vector<std::string> TernarySearchTree::autoComplete(const std::string& prefix) const {
    // default to a small list
    return autocomplete(prefix, 10);
}

std::size_t TernarySearchTree::getMemoryUsage() const {
    // rough estimate of memory usage
    return node_count * sizeof(Node);
}

// static helpers
TernarySearchTree::Node* TernarySearchTree::insert(Node* node, const char* word, std::size_t& node_count_ref) {
    char c = *word;
    if (c == '\0') return node; // shouldn't happen since we guard empty

    if (!node) {
        node = new Node(c);
        ++node_count_ref;
    }

    if (c < node->ch) {
        node->left = insert(node->left, word, node_count_ref);
    } else if (c > node->ch) {
        node->right = insert(node->right, word, node_count_ref);
    } else { // equal char
        if (*(word + 1) == '\0') {
            node->isEnd = true;
        } else {
            node->eq = insert(node->eq, word + 1, node_count_ref);
        }
    }
    return node;
}

bool TernarySearchTree::contains(const Node* node, const char* word) {
    if (!node || !word) return false;
    char c = *word;
    if (c < node->ch) return contains(node->left, word);
    if (c > node->ch) return contains(node->right, word);
    // equal
    if (*(word + 1) == '\0') return node->isEnd;
    return contains(node->eq, word + 1);
}

const TernarySearchTree::Node* TernarySearchTree::findPrefixNode(const Node* node, const char* prefix) {
    if (!node || !prefix) return nullptr;
    char c = *prefix;
    if (c < node->ch) return findPrefixNode(node->left, prefix);
    if (c > node->ch) return findPrefixNode(node->right, prefix);
    if (*(prefix + 1) == '\0') return node; // matched last prefix char at this node
    return findPrefixNode(node->eq, prefix + 1);
}

void TernarySearchTree::collect(const Node* node, std::string& current, std::vector<std::string>& out, std::size_t max_results) {
    if (!node || out.size() >= max_results) return;

    // go left
    collect(node->left, current, out, max_results);
    if (out.size() >= max_results) return;

    // visit: append char, walk equal, then pop
    current.push_back(node->ch);
    if (node->isEnd) {
        out.push_back(current);
    }
    if (out.size() < max_results) {
        collect(node->eq, current, out, max_results);
    }
    current.pop_back();

    if (out.size() >= max_results) return;

    // go right
    collect(node->right, current, out, max_results);
}

void TernarySearchTree::destroy(Node* node) {
    if (!node) return;
    // post-order delete
    destroy(node->left);
    destroy(node->eq);
    destroy(node->right);
    delete node;
}
