#ifndef TST_HPP
#define TST_HPP

#include <cstddef>
#include <string>
#include <vector>

// this is the ternary search tree interface and implementation details
class TernarySearchTree {
public:
    TernarySearchTree();
    ~TernarySearchTree();

    // insert a word into the tst
    void insert(const std::string& word);

    // check if a word exists in the tst
    bool contains(const std::string& word) const;

    // get up to max_results words that start with prefix
    std::vector<std::string> autocomplete(const std::string& prefix, std::size_t max_results = 10) const;

    // number of nodes in the tst (rough proxy for size)
    std::size_t nodeCount() const;

    // fast check for empty structure
    bool empty() const;

    // below methods mirror the interface expected by main
    bool search(const std::string& word) const; // alias for contains
    std::vector<std::string> autoComplete(const std::string& prefix) const; // default sized autocomplete
    std::size_t getMemoryUsage() const; // bytes used by nodes

private:
    struct Node {
        char ch;
        bool isEnd;
        Node* left;
        Node* eq;
        Node* right;
        explicit Node(char c) : ch(c), isEnd(false), left(nullptr), eq(nullptr), right(nullptr) {}
    };

    Node* root;
    std::size_t node_count;

    // helpers
    static Node* insert(Node* node, const char* word, std::size_t& node_count_ref);
    static bool contains(const Node* node, const char* word);
    static const Node* findPrefixNode(const Node* node, const char* prefix);
    static void collect(const Node* node, std::string& current, std::vector<std::string>& out, std::size_t max_results);

    static void destroy(Node* node);
};

#endif // TST_HPP
