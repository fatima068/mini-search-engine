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
    "i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", "it", "its", "itself", "they", "them", "their", "theirs", "themselves", "what", "which", "who", "whom", "this", "that", "these", "those", "am", "is", "are", "was", "were", "be", "been", "being", "have", "has", "had", "having", "does", "did", "doing", "a", "an", "the", "and", "but", "because", "as", "until", "of", "at", "by", "for", "with", "about", "against", "between", "into", "through", "during", "before", "after", "above", "below", "to", "from", "up", "down", "in", "out", "on", "off", "over", "under", "again", "further", "then", "once", "here", "there", "when", "where", "why", "how", "all", "any", "both", "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than", "too", "very", "s", "t", "can", "will", "just", "don", "should", "now"
};

const int numStopWords = sizeof(stopWords) / sizeof(stopWords[0]);

// read line from txt, clean it using removePunctuation(), then return that line and parse that
string removePunctuation(const string& line) {
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

void tokenize(const string& line, safeArray<string>& tokenArr){
    stringstream ss(line);
    string token;
    while (ss >> token) {
        if(isStopWord(token) == false)
        tokenArr.pushback(token);
    }
}


safeArray<string> getAllTextFiles() { // returns aray of all txt files in the specified folder - to automate file reading and new files detection 
    safeArray<string> fileList;
    for (const auto& entry : fs::directory_iterator("textFiles")) {
        if (entry.path().extension() == ".txt") {
            fileList.pushback(entry.path().string());
        }
    }
    return fileList;
}

void generateFrequency(safeArray<string>& tokens, safeArray<WordCount>& freqList) {
    for (int i = 0; i < tokens.size(); i++) {
        string word = tokens[i];
        bool found = false;

        for (int j = 0; j < freqList.size(); j++) {
            if (freqList[j].word == word) {
                freqList[j].count++;
                found = true;
                break;
            }
        }

        if (!found) {
            WordCount wc = { word, 1 };
            freqList.pushback(wc);
        }
    }
}

void displayFileStats(const safeArray<FileData>& allFiles) {
    for (int i = 0; i < allFiles.size(); i++) {
        cout << "\nFile: " << allFiles[i].filename << endl;
        cout << "----------------------------------\n";
        cout << "Total Tokens: " << allFiles[i].tokens.size() << endl;
        cout << "Unique Words: " << allFiles[i].freqList.size() << endl;
        cout << "Word Frequencies:\n";
        for (int j = 0; j < allFiles[i].freqList.size(); j++) {
            cout << allFiles[i].freqList[j].word << " -> "
                 << allFiles[i].freqList[j].count << endl;
        }
    }
}
