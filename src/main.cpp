#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>

using namespace std;
using namespace chrono;

// Include the TST implementation
#include "tst.hpp"

// Forward declaration for Trie class
// This will be implemented by Kalea
class Trie {
public:
    virtual void insert(const string& word) = 0;
    virtual bool search(const string& word) = 0;
    virtual vector<string> autoComplete(const string& prefix) = 0;
    virtual size_t getMemoryUsage() = 0;
    virtual ~Trie() {}
};

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
    template<typename T>
    static PerformanceMetrics testStructure(T* structure, const vector<string>& words) {
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
        for (const auto& word : dataManager->getWords()) {
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

    void insertWordTrie(const string& word) {
        auto start = high_resolution_clock::now();
        trie->insert(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Inserted '" << word << "' into Trie in " << duration.count() << " μs" << endl;
    }

    void insertWordTST(const string& word) {
        auto start = high_resolution_clock::now();
        tst->insert(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Inserted '" << word << "' into TST in " << duration.count() << " μs" << endl;
    }

    void searchWordTrie(const string& word) {
        auto start = high_resolution_clock::now();
        bool found = trie->search(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Word '" << word << "' " << (found ? "FOUND" : "NOT FOUND") 
             << " in Trie (" << duration.count() << " μs)" << endl;
    }

    void searchWordTST(const string& word) {
        auto start = high_resolution_clock::now();
        bool found = tst->search(word);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start);
        cout << "Word '" << word << "' " << (found ? "FOUND" : "NOT FOUND") 
             << " in TST (" << duration.count() << " μs)" << endl;
    }

    void autoCompleteTrie(const string& prefix) {
        auto start = high_resolution_clock::now();
        vector<string> suggestions = trie->autoComplete(prefix);
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
        auto start = high_resolution_clock::now();
        vector<string> suggestions = tst->autoComplete(prefix);
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
        // Note: For Trie, you'll need Kalea's implementation
        TernarySearchTree* freshTST = new TernarySearchTree();
        
        // For now, just test TST if Trie isn't available
        cout << "\nTesting TST..." << endl;
        PerformanceMetrics tstMetrics = PerformanceTester::testStructure(freshTST, sample);
        
        cout << "\nTST Results:" << endl;
        cout << "- Avg Insertion Time: " << fixed << setprecision(4) 
             << tstMetrics.avgInsertionTime << " μs" << endl;
        cout << "- Avg Search Time: " << fixed << setprecision(4) 
             << tstMetrics.avgSearchTime << " μs" << endl;
        cout << "- Memory Usage: " << (tstMetrics.memoryUsage / 1024.0) << " KB" << endl;
        
        delete freshTST;
        
        // Uncomment when Trie implementation is ready:
        /*
        Trie* freshTrie = new TrieImplementation();
        cout << "\nTesting Trie..." << endl;
        PerformanceMetrics trieMetrics = PerformanceTester::testStructure(freshTrie, sample);
        
        cout << "\nGenerating comparison..." << endl;
        PerformanceTester::displayComparison(trieMetrics, tstMetrics);
        
        delete freshTrie;
        */
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
        
        vector<string> allWords = dataManager->getWords();
        cout << "\nTesting with " << allWords.size() << " words..." << endl;
        
        // Create fresh structures for fair comparison
        TernarySearchTree* freshTST = new TernarySearchTree();
        
        cout << "\nTesting TST..." << endl;
        PerformanceMetrics tstMetrics = PerformanceTester::testStructure(freshTST, allWords);
        
        cout << "\nTST Results:" << endl;
        cout << "- Avg Insertion Time: " << fixed << setprecision(4) 
             << tstMetrics.avgInsertionTime << " μs" << endl;
        cout << "- Avg Search Time: " << fixed << setprecision(4) 
             << tstMetrics.avgSearchTime << " μs" << endl;
        cout << "- Memory Usage: " << (tstMetrics.memoryUsage / 1024.0) << " KB" << endl;
        
        delete freshTST;
        
        // Uncomment when Trie implementation is ready:
        /*
        Trie* freshTrie = new TrieImplementation();
        cout << "\nTesting Trie..." << endl;
        PerformanceMetrics trieMetrics = PerformanceTester::testStructure(freshTrie, allWords);
        
        cout << "\nGenerating comparison..." << endl;
        PerformanceTester::displayComparison(trieMetrics, tstMetrics);
        
        delete freshTrie;
        */
    }

    void displayMemoryUsage() {
        cout << "\n" << string(50, '=') << endl;
        cout << "MEMORY USAGE" << endl;
        cout << string(50, '=') << endl;
        
        if (trieLoaded) {
            size_t trieMemory = trie->getMemoryUsage();
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
    // Initialize dataset manager
    DatasetManager dataManager("479k_words.txt");
    
    cout << "Loading dataset..." << endl;
    if (!dataManager.loadDataset()) {
        cerr << "Failed to load dataset. Please ensure '479k_words.txt' is in the current directory." << endl;
        return 1;
    }

    cout << "\nDataset loaded successfully!" << endl;
    
    // Initialize TST (Aarnav's implementation is ready!)
    TernarySearchTree* tst = new TernarySearchTree();
    
    // Note: Uncomment once Kalea's Trie implementation is ready
    /*
    Trie* trie = new TrieImplementation();
    MenuSystem menu(trie, tst, &dataManager);
    menu.run();
    delete trie;
    */
    
    // For now, we can still use the menu with just TST
    // You'll need to temporarily comment out Trie-specific menu options
    // or implement a stub Trie class
    
    cout << "TST structure initialized." << endl;
    cout << "Waiting for Kalea's Trie implementation to enable full comparison." << endl;
    
    // Temporary: You can test TST directly here
    cout << "\n--- Quick TST Test ---" << endl;
    auto testWords = dataManager.getSample(5);
    for (const auto& word : testWords) {
        tst->insert(word);
        cout << "Inserted: " << word << endl;
    }
    
    cout << "\nSearching for first word..." << endl;
    if (!testWords.empty() && tst->search(testWords[0])) {
        cout << "Found: " << testWords[0] << endl;
        
        // Test autocomplete with first 3 chars
        if (testWords[0].length() >= 3) {
            string prefix = testWords[0].substr(0, 3);
            auto suggestions = tst->autoComplete(prefix);
            cout << "\nAutocomplete for '" << prefix << "':" << endl;
            for (const auto& s : suggestions) {
                cout << "  - " << s << endl;
            }
        }
    }
    
    cout << "\nMemory usage: " << (tst->getMemoryUsage() / 1024.0) << " KB" << endl;
    
    delete tst;
    return 0;
}
