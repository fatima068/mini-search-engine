#include <iostream>
#include <cstdlib>
#include<string.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include "dataStructures.hpp"
using namespace std;

/*we want to read line from txt, clean it using removePunctuation(), then return that line and parse that*/
string removePunctuation(const string& line) {
    string cleaned;
    for (char ch : line) {
        if (!ispunct(ch)) {  
            cleaned += tolower(ch);
        }
    }
    return cleaned;
}

void tokenize(const string& line, safeArray<string>& tokenArr){
    stringstream ss(line);
    string token;
    while (ss >> token) {
        tokenArr.pushback(token);
    }
}
