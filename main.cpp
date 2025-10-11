#include <iostream>
#include <fstream>
#include "functions.hpp"
using namespace std;

int main(){
    // manual file reading, will not read any files except the ones in this array so if later i add more files i will have to manually ad them here
    // see automatic file reading later
    string filenames[] = {"textFiles/book1.txt", "textFiles/book2.txt", "textFiles/book3.txt"};
    safeArray<string> allTokens;

    for (string fname : filenames) {
        ifstream file(fname);
        if (!file) {
            cerr << "error opening " << fname << endl;
            continue;
        }
        string line;
        while (getline(file, line)) {
            string cleanedLine = removePunctuation(line);
            tokenize(cleanedLine, allTokens);
        }
        file.close();
    }
    allTokens.print();
}