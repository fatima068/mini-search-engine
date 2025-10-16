#include <iostream>
#include <fstream>
#include <chrono>
#include "functions.hpp"
using namespace std;

int main() {
    auto start = chrono::high_resolution_clock::now();

    safeArray<string> filenames = getAllTextFiles();
    safeArray<FileData> allFiles;

    cout << filenames.size() << " text files detected" << endl;

    for (int i = 0; i < filenames.size(); i++) {
        ifstream file(filenames[i]);
        if (!file) {
            cerr << "error opening: " << filenames[i] << endl;
            continue;
        }

        FileData currentFile; // fileData obj has filename, string array of tokens, hash table for storing frequencyof each word in that file
        currentFile.filename = filenames[i];

        string line;
        while (getline(file, line)) {
            string cleaned = removePunctuation(line);
            tokenize(cleaned, currentFile.tokens); // creating tokens for the file being currently processed and pushing in token array
        } 
        file.close();

        generateFrequency(currentFile.tokens, currentFile.freqTable); // generating frequency hash table for current file
        allFiles.pushback(currentFile); // storing data of the file which has been processed before moving on to the next one 
    }

    InvertedIndexHashTable globalIndex;
    buildInvertedIndex(allFiles, globalIndex);

    // globalIndex.display(); // display the inverted index

    string testWord; // testing the search functionality using the inverted index created
    cout << endl << "Enter a word to search (or type 'exit'): ";
    while (true) {
        cin >> testWord;
        if (testWord == "exit") break;

        FileNode* result = globalIndex.search(testWord); 
        if (result == nullptr) {
            cout << "Word not found in any file"<< endl;
        } else {
            cout << "\nfiles containing '" << testWord << "':" << endl;
            while (result != nullptr) {
                cout << "  " << result->filename << " (" << result->frequency << " occurrences)\n";
                result = result->next;
            }
        }
        cout << "\nEnter another word to search (or 'exit'): ";
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    cout << "\nExecution Time: " << duration.count() << " seconds.\n";



}


