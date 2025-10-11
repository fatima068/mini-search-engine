#include <iostream>
#include <cstdlib>
#include<string.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include "dataStructures.hpp"
using namespace std;

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

void tokenize(const string& line, safeArray<string>& tokenArr){
    stringstream ss(line);
    string token;
    while (ss >> token) {
        tokenArr.pushback(token);
    }
}