#include <iostream>
#include <cstdlib>
#include<string.h>
using namespace std;


// 1. DYNAMIC SAFE ARRAY 
template <typename T>
class safeArray {
    private:
    int ncols; 
    T *dynamicArray;

    public:
    safeArray() : ncols(0), dynamicArray(NULL) {}

    //copy constructor (rule of 3 implementation)
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

    //destructor
    ~safeArray (){
        delete [] dynamicArray; //free allocated memory
        dynamicArray=NULL;
    }

    // provides bound checking before accessing array 
    T &operator[] (int i) const {
        if(i<0 || i>ncols-1 ) {
            cout << "Boundary Error\n";
            exit(1);
        }
        return dynamicArray[i];
    }

    //assignment operator
    safeArray &operator= (const safeArray & rhs) {
        if (this == &rhs) // Self assignment check
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
};

struct HashNode {
    string word;
    int count;
    HashNode* next;

    HashNode(string word, int count) {  
        this->word = word;
        this->count = count;
        this->next = nullptr;
    }
};


class HashTable {
    private:
    int tableSize;
    HashNode** table;

    int hashFunction(const string& key) {
        unsigned long hash = 0;
        for (char c : key)
            hash = (hash * 31 + c) % tableSize;
        return hash;
    }

    public:
    HashTable(int size = 101) {
        tableSize = size;
        table = new HashNode*[tableSize];
        for (int i = 0; i< tableSize; i++) {
            table[i] = nullptr;
        } 
    }

    // copy constructor
    HashTable(const HashTable &rhs) {
        tableSize = rhs.tableSize;
        table = new HashNode*[tableSize];
        for (int i = 0; i < tableSize; i++) {
            if (rhs.table[i] == nullptr) {
                table[i] = nullptr;
            } else {
                // Deep copy the linked list
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

    // Copy assignment operator
    HashTable& operator=(const HashTable &rhs) {
        if (this == &rhs)
            return *this;

        // 1. Clean up existing data
        for (int i = 0; i < tableSize; i++) {
            HashNode* current = table[i];
            while (current != nullptr) {
                HashNode* next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;

        // 2. Copy size and allocate new table
        tableSize = rhs.tableSize;
        table = new HashNode*[tableSize];

        // 3. Deep copy each bucket (same logic as copy constructor)
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


    void insertWord(const string& word) {
        int index = hashFunction(word);
        HashNode* head = table[index];
        // Search if word already exists in the chain and if it does increase count 
        HashNode* current = head;
        while (current != nullptr) {
            if (current->word == word) {
                current->count++;
                return;
            }
            current = current->next;
        }
        // create a new node and insert at head if value doesnt already exisst
        HashNode* newNode = new HashNode(word, 1);
        newNode->next = head;
        table[index] = newNode;
    }

    int getFrequency(const string& word) {
        int index = hashFunction(word);
        HashNode* current = table[index];

        while (current != nullptr) {
            if (current->word == word)
                return current->count;
            current = current->next;
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
            HashNode *current = table[i];
            while (current != nullptr) {
                HashNode *next = current->next;
                delete current;
                current = next;
            }
        }
        delete[] table;
    }
};

struct FileData {
    string filename;
    safeArray<string> tokens;
    HashTable freqTable;
};