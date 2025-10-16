#include <iostream>
#include <fstream>
#include <chrono>
#include "functions.hpp"
using namespace std;

int main() {
    auto start = chrono::high_resolution_clock::now();

    safeArray<string> filenames = getAllTextFiles();
    safeArray<FileData> allFiles;

    cout << "Detected " << filenames.size() << " text files.\n";

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
        cout << "Processed file: " << currentFile.filename  << " with " << currentFile.tokens.size() << " tokens.\n";

        generateFrequency(currentFile.tokens, currentFile.freqTable);

        allFiles.pushback(currentFile);
    }
    
    displayFileStats(allFiles);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << "\nExecution Time: " << duration.count() << " seconds.\n";
}
