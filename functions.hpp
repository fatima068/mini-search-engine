#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <iostream>
#include <cstdlib>
#include<string.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include "dataStructures.hpp"
using namespace std;
#include <filesystem>
namespace fs = std::filesystem;

const string stopWords[] = {
    "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", "it", "its", "itself", "they", "them", "their", "theirs", "themselves", "what", "which", "who", "whom", "this", "that", "these", "those", "am", "is", "are", "was", "were", "be", "been", "being", "have", "has", "had", "having", "does", "did", "doing", "a", "an", "the", "and", "but", "because", "as", "until", "of", "at", "by", "for", "with", "about", "against", "between", "into", "through", "during", "before", "after", "above", "below", "to", "from", "up", "down", "in", "out", "on", "off", "over", "under", "again", "further", "then", "once", "here", "there", "when", "where", "why", "how", "all", "any", "both", "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than", "too", "very", "s", "t", "d", "can", "will", "just", "don", "should", "now"
};

const int numStopWords = sizeof(stopWords) / sizeof(stopWords[0]);

safeArray<string> getAllTextFiles() { // returns aray of names of all txt files in the specified folder - to automate file reading and new files detection 
    safeArray<string> fileList;
    for (const auto& entry : fs::directory_iterator("textFiles")) {
        if (entry.path().extension() == ".txt") {
            fileList.pushback(entry.path().string());
        }
    }
    return fileList;
}

string removePunctuation(const string& line) { // read line from file, clean it using removePunctuation() and return
    string cleaned;
    for (char ch : line) {
        if (!ispunct(ch)) {  
            cleaned += tolower(ch);
        }
    }
    return cleaned;
} // if the file has stuff like don’t or it’s, we want to keep the apostrophe but this function isnt doing that, consider it if time left end mei 

bool isStopWord(const string& word) {
    for (int i = 0; i < numStopWords; i++) {
        if (word == stopWords[i]) {
            return true;
        }
    }
    return false;
}

void tokenize(const string& line, safeArray<string>& tokenArr){ // tokenizing the cleaned line and pushing all tokens to the array of respective file 
    stringstream ss(line);
    string token;
    while (ss >> token) {
        if(isStopWord(token) == false)
        tokenArr.pushback(token);
    }
}

void generateFrequency(safeArray<string> &tokens, HashTable &freqTable) { // inserts each word in the given token array in the frequency hash table of that file 
    for (int i = 0; i < tokens.size(); i++)
        freqTable.insertWord(tokens[i]);
}

void buildInvertedIndex(safeArray<FileData>& allFiles, InvertedIndexHashTable& globalIndex) {
    for (int i = 0; i < allFiles.size(); i++) {
        FileData& current = allFiles[i];
        for (int j = 0; j < current.tokens.size(); j++) {
            string word = current.tokens[j];
            int freq = current.freqTable.getFrequency(word);
            globalIndex.insert(word, current.filename, freq);
        }
    }
    cout << endl<< "built global inverted index" << endl;
}

// void displayFileStats(const safeArray<FileData> &allFiles) {
//     for (int i = 0; i < allFiles.size(); i++) {
//         cout << endl<< "File: " << allFiles[i].filename << endl;
//         cout << "Total Tokens: " << allFiles[i].tokens.size() << endl;
//         cout << "Word Frequencies:" <<endl;
//         allFiles[i].freqTable.display();
//     }
// }

void displayResults(const safeArray<FileNode>& results, const string& query) {
    if (results.size() == 0) {
        cout << "no results found for '" <<query << "'\n";
        return;
    }

    cout << "\nResults for '" <<query << "':\n";
    for (int i = 0; i < results.size(); i++) {
        cout << i + 1 << ". " << results[i].filename <<": "<< results[i].frequency <<" occurrences" <<endl;
    }
}

void displayTopWords(FileData& file, int n, safeArray<string> &words, safeArray<int> &freqs) { // input n and file, then display top n word in that file
    cout << "\ntop " <<n << " words in file: " << file.filename << endl;

    //Extract all (word, frequency) pairs into arrays for sorting because sorting is easier on arrays
    // safeArray<string> words;
    // safeArray<int> freqs;
    for (int i = 0; i < file.freqTable.getSize(); i++) {
        // traverse each hash bucket
        HashNode* current = file.freqTable.getTable()[i];
        while (current != nullptr) {
            words.pushback(current->word);
            freqs.pushback(current->count);
            current = current->next;
        }
    } 

    //sort by frequency (selection sort in descending order)
    for (int i = 0; i < freqs.size() - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < freqs.size(); j++) {
            if (freqs[j] > freqs[maxIndex])
                maxIndex = j;
        }
        if (maxIndex != i) {
            int tempFreq = freqs[i];
            string tempWord = words[i];
            freqs[i] = freqs[maxIndex];
            words[i] = words[maxIndex];
            freqs[maxIndex] = tempFreq;
            words[maxIndex] = tempWord;
        }
    }

    // for (int i = 0; i < n; i++) {
    //     cout << i + 1 << ". " << words[i] << " (" << to_string(freqs[i]) << ")\n";
    // }
}

safeArray<FileNode> performSearch(InvertedIndexHashTable& globalIndex, const string& query){ //search a word and return sorted array of results
    safeArray<FileNode> results;
    FileNode* resultList = globalIndex.search(query);
    
    while (resultList != nullptr) {
        results.pushback(*resultList);
        resultList = resultList->next;
    }
    // Sort results by frequency (descending)
    for (int i = 0; i < results.size() - 1; i++) {
        int maxIndex = i;
        for (int j = i + 1; j < results.size(); j++) {
            if (results[j].frequency > results[maxIndex].frequency)
                maxIndex = j;
        }
        if (maxIndex != i) {
            FileNode temp = results[i];
            results[i] = results[maxIndex];
            results[maxIndex] = temp;
        }
    }
    return results;
}

// void menu(InvertedIndexHashTable& globalIndex, safeArray<FileData>& allFiles) {
//     int choice;
//     SearchHistory history;

//     while (true) {
//         cout << "\n-- MINI SEARCH ENGINE --"<< endl;
//         cout << "1. Search for a word"<< endl;
//         cout << "2. View top n words in a file"<< endl;
//         cout << "3. View search history"<< endl;
//         cout << "4. Clear search history"<< endl;
//         cout << "5. Exit"<< endl;
//         cout << "Enter your choice (1 - 5): ";
//         cin >> choice;

//         if (choice == 1) {
//             string query;
//             cout << "\nEnter the word to search: ";
//             cin >> query;
//             history.push(query);

//             safeArray<FileNode> results = performSearch(globalIndex, query);
//             displayResults(results, query);
//         } 
//         else if (choice == 2) {
//             cout << "\nAvailable files:\n";
//             for (int i = 0; i < allFiles.size(); i++) {
//                 cout << i + 1 << ". " << allFiles[i].filename << endl;
//             }

//             int fileChoice, n;
//             cout << "\nSelect file number: ";
//             cin >> fileChoice;
//             cout << "Enter N (number of top words to view): ";
//             cin >> n;

//             if (fileChoice > 0 && fileChoice <= allFiles.size()) {
//                 safeArray<string> words;
//                 safeArray<int> freqs;
//                 displayTopWords(allFiles[fileChoice - 1], n, words, freqs);
//                 //Display top n words
//                 for (int i = 0; i<n && i<freqs.size(); i++) {
//                     cout << i + 1 << ". " << words[i] << " (" << freqs[i] << ")\n";
//                 }
//             } else cout << "Invalid file choice\n";
//         } 
//         else if (choice == 3) {
//             history.display();
//         }
//         else if (choice == 4) {
//             history.clear();
//         }
//         else if (choice == 5) {
//             cout << "exiting search engine..." << endl;
//             break;
//         } 
//         else {
//             cout << "Enter a valid choice (1-5)" << endl;
//         }
//     }
// }
#endif