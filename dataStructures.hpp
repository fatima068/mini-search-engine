#ifndef DATASTRUCTURES_HPP
#define DATASTRUCTURES_HPP

#include <iostream>
#include <cstdlib>
using namespace std;

template <typename T>
class safeArray {
    private:
    int ncols; 
    T *dynamicArray;

    public:
    safeArray() : ncols(0), dynamicArray(NULL) {}

    safeArray (const safeArray & rhs) {
        ncols = rhs.ncols;
        dynamicArray = new T[ncols];
        for(int i = 0; i < ncols; i++)
            dynamicArray[i] = rhs.dynamicArray[i];
    }

    void pushback(const T& val) {
        T *newArray = new T[ncols + 1];
        for (int i = 0; i < ncols; ++i) {
            newArray[i] = dynamicArray[i]; 
        }
        newArray[ncols] = val; 
        delete[] dynamicArray; 
        dynamicArray = newArray;
        ncols++;
    } 

    void clear() {
        delete[] dynamicArray;
        dynamicArray = nullptr;
        ncols = 0;
    }

    ~safeArray (){
        delete [] dynamicArray;
        dynamicArray=NULL;
    }

    T &operator[] (int i) const {
        if (i < 0 || i >= ncols) {
            throw std::out_of_range("safeArray index out of bounds");
        }
        return dynamicArray[i];
    }

    safeArray &operator= (const safeArray & rhs) {
        if (this == &rhs)
            return *this;

        delete[] dynamicArray;
        dynamicArray = NULL;
        ncols = rhs.ncols;
        dynamicArray = new T[ncols];
        for(int i = 0; i < ncols; i++)
            dynamicArray[i] = rhs.dynamicArray[i];

        return *this;
    }

    void print() {
        for(int i = 0; i < ncols; i++)
            cout << dynamicArray[i] << ", ";
        cout << endl;
    }

    int size() const {
        return ncols;
    }

    bool isEmpty() const {
        return ncols == 0;
    }
};

struct HashNode { // for the hash table that is storing frequency of words in a file for each file 
    string word;
    int count;
    safeArray<int> positions; // word's positions in the file 
    HashNode* next;

    HashNode(string word, int count, int position = -1) : word(word), count(count), next(nullptr) {
        if(position != -1) positions.pushback(position);
    }
};

class HashTable {
    private:
    int tableSize;
    HashNode** table;

    int hashFunction(const string& key) { // to generate hash index for a given word
        unsigned long hash = 0;
        for (char c : key)
            hash = (hash * 31 + c) % tableSize;
        return hash;
    }

    public:
    HashTable(int size = 101) { //look in to hash function effeciency later
        tableSize = size;
        table = new HashNode*[tableSize];
        for (int i = 0; i< tableSize; i++) {
            table[i] = nullptr;
        } 
    }

    HashNode** getTable() const {
        return table;
    }

    HashTable(const HashTable &rhs) {
        tableSize = rhs.tableSize; 

        table = new HashNode*[tableSize];
        for (int i = 0; i < tableSize; i++) {
            if (rhs.table[i] == nullptr) { // no collisions, next ptr is null, so move on to the next index
                table[i] = nullptr;
            } else { // the linked list is also a pointer so deep copy 
                HashNode* currentSrc = rhs.table[i];
                HashNode* currentDest = new HashNode(currentSrc->word, currentSrc->count);

                table[i] = currentDest;
                currentSrc = currentSrc->next;
                while (currentSrc != nullptr) {
                    currentDest->next = new HashNode(currentSrc->word, currentSrc->count);
                    currentDest = currentDest->next;
                    currentSrc = currentSrc->next;
                }
            }
        }
    }

    HashTable& operator=(const HashTable &rhs) {
        if (this == &rhs) {return *this; }

        for (int i = 0; i < tableSize; i++) { //cleaning existing data
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;
        tableSize = rhs.tableSize; 
        table = new HashNode*[tableSize];

        for (int i = 0; i < tableSize; i++) {
            if (rhs.table[i] == nullptr) {
                table[i] = nullptr;
            } else {
                table[i] = new HashNode(rhs.table[i]->word, rhs.table[i]->count);

                HashNode* srcCurr = rhs.table[i]->next;
                HashNode* destCurr = table[i];
                while (srcCurr != nullptr) {
                    destCurr->next = new HashNode(srcCurr->word, srcCurr->count);
                    destCurr = destCurr->next;
                    srcCurr = srcCurr->next;
                }
            }
        }
        return *this;
    }

    void insertWord(const string& word,int position =-1) {
        int index = hashFunction(word);
        HashNode* head = table[index];
        HashNode* temp = head;
        while (temp != nullptr) {
            if (temp->word == word) {
                temp->count++;
                if(position != -1) {
                    temp->positions.pushback(position);
                }
                return;
            }
            temp = temp->next;
        }
        HashNode* newNode = new HashNode(word, 1);
        newNode->next = head;
        table[index] = newNode;
    }

    HashNode* findWordNode(const string& word) {
        int index = hashFunction(word);
        HashNode* temp = table[index];
        while (temp != nullptr) {
            if (temp->word == word) return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    int getFrequency(const string& word) {
        int index = hashFunction(word);
        HashNode* temp = table[index];

        while (temp != nullptr) {
            if (temp->word == word)
                return temp->count;
            temp = temp->next;
        }
        return 0; 
    }

    void display() {
        for (int i = 0; i < tableSize; i++) {
            HashNode* current = table[i];
            if (current != nullptr) {
                cout << "[" << i << "]: ";
                while (current != nullptr) {
                    cout << "(" << current->word << ", " << current->count << ") -> ";
                    current = current->next;
                }
                cout << "NULL" << endl;
            }
        }
    }

    int getSize() const {
        return tableSize;
    }

    ~HashTable() {
        for (int i = 0; i < tableSize; i++) {
            HashNode *toDelete = table[i];
            while (toDelete != nullptr) {
                HashNode *next = toDelete->next;
                delete toDelete;
                toDelete = next;
            }
        }
        delete[] table;
    }
};

struct FileData { // is used to store data of each file
    string filename;
    safeArray<string> tokens;
    HashTable freqTable;
};

struct FileNode {  //linked list for frequency and source file of word in inverted index table 
    string filename;
    int frequency;
    FileNode* next;
    
    FileNode() : filename(""), frequency(0), next(nullptr) {}

    FileNode(string filename, int frequency) : filename(filename), frequency(frequency), next(nullptr) {}
};

struct InvertedIndexTableNode {
    string word;
    FileNode* fileList;
    InvertedIndexTableNode* next;
    InvertedIndexTableNode(string word) : word(word), fileList(nullptr), next(nullptr) {}
};

class InvertedIndexHashTable { // maps each word to the list of files in which it appears 
    private:
    int tableSize;
    InvertedIndexTableNode** table;
    // table
    // ↓
    // pointer → array of pointers → which each points to the head of a linked list of InvertedIndexTableNode
    // inside every InvertedIndexTableNode there is a linked list of FileNodes

    int hashFunction(const string& key) {
        unsigned long hash = 0;
        for (char c : key)
            hash = (hash * 31 + c) % tableSize;
        return hash;
    }

    public:
    InvertedIndexHashTable(int size = 101) {
        tableSize = size;
        table = new InvertedIndexTableNode*[tableSize];
        for (int i = 0; i < tableSize; i++)
            table[i] = nullptr;
    }

    int getTableSize() const { return tableSize; }

    InvertedIndexTableNode** getTable() const { return table; }

    InvertedIndexHashTable(const InvertedIndexHashTable& rhs) {
        tableSize = rhs.tableSize;
        table = new InvertedIndexTableNode*[tableSize];

        for (int i = 0; i < tableSize; i++) {
            if (rhs.table[i] == nullptr) {
                table[i] = nullptr;
            } else {
                // copy the word-level linked list
                InvertedIndexTableNode* srcWordNode = rhs.table[i];
                InvertedIndexTableNode* destWordNode = new InvertedIndexTableNode(srcWordNode->word);
                table[i] = destWordNode;

                // Copy the file list for this word
                if (srcWordNode->fileList != nullptr) {
                    FileNode* srcFileNode = srcWordNode->fileList;
                    FileNode* destFileNode = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                    destWordNode->fileList = destFileNode;
                    srcFileNode = srcFileNode->next;

                    // copy file nodes
                    while (srcFileNode != nullptr) {
                        destFileNode->next = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                        destFileNode = destFileNode->next;
                        srcFileNode = srcFileNode->next;
                    }
                }

                srcWordNode = srcWordNode->next;

                // Deep copy rest of the word nodes in the list
                while (srcWordNode != nullptr) {
                    destWordNode->next = new InvertedIndexTableNode(srcWordNode->word);
                    destWordNode = destWordNode->next;

                    // Copy file list for each next word
                    if (srcWordNode->fileList != nullptr) {
                        FileNode* srcFileNode = srcWordNode->fileList;
                        FileNode* destFileNode = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                        destWordNode->fileList = destFileNode;
                        srcFileNode = srcFileNode->next;

                        while (srcFileNode != nullptr) {
                            destFileNode->next = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                            destFileNode = destFileNode->next;
                            srcFileNode = srcFileNode->next;
                        }
                    }

                    srcWordNode = srcWordNode->next;
                }
            }
        }
    }

    InvertedIndexHashTable& operator=(const InvertedIndexHashTable& rhs) {
        if (this == &rhs)
            return *this; 

        for (int i = 0; i < tableSize; i++) {
            InvertedIndexTableNode* wordNode = table[i];
            while (wordNode != nullptr) {
                // delete all file nodes in fileList
                FileNode* fileNode = wordNode->fileList;
                while (fileNode != nullptr) {
                    FileNode* tempFile = fileNode;
                    fileNode = fileNode->next;
                    delete tempFile;
                }

                // delete the node itself
                InvertedIndexTableNode* tempWord = wordNode;
                wordNode = wordNode->next;
                delete tempWord;
            }
        }
        delete[] table;

        tableSize = rhs.tableSize;
        table = new InvertedIndexTableNode*[tableSize];

        for (int i = 0; i < tableSize; i++) {
            if (rhs.table[i] == nullptr) {
                table[i] = nullptr;
            } else {
                // copy word node list
                InvertedIndexTableNode* srcWordNode = rhs.table[i];
                InvertedIndexTableNode* destWordNode = new InvertedIndexTableNode(srcWordNode->word);
                table[i] = destWordNode;

                // Copy file list
                if (srcWordNode->fileList != nullptr) {
                    FileNode* srcFileNode = srcWordNode->fileList;
                    FileNode* destFileNode = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                    destWordNode->fileList = destFileNode;
                    srcFileNode = srcFileNode->next;

                    while (srcFileNode != nullptr) {
                        destFileNode->next = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                        destFileNode = destFileNode->next;
                        srcFileNode = srcFileNode->next;
                    }
                }

                srcWordNode = srcWordNode->next;

                // Copy remaining word nodes in lsit
                while (srcWordNode != nullptr) {
                    destWordNode->next = new InvertedIndexTableNode(srcWordNode->word);
                    destWordNode = destWordNode->next;

                    // Copy file list for this word
                    if (srcWordNode->fileList != nullptr) {
                        FileNode* srcFileNode = srcWordNode->fileList;
                        FileNode* destFileNode = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                        destWordNode->fileList = destFileNode;
                        srcFileNode = srcFileNode->next;

                        while (srcFileNode != nullptr) {
                            destFileNode->next = new FileNode(srcFileNode->filename, srcFileNode->frequency);
                            destFileNode = destFileNode->next;
                            srcFileNode = srcFileNode->next;
                        }
                    }

                    srcWordNode = srcWordNode->next;
                }
            }
        }

        return *this;
    }

    void insert(const string& word, const string& filename, int frequency) {
        int index = hashFunction(word);
        InvertedIndexTableNode* head = table[index];

        // Search if word exists
        InvertedIndexTableNode* current = head;
        while (current != nullptr) {
            if (current->word == word) {
                FileNode* f = current->fileList;
                while (f != nullptr) {
                    if (f->filename == filename) {
                        f->frequency = frequency; // update frequency if the word is already present
                        return;
                    }
                    f = f->next;
                }
                // word not found, so add new
                FileNode* newFile = new FileNode(filename, frequency);
                newFile->next = current->fileList;
                current->fileList = newFile;
                return;
            }
            current = current->next;
        }

        // word not found, so add new
        InvertedIndexTableNode* newEntry = new InvertedIndexTableNode(word);
        FileNode* newFile = new FileNode(filename, frequency);
        newEntry->fileList = newFile;
        newEntry->next = head;
        table[index] = newEntry;
    }


    FileNode* search(const string& word) { // will return a pointer to file list of given word
        int index = hashFunction(word);
        InvertedIndexTableNode* current = table[index];

        while (current != nullptr) {
            if (current->word == word) {
                return current->fileList; // found, return linked list of files
            }
            current = current->next;
        }
        return nullptr;
    }

    void display() { 
        cout << "\n--GLOBAL INVERTED INDEX--" << endl;
        for (int i = 0; i < tableSize; i++) {
            InvertedIndexTableNode* current = table[i];
            while (current != nullptr) {
                cout << "[" << current->word << "] -> ";
                FileNode* filePtr = current->fileList;
                while (filePtr != nullptr) {
                    cout << "(" << filePtr->filename << ", " << filePtr->frequency << ") -> ";
                    filePtr = filePtr->next;
                }
                cout << "NULL\n";
                current = current->next;
            }
        }
    }
};

struct SearchHistoryNode { // linked list based stack for search history
    string query;
    SearchHistoryNode* next;
    SearchHistoryNode(string query) : query(query), next(nullptr) {}
};

class SearchHistory { // history stack 
    private:
    SearchHistoryNode* top;

    public:
    SearchHistory() : top(nullptr) {}

    void push(const string& query) {
        SearchHistoryNode* newNode = new SearchHistoryNode(query);
        newNode->next = top;
        top = newNode;
    }

    SearchHistoryNode* peek() {
        return top;
    }

    void display() {
        if (top == nullptr) {
            cout << "search history empty" << endl;
            return;
        }
        cout << "\nSearch History:" << endl; 
        SearchHistoryNode* temp = top;
        int count = 1;
        while (temp != nullptr) {
            cout << count++ << ". " << temp->query << endl;
            temp = temp->next;
        }
    }

    void pop(){
        if (top==nullptr){
            cout<<"search history empty, cannot pop"<< endl;
            return;
        }
        SearchHistoryNode* temp = top;
        top = top->next;
        delete temp;
    }

    void clear() {
        while (top != nullptr) {
            SearchHistoryNode* temp = top;
            top = top->next;
            delete temp;
        }
        cout << "search history cleared" << endl;
    }

    ~SearchHistory() {
        clear();
    }
};

struct TrieNode {
    bool isEnd;
    TrieNode* children[26];

    TrieNode() {
        isEnd = false;
        for (int i = 0; i < 26; i++) {
            children[i] = nullptr;
        }
    }
};

class Trie {
    private:
    TrieNode* root;

    void dfs(TrieNode* node, string& current, safeArray<string>& result) {
        if (node->isEnd)
            result.pushback(current);

        for (int i = 0; i < 26; i++) {
            if (node->children[i] != nullptr) {
                current.push_back(char('a' + i));
                dfs(node->children[i], current, result);
                current.pop_back(); 
            }
        }
    }
    
    public:
    Trie() { root = new TrieNode(); }

    void insert(const string& word) {
        TrieNode* curr = root;
        for (char c : word) {
            if (c >= 'A' && c <= 'Z')
                c = c - 'A' + 'a';

            if (c < 'a' || c > 'z') 
                continue;

            int index = c - 'a';
            if (curr->children[index] == nullptr)
                curr->children[index] = new TrieNode();

            curr = curr->children[index];
        }
        curr->isEnd = true;
    }

    safeArray<string> getSuggestions(const string& input) {
        safeArray<string> result;
        if (input.empty()) return result;
    
        string lastWord = input;
        size_t lastSpace = input.find_last_of(' '); // extract last word only
        if (lastSpace != string::npos) {
            lastWord = input.substr(lastSpace + 1);
        }
        
        if (lastWord.empty()) return result;
        
        TrieNode* curr = root;
        string temp = lastWord;

        //navigate to the node for the last word prefix
        for (char c : lastWord) {
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a';
            }
            if (c < 'a' || c > 'z') {
                return result;
            }

            int idx = c - 'a';
            if (curr->children[idx] == nullptr) {
                return result; 
            }
            curr = curr->children[idx];
        }
        dfs(curr, temp, result); // getting completions for last word
        return result;
    }
};
#endif
