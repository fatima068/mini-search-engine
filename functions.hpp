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

void displayTopWords(FileData& file, int n, safeArray<string> &words, safeArray<int> &freqs) { // input n and file, then display top n word in that file
    cout << "\ntop " <<n << " words in file: " << file.filename << endl;

    //Extract all (word, frequency) pairs into arrays for sorting because sorting is easier on arrays
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
}

safeArray<FileNode> performSearch(InvertedIndexHashTable& globalIndex, const string& query) { //take inverted index & query and return array of results
    safeArray<FileNode> results;
    safeArray<string> queryWords; //tokenize query
    stringstream ss(query);
    string word;
    while (ss>>word) { queryWords.pushback(word);}
    // if (queryWords.size() == 0) { return results; }
    
    struct EnhancedFileNode {
        string filename;
        int matchCount;
        int totalFrequency;
        int searchScore;  //matchCount*10000 + totalfrequency
    };
    
    safeArray<EnhancedFileNode> enhancedResults;
        for (int i = 0; i < queryWords.size(); i++) {
        FileNode* wordResults = globalIndex.search(queryWords[i]);
        FileNode* current = wordResults;
        
        while (current != nullptr) {
            //new entry if file doesnt already exist in results
            bool found = false;
            for (int j = 0; j < enhancedResults.size(); j++) {
                if (enhancedResults[j].filename == current->filename) {
                    enhancedResults[j].matchCount++;
                    enhancedResults[j].totalFrequency += current->frequency;
                    found = true;
                    break;
                }
            }
            if (!found) {
                EnhancedFileNode newFile;
                newFile.filename = current->filename;
                newFile.matchCount = 1;
                newFile.totalFrequency = current->frequency;
                enhancedResults.pushback(newFile);
            }
            current = current->next;
        }
    }
    
    for (int i = 0; i < enhancedResults.size(); i++) {
        //files with more matching words at the top, then sort by total frequency
        enhancedResults[i].searchScore = enhancedResults[i].matchCount * 10000 + enhancedResults[i].totalFrequency;
        
        FileNode result;
        result.filename = enhancedResults[i].filename;
        result.frequency = enhancedResults[i].searchScore; //score for sorting
        result.next = nullptr;
        results.pushback(result);
    }
    
    //sorting by search score -descending order
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
#endif