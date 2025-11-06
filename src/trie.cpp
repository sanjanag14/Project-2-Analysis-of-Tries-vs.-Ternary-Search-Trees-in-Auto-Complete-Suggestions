// placeholder source for trie (to be implemented by kalea)
#include "trie.hpp"
#include <cstddef>
#include <string>
#include <vector>
using namespace std;
Trie::Node::Node() {
    endTrie = false;
    for (int i = 0; i < 26; i++) {
        children[i] = nullptr;
    }
}
Trie::Trie() {
    root = nullptr;
    nodeCnt = 0;
}
Trie::~Trie() {
    destroy(root);
    root = nullptr;
    nodeCnt=0;
}
void Trie::insert(string& word) {
    if (word.empty()) {
        return;
    }
    const char* str = word.c_str();
    Node* update = insert(root, str, nodeCnt);
    root = update;
}
bool Trie::contain(string& word)  {
    if (word.empty()) {
        return false;
    }
    const char* str = word.c_str();
    bool exist = contain(root, str);
    return exist;
}
vector<string> Trie::autocomplete(string& prefix, size_t max) {
    vector<string> final;
    if (root == nullptr) {
        return final;
    }
    if (prefix.empty()) {
        return final;
    }
    if (max == 0) {
        return final;
    }
    Node* node = findPre(root, prefix.c_str());
    if (node == nullptr) {
    return final;
    }
    string now = prefix;
    if (node->endTrie) {
    final.push_back(prefix);
    } 
    collect(node, now, final, max);
    return final;
}
Trie::Node* Trie::insert(Node* node, const char* word, size_t& cnt) {
    if (node == nullptr) {
        node = new Node();
        cnt = cnt + 1;
    }
    char val = *word;
    if (val == '\0') {
        node->endTrie = true;
        return node;
    }
    if (val < 'a' || val > 'z') {
        return node;
    }
    int ind = val - 'a'; 
    Node* kid = insert(node->children[ind], word + 1, cnt);
    node->children[ind] = kid;
    return node;
}
bool Trie::contain( Node* node, const char* word) {
    if (node == nullptr) {
        return false;
    }
    char val = *word;
    if (val == '\0') {
        return node->endTrie;
    }
    if (val < 'a' || val > 'z') {
        return false;
    }
    int ind = val - 'a';
    Node* kid = node->children[ind];
    const char* next = word + 1;
    return contain(kid, next);
}
Trie::Node* Trie::findPre(Node* node, const char* pre) {
    if (node == nullptr) {
        return nullptr;
    }
    char val = *pre;
    if (val == '\0') {
        return node; 
    }
    if (val < 'a' || val > 'z') {
        return nullptr;
    }
    int ind = val - 'a';
    Node* kid = node->children[ind];
    const char* next = pre + 1;
    return findPre(kid, next);
}
void Trie::collect( Node* node, string now, vector<string>& out, size_t max) {
    if (node == nullptr) {
        return;
    }
    if (out.size() >= max) {
        return;
    }
    if (node->endTrie && out.size() < max) {
    out.push_back(now);
    } else{
    }
    for (int i = 0; i < 26; i++) {
        if (node->children[i] == nullptr) {
            continue;
        }
        if (out.size() >= max) {
            return;
        }
        now.push_back('a' + i);
        collect(node->children[i], now, out, max);
        now.pop_back();
    }
}
size_t Trie::cntNodes() { 
    return nodeCnt; 
}
bool Trie::empty()  { 
    return root == nullptr; 
}
bool Trie::search( std::string& word) { 
    return contain(word); 
}
vector<std::string> Trie::autocomplete(string& prefix) {
    return autocomplete(prefix, 10);
}
size_t Trie::getMemory() {
    return nodeCnt * sizeof(Node);
}
void Trie::destroy(Node* node) {
    if (node != nullptr) {
    for (int i = 0; i < 26; i++) {
        destroy(node->children[i]);
    }
    delete node;
    } else {
        return;
    }
}
