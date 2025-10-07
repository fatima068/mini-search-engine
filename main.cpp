#include <iostream>
#include <fstream>
#include "functions.hpp"
using namespace std;

int main(){
    ifstream book1File("textFiles/book1.txt");
    if (!book1File) {
        cerr << "error opening book1File!" << endl;
        return 1;
    }

    safeArray<string> book1Tokens;
    string line;
    while (getline(book1File, line)) {
        string cleanedLine = removePunctuation(line);
        tokenize(cleanedLine, book1Tokens);
    }
    book1File.close();

    book1Tokens.print(); 
}