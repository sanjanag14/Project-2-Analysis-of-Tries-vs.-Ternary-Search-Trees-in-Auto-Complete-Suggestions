#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include "trie.hpp"
#include "tst.hpp"

using namespace std;
using namespace chrono;

// Data structure to hold performance metrics
struct PerformanceMetrics {
    double avgInsertionTime;
    double avgSearchTime;
    size_t memoryUsage;
    int numWords;
};

class DatasetManager {
private:
    vector<string> words;
    string filename;

public:
    DatasetManager(const string& file) : filename(file) {}

    bool loadDataset() {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Error: Could not open file " << filename << endl;
            return false;
        }

        string word;
        while (getline(file, word)) {
            // Clean the word (remove whitespace, convert to lowercase)
            word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
            if (!word.empty()) {
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                words.push_back(word);
            }
        }
        file.close();

        cout << "Successfully loaded " << words.size() << " words from dataset." << endl;
        return true;
    }

    const vector<string>& getWords() const {
        return words;
    }

    vector<string> getSample(int n) const {
        if (n >= words.size()) return words;
        vector<string> sample;
        for (int i = 0; i < n && i < words.size(); i++) {
            sample.push_back(words[i]);
        }
        return sample;
    }
};

class PerformanceTester {
public:
    static PerformanceMetrics testTrie(Trie* structure, const vector<string>& words) {
        PerformanceMetrics metrics = {0, 0, 0, 0};

        // Test insertion time
        auto startInsert = high_resolution_clock::now();
        for (auto word : words) {  // Need non-const for Trie::insert
            structure->insert(word);
        }
        auto endInsert = high_resolution_clock::now();
        auto durationInsert = duration_cast<microseconds>(endInsert - startInsert);
        metrics.avgInsertionTime = durationInsert.count() / (double)words.size();

        // Test search time
        auto startSearch = high_resolution_clock::now();
        for (auto word : words) {  // Need non-const for Trie::search
            structure->search(word);
        }
        auto endSearch = high_resolution_clock::now();
        auto durationSearch = duration_cast<microseconds>(endSearch - startSearch);
        metrics.avgSearchTime = durationSearch.count() / (double)words.size();

        // Get memory usage
        metrics.memoryUsage = structure->getMemory();
        metrics.numWords = words.size();

        return metrics;
    }

    static PerformanceMetrics testTST(TernarySearchTree* structure, const vector<string>& words) {
        PerformanceMetrics metrics = {0, 0, 0, 0};

        // Test insertion time
        auto startInsert = high_resolution_clock::now();
        for (const auto& word : words) {
            structure->insert(word);
        }
        auto endInsert = high_resolution_clock::now();
        auto durationInsert = duration_cast<microseconds>(endInsert - startInsert);
        metrics.avgInsertionTime = durationInsert.count() / (double)words.size();

        // Test search time
        auto startSearch = high_resolution_clock::now();
        for (const auto& word : words) {
            structure->search(word);
        }
        auto endSearch = high_resolution_clock::now();
        auto durationSearch = duration_cast<microseconds>(endSearch - startSearch);
        metrics.avgSearchTime = durationSearch.count() / (double)words.size();

        // Get memory usage
        metrics.memoryUsage = structure->getMemoryUsage();
        metrics.numWords = words.size();

        return metrics;
    }

    static void displayComparison(const PerformanceMetrics& trieMetrics,
                                  const PerformanceMetrics& tstMetrics) {
        cout << "\n" << string(80, '=') << endl;
        cout << "PERFORMANCE COMPARISON RESULTS" << endl;
        cout << string(80, '=') << endl;
        cout << "Number of words tested: " << trieMetrics.numWords << endl << endl;

        cout << left << setw(30) << "Metric"
             << setw(20) << "Trie"
             << setw(20) << "TST"
             << setw(10) << "Winner" << endl;
        cout << string(80, '-') << endl;

        // Insertion time comparison
        cout << left << setw(30) << "Avg Insertion Time (μs)";
        cout << setw(20) << fixed << setprecision(4) << trieMetrics.avgInsertionTime;
        cout << setw(20) << fixed << setprecision(4) << tstMetrics.avgInsertionTime;
        cout << setw(10) << (trieMetrics.avgInsertionTime < tstMetrics.avgInsertionTime ? "Trie" : "TST") << endl;

        // Search time comparison
        cout << left << setw(30) << "Avg Search Time (μs)";
        cout << setw(20) << fixed << setprecision(4) << trieMetrics.avgSearchTime;
        cout << setw(20) << fixed << setprecision(4) << tstMetrics.avgSearchTime;
        cout << setw(10) << (trieMetrics.avgSearchTime < tstMetrics.avgSearchTime ? "Trie" : "TST") << endl;

        // Memory usage comparison
        cout << left << setw(30) << "Memory Usage (KB)";
        cout << setw(20) << (trieMetrics.memoryUsage / 1024.0);
        cout << setw(20) << (tstMetrics.memoryUsage / 1024.0);
        cout << setw(10) << (trieMetrics.memoryUsage < tstMetrics.memoryUsage ? "Trie" : "TST") << endl;

        cout << string(80, '=') << endl;

        // Summary statistics
        double insertSpeedup = max(trieMetrics.avgInsertionTime, tstMetrics.avgInsertionTime) /
                               min(trieMetrics.avgInsertionTime, tstMetrics.avgInsertionTime);
        double searchSpeedup = max(trieMetrics.avgSearchTime, tstMetrics.avgSearchTime) /
                              min(trieMetrics.avgSearchTime, tstMetrics.avgSearchTime);
        double memoryRatio = max(trieMetrics.memoryUsage, tstMetrics.memoryUsage) /
                            (double)min(trieMetrics.memoryUsage, tstMetrics.memoryUsage);

        cout << "\nSUMMARY:" << endl;
        cout << "- Insertion: " << (trieMetrics.avgInsertionTime < tstMetrics.avgInsertionTime ? "Trie" : "TST")
             << " is " << fixed << setprecision(2) << insertSpeedup << "x faster" << endl;
        cout << "- Search: " << (trieMetrics.avgSearchTime < tstMetrics.avgSearchTime ? "Trie" : "TST")
             << " is " << fixed << setprecision(2) << searchSpeedup << "x faster" << endl;
        cout << "- Memory: " << (trieMetrics.memoryUsage < tstMetrics.memoryUsage ? "Trie" : "TST")
             << " uses " << fixed << setprecision(2) << memoryRatio << "x less memory" << endl;
        cout << string(80, '=') << endl;
    }
};

class MenuSystem {
private:
    Trie* trie;
    TernarySearchTree* tst;
    DatasetManager* dataManager;
    bool trieLoaded;
    bool tstLoaded;

public:
    MenuSystem(Trie* t, TernarySearchTree* ts, DatasetManager* dm)
        : trie(t), tst(ts), dataManager(dm), trieLoaded(false), tstLoaded(false) {}

    void displayMenu() {
        cout << "\n" << string(60, '=') << endl;
        cout << "AUTO-COMPLETE: TRIE vs TST COMPARISON SYSTEM" << endl;
        cout << string(60, '=') << endl;
        cout << "1.  Load dataset into Trie" << endl;
        cout << "2.  Load dataset into TST" << endl;
        cout << "3.  Insert word into Trie" << endl;
        cout << "4.  Insert word into TST" << endl;
        cout << "5.  Search word in Trie" << endl;
        cout << "6.  Search word in TST" << endl;
        cout << "7.  Auto-complete using Trie" << endl;
        cout << "8.  Auto-complete using TST" << endl;
        cout << "9.  Compare performance (load sample)" << endl;
        cout << "10. Compare performance (full dataset)" << endl;
        cout << "11. Display memory usage" << endl;
        cout << "0.  Exit" << endl;
        cout << string(60, '=') << endl;
        cout << "Enter choice: ";
    }

    void run() {
        int choice;
        string word, prefix;

        while (true) {
            displayMenu();
            cin >> choice;
            cin.ignore();

            switch (choice) {
                case 1:
                    loadIntoTrie();
                    break;
                case 2:
                    loadIntoTST();
                    break;
                case 3:
                    cout << "Enter word to insert: ";
                    getline(cin, word);
                    insertWordTrie(word);
                    break;
                case 4:
                    cout << "Enter word to insert: ";
                    getline(cin, word);
                    insertWordTST(word);
                    break;
                case 5:
                    cout << "Enter word to search: ";
                    getline(cin, word);
                    searchWordTrie(word);
                    break;
                case 6:
                    cout << "Enter word to search: ";
                    getline(cin, word);
                    searchWordTST(word);
                    break;
                case 7:
                    cout << "Enter prefix for auto-complete: ";
                    getline(cin, prefix);
                    autoCompleteTrie(prefix);
                    break;
                case 8:
                    cout << "Enter prefix for auto-complete: ";
                    getline(cin, prefix);
                    autoCompleteTST(prefix);
                    break;
                case 9:
                    compareSample();
                    break;
                case 10:
                    compareFull();
                    break;
                case 11:
                    displayMemoryUsage();
                    break;
                case 0:
                    cout << "Exiting program. Goodbye!" << endl;
                    return;
                default:
                    cout << "Invalid choice. Please try again." << endl;
            }
        }
    }

private:
    void loadIntoTrie() {
        auto start = high_resolution_clock::now();
        for (auto word : dataManager->getWords()) {  // Need non-const copy
            trie->insert(word);
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);

        trieLoaded = true;
        cout << "Loaded " << dataManager->getWords().size()
             << " words into Trie in " << duration.count() << " ms" << endl;
    }

    void loadIntoTST() {
        auto start = high_resolution_clock::now();
        for (const auto& word : dataManager->getWords()) {
            tst->insert(word);
        }
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(end - start);

        tstLoaded = true;
        cout << "Loaded " << dataManager->getWords().size()
             << " words into TST in " << duration.count() << " ms" << endl;
    }

    void insertWordTrie(string word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        trie->insert(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Inserted '" << word << "' into Trie in " << duration.count() << " μs" << endl;
    }

    void insertWordTST(string word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        tst->insert(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Inserted '" << word << "' into TST in " << duration.count() << " μs" << endl;
    }

    void searchWordTrie(string word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        bool found = trie->search(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Word '" << word << "' " << (found ? "FOUND" : "NOT FOUND")
             << " in Trie (" << duration.count() << " μs)" << endl;
    }

    void searchWordTST(const string& word) {
        string lowerWord = word;
        transform(lowerWord.begin(), lowerWord.end(), lowerWord.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        bool found = tst->search(lowerWord);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Word '" << word << "' " << (found ? "FOUND" : "NOT FOUND")
             << " in TST (" << duration.count() << " μs)" << endl;
    }

    void autoCompleteTrie(string prefix) {
        transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        vector<string> suggestions = trie->autocomplete(prefix);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        cout << "Auto-complete suggestions for '" << prefix << "' from Trie:" << endl;
        int count = 0;
        for (const auto& suggestion : suggestions) {
            cout << "  " << suggestion << endl;
            if (++count >= 10) break; // Limit to 10 suggestions
        }
        cout << "Total: " << suggestions.size() << " suggestions ("
             << duration.count() << " μs)" << endl;
    }

    void autoCompleteTST(const string& prefix) {
        string lowerPrefix = prefix;
        transform(lowerPrefix.begin(), lowerPrefix.end(), lowerPrefix.begin(), ::tolower);
        auto start = high_resolution_clock::now();
        vector<string> suggestions = tst->autoComplete(lowerPrefix);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);

        cout << "Auto-complete suggestions for '" << prefix << "' from TST:" << endl;
        int count = 0;
        for (const auto& suggestion : suggestions) {
            cout << "  " << suggestion << endl;
            if (++count >= 10) break; // Limit to 10 suggestions
        }
        cout << "Total: " << suggestions.size() << " suggestions ("
             << duration.count() << " μs)" << endl;
    }

    void compareSample() {
        cout << "Enter sample size (e.g., 1000, 10000): ";
        int sampleSize;
        cin >> sampleSize;

        vector<string> sample = dataManager->getSample(sampleSize);

        cout << "\nTesting with " << sample.size() << " words..." << endl;

        // Create fresh structures for fair comparison
        Trie* freshTrie = new Trie();
        TernarySearchTree* freshTST = new TernarySearchTree();

        cout << "Testing Trie..." << endl;
        PerformanceMetrics trieMetrics = PerformanceTester::testTrie(freshTrie, sample);

        cout << "Testing TST..." << endl;
        PerformanceMetrics tstMetrics = PerformanceTester::testTST(freshTST, sample);

        PerformanceTester::displayComparison(trieMetrics, tstMetrics);

        delete freshTrie;
        delete freshTST;
    }

    void compareFull() {
        cout << "Warning: This will test the full dataset and may take time." << endl;
        cout << "Proceed? (y/n): ";
        char confirm;
        cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            cout << "Comparison cancelled." << endl;
            return;
        }

        const vector<string>& allWords = dataManager->getWords();
        cout << "\nTesting with " << allWords.size() << " words..." << endl;

        // Create fresh structures for fair comparison
        Trie* freshTrie = new Trie();
        TernarySearchTree* freshTST = new TernarySearchTree();

        cout << "Testing Trie..." << endl;
        PerformanceMetrics trieMetrics = PerformanceTester::testTrie(freshTrie, allWords);

        cout << "Testing TST..." << endl;
        PerformanceMetrics tstMetrics = PerformanceTester::testTST(freshTST, allWords);

        PerformanceTester::displayComparison(trieMetrics, tstMetrics);

        delete freshTrie;
        delete freshTST;
    }

    void displayMemoryUsage() {
        cout << "\n" << string(50, '=') << endl;
        cout << "MEMORY USAGE" << endl;
        cout << string(50, '=') << endl;

        if (trieLoaded) {
            size_t trieMemory = trie->getMemory();
            cout << "Trie: " << (trieMemory / 1024.0) << " KB" << endl;
        } else {
            cout << "Trie: Not loaded" << endl;
        }

        if (tstLoaded) {
            size_t tstMemory = tst->getMemoryUsage();
            cout << "TST:  " << (tstMemory / 1024.0) << " KB" << endl;
        } else {
            cout << "TST:  Not loaded" << endl;
        }

        cout << string(50, '=') << endl;
    }
};

int main() {
    // Display available datasets
    cout << "Available dataset files:" << endl;
    cout << "1. words.txt" << endl;
    cout << "2. words_alpha.txt" << endl;
    cout << "3. words_dictionary.json" << endl;
    cout << "4. Custom file (enter filename)" << endl;
    cout << "Enter choice (1-4): ";
    
    int fileChoice;
    cin >> fileChoice;
    cin.ignore();
    
    string filename;
    switch (fileChoice) {
        case 1:
            filename = "words.txt";
            break;
        case 2:
            filename = "words_alpha.txt";
            break;
        case 3:
            filename = "words_dictionary.json";
            break;
        case 4:
            cout << "Enter filename: ";
            getline(cin, filename);
            break;
        default:
            cout << "Invalid choice. Using words.txt" << endl;
            filename = "words.txt";
    }
    
    // Initialize dataset manager
    DatasetManager dataManager(filename);

    cout << "\nLoading dataset from " << filename << "..." << endl;
    if (!dataManager.loadDataset()) {
        cerr << "Failed to load dataset. Please ensure '" << filename << "' is in the current directory." << endl;
        return 1;
    }

    // Create instances of Trie and TST
    Trie* trie = new Trie();
    TernarySearchTree* tst = new TernarySearchTree();

    MenuSystem menu(trie, tst, &dataManager);
    menu.run();

    delete trie;
    delete tst;

    return 0;
}
