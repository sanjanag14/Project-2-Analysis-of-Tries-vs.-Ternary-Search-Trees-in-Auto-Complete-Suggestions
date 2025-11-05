// placeholder for trie (to be implemented by kalea)
#ifndef TRIE_HPP
#define TRIE_HPP
#include <cstddef>
#include <string>
#include <vector>
using namespace std;
class Trie{
private:
     struct Node{
        bool endTrie;
        Node* children[26];
        Node();
     };
public:
     Trie();
     ~Trie();
     void insert(string& word);
     bool contain(string& word);
     vector<string> autocomplete(string& prefix, size_t max);
     size_t cntNodes();
     bool empty();
     bool search(string& word);
     vector<string> autocomplete(string& prefix);
     size_t getMemory();
private:
    Node* root;
    size_t nodeCnt;
    Node* insert(Node* node, const char* word, size_t& cnt);
    bool contain(Node* node, const char* word);
    Node* findPre(Node* node, const char* pre);
    void collect(Node* node, string now, vector<string>& out, size_t max);
    void destroy(Node* node);
};
#endif
