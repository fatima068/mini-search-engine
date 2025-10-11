#include <iostream>
#include <fstream>
#include "functions.hpp"
using namespace std;

int main(){
    safeArray<string> filenames = getAllTextFiles();
    safeArray<FileData> allFiles;

    for (int i = 0; i < filenames.size(); i++) {
        ifstream file(filenames[i]);
        if (!file) {
            cerr << "Error opening: " << filenames[i] << endl;
            continue;
        }

        FileData currentFile;
        currentFile.filename = filenames[i];

        string line;
        while (getline(file, line)) {
            string cleaned = removePunctuation(line);
            tokenize(cleaned, currentFile.tokens);
        }
        file.close();

        generateFrequency(currentFile.tokens, currentFile.freqList);
        allFiles.pushback(currentFile);

        displayFileStats(allFiles);
    }
}
