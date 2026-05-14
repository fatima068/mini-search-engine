#include <iostream>
#include <fstream>
#include <chrono>
#include "functions.hpp"
#include "dataStructures.hpp"
#include "customUI.hpp"
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
using namespace sf;
using namespace std;

int main() {
    SearchHistory searchHistory; 
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
        FileData currentFile;
        currentFile.filename = filenames[i];

        string line;
        int position = 0; //to track word positions in file
        while (getline(file, line)) {
            string cleaned = removePunctuation(line);
            //tokenize and track positions
            stringstream ss(cleaned);
            string token;
            while (ss >> token) {
                if(!isStopWord(token)) {
                    currentFile.tokens.pushback(token);
                    currentFile.freqTable.insertWord(token, position);
                    position++;
                }
            }
        }
        file.close();

        allFiles.pushback(currentFile);
    }
    InvertedIndexHashTable globalIndex;
    buildInvertedIndex(allFiles, globalIndex);
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    string preprocTime =  "Total pre-processing Time (cleaning, tokenization, inverted index building): " + to_string(duration.count()) + " seconds...";
    
    //simpleHashTableVisualization(globalIndex); 
    //cout <<endl<<endl <<" Complete Inverted Index: " << endl;
    //globalIndex.display();

    Trie trie;
    for (int i = 0; i < allFiles.size(); i++) {
        for (int j = 0; j < allFiles[i].tokens.size(); j++) {
            trie.insert(allFiles[i].tokens[j]);
        }
    } 
    RenderWindow window1(VideoMode({1800, 830}), "Mini Search Engine");
    window1.setFramerateLimit(60);
    Font font;
    if (!font.openFromFile("arial.ttf")) {
        throw runtime_error("Failed to load font (arial.ttf).");
    }
    Color bgColor(240, 240, 220);
    Color buttonColor(192, 192, 192); 
    Color buttonHoverColor(160, 160, 160); 
    Color buttonBorderColor(128, 128, 128);
    Color textColor(0, 0, 139); 
    Color titleColor(75, 0, 130); 
    
    Vector2u windowSize = window1.getSize();
    Vector2f windowCenter(windowSize.x / 2.f, windowSize.y / 2.f);
    
    RectangleShape topBar({1800.f, 40.f});
    topBar.setPosition({0.f, 0.f});
    topBar.setFillColor(buttonColor);
    topBar.setOutlineThickness(1);
    topBar.setOutlineColor(buttonBorderColor);
    
    myText titleText(font, "google", 80, {windowCenter.x - 140.f, 150.f}, titleColor, Text::Bold);
    myText subtitleText(font, "mini search engine", 20, {windowCenter.x-100.f, 250.f}, textColor, Text::Regular);
    myText preprocTimeText(font, preprocTime, 16, {10.f, 805.f}, textColor, Text::Regular);
    myText searchPrompt(font, "Enter search term:", 18, {600.f, 310.f}, textColor, Text::Regular);
    myText historyHeader(font, "Search History", 24, {100.f, 150.f}, titleColor, Text::Bold);
    myText rankedHeader(font, "Top Ranked Words", 24, {100.f, 150.f}, titleColor, Text::Bold);
    myText filePrompt(font, "File number (1-" + to_string(allFiles.size()) + "):", 16, {600.f, 230.f}, textColor, Text::Regular);
    myText nPrompt(font, "Number of top words:", 16, {900.f, 230.f}, textColor, Text::Regular);
    myText searchHeader(font, "Web Search", 24, {100.f, 320.f}, titleColor, Text::Bold);
    SuggestionBox searchSuggestionBox(font, {600.f, 390.f}, {500.f, 100.f});
    
    InputBox searchInputBox(font, {600.f, 340.f}, {500.f, 35.f});
    InputBox fileInputBox(font, {600.f, 260.f}, {200.f, 35.f});
    InputBox nInputBox(font, {900.f, 260.f}, {200.f, 35.f});
    
    Button searchButton(font, "SEARCH", {1180.f, 355.f}, {80.f, 35.f}, buttonBorderColor, textColor);
    Button button1(font, "Search Word", {600.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    button1.setTextPosition({600.f - 50.f, 400.f - 10.f});
    Button button2(font, "View Search History", {900.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    button2.setTextPosition({900.f - 80.f, 400.f - 10.f});
    Button button3(font, "Top N Ranked Words", {1200.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    button3.setTextPosition({1200.f - 80.f, 400.f - 10.f});
    Button backButton(font, "BACK", {100.f, 100.f}, {80.f, 30.f}, buttonBorderColor, textColor);
    backButton.setTextPosition({100.f - 23.f, 100.f - 10.f});
    Button clearAllHistoryButton(font, "CLEAR ALL HISTORY", {1100.f, 750.f}, {180.f, 30.f}, buttonBorderColor, textColor);
    clearAllHistoryButton.setTextPosition({1100.f - 85.f, 750.f - 10.f});
    Button clearRecentHistoryButton(font, "CLEAR MOST RECENT HISTORY", {800.f, 750.f}, {280.f, 30.f}, buttonBorderColor, textColor);
    clearRecentHistoryButton.setTextPosition({800.f - 135.f, 750.f - 10.f});
    Button rankedSearchButton(font, "GET WORDS", {1250.f, 275.f}, {120.f, 35.f}, buttonBorderColor, textColor);
    rankedSearchButton.setTextPosition({1250.f - 50.f, 275.f - 10.f});

    RectangleShape statusBar({1800.f, 30.f});
    statusBar.setPosition({0.f, 800.f});
    statusBar.setFillColor(buttonColor);
    statusBar.setOutlineThickness(1);
    statusBar.setOutlineColor(buttonBorderColor);

    ScrollablePanel searchResultsPanel(font, Vector2f(100.f, 400.f), Vector2f(1600.f, 400.f));
    ScrollablePanel rankedResultsPanel(font, Vector2f(100.f, 320.f), Vector2f(1600.f, 460.f));

    RectangleShape historyBox({1600.f, 500.f});
    historyBox.setPosition({100.f, 200.f});
    historyBox.setFillColor(Color::White);
    historyBox.setOutlineThickness(2);
    historyBox.setOutlineColor(buttonBorderColor);

    safeArray<string> searchResults;
    safeArray<string> suggestionList;
    safeArray<string> rankedOutput; 
    enum class Screen {MAIN, SEARCH, HISTORY, RANKED };
    Screen currentScreen = Screen::MAIN;

    auto performSearchAction = [&]() {
        cout << "Search performed!" << endl;
        string currentInput = searchInputBox.getText();
        safeArray<FileNode> results;
        string searchType = "regular";
        
        if (currentInput.find(' ')!= string::npos) {
            results = performPhraseSearch(globalIndex, currentInput, allFiles);
            searchType = "phrase";
        }
        else { // search entire exact word or substring
            safeArray<FileNode> exactResults = performSearch(globalIndex, currentInput);
            safeArray<FileNode> substringResults = performSubstringSearch(globalIndex, currentInput);
            results = exactResults;
            // Add substring results that aren't already in exact results
            for (int i = 0; i < substringResults.size(); i++) {
                bool alreadyExists = false;
                for (int j = 0; j < exactResults.size(); j++) {
                    if (substringResults[i].filename == exactResults[j].filename) {
                        alreadyExists = true;
                        break;
                    }
                }
                if (!alreadyExists) {
                    results.pushback(substringResults[i]);
                }
            }
            
            if (exactResults.size() > 0 && substringResults.size() > 0) {
                searchType = "combined";
            } 
            else if (exactResults.size() > 0) {
                searchType = "exact";
            } 
            else {
                searchType = "substring";
            }
        }
        searchResults.clear();

        if (currentInput.empty()) { searchResults.pushback("Please enter a search term"); } 
        else if (results.size() == 0) {
            searchResults.pushback(string("No results found for '") + currentInput+string("'"));
            searchHistory.push(currentInput);
        }
        else {
            searchHistory.push(currentInput);
            
            for (int i = 0; i < results.size(); ++i) {
                string line = to_string(i + 1) + ". " + results[i].filename;
                
                if (searchType == "phrase") {
                    int phraseCount = results[i].frequency / 1000000;
                    line += " (" + to_string(phraseCount) + " phrase matches)";
                }
                else if (searchType == "substring") {
                    line += " (" + to_string(results[i].frequency) + " occurrences)";
                }
                else { // for exact match
                    int matchCount = results[i].frequency / 10000;
                    int actualFrequency = results[i].frequency % 10000;
                    line += " (" + to_string(matchCount) + " words matched, " + to_string(actualFrequency) + " occurrences)";
                }
                
                searchResults.pushback(line);
            }
        }
        searchResultsPanel.setContent(searchResults);
        searchResultsPanel.setScrollOffset(0.f);
    };

    auto performRankedSearchAction = [&]() {
        string fileInput = fileInputBox.getText();
        string nInput = nInputBox.getText();
        
        if (fileInput.empty() || nInput.empty()) {
            rankedOutput.clear();
            rankedOutput.pushback("Enter both file number and N(num of words u want to view)");
        }
        else {
            try {
                int fileN = stoi(fileInput);
                int Nwords = stoi(nInput);
                
                if (fileN < 1 || fileN > allFiles.size()) {
                    rankedOutput.clear();
                    rankedOutput.pushback("File number must be between 1 and " + to_string(allFiles.size()));
                } else if(Nwords < 1){
                    rankedOutput.clear();
                    rankedOutput.pushback("enter N > 0");
                } else {
                    cout << "File: " << fileN << ", N: " << Nwords << endl;
                    int fileIndex = fileN - 1;
                    safeArray<string> words;
                    safeArray<int> freqs;
                    displayTopWords(allFiles[fileIndex], Nwords, words, freqs);

                    rankedOutput.clear();
                    for (int i = 0; i < Nwords && i < freqs.size(); i++) {
                        string line = to_string(i + 1) + ". " + words[i] + " (" + to_string(freqs[i]) + ")";
                        rankedOutput.pushback(line);
                    }
                }
            } catch (...) {
                rankedOutput.clear();
                rankedOutput.pushback("invalid number entered");
            }
        }
        rankedResultsPanel.setContent(rankedOutput);
        rankedResultsPanel.setScrollOffset(0.f);
    };

    while (window1.isOpen()) {
        while (std::optional<Event> event = window1.pollEvent()) {
            if (event->is<Event::Closed>())
                window1.close();

            Vector2i mousePos = Mouse::getPosition(window1);

            if (currentScreen == Screen::MAIN) {
                button1.setHovered(button1.contains(mousePos));
                button2.setHovered(button2.contains(mousePos));
                button3.setHovered(button3.contains(mousePos));

                if (event->is<Event::MouseButtonPressed>()) {
                    if (button1.contains(mousePos)) {
                        cout << "Search Word button clicked!" << endl;
                        currentScreen = Screen::SEARCH;
                        searchInputBox.clear();
                        searchResults.clear();
                        searchResultsPanel.setScrollOffset(0.f);
                    } 
                    else if (button2.contains(mousePos)) {
                        cout << "View History button clicked!" << endl;
                        currentScreen = Screen::HISTORY;
                    } 
                    else if (button3.contains(mousePos)) {
                        cout << "N Ranked Words button clicked!" << endl;
                        currentScreen = Screen::RANKED;
                        rankedOutput.clear();
                        fileInputBox.clear();
                        nInputBox.clear();
                        rankedResultsPanel.setScrollOffset(0.f);
                    }
                }
            } 
            
            else if (currentScreen == Screen::SEARCH) {
                backButton.setHovered(backButton.contains(mousePos));
                searchButton.setHovered(searchButton.contains(mousePos));
                if (searchInputBox.getText().empty() && !searchResults.isEmpty()) {
                    searchResults.clear();
                    searchResultsPanel.setContent(searchResults);
                }
                searchInputBox.handleEvent(*event, {(float)(mousePos.x), (float)(mousePos.y)}, searchSuggestionBox, suggestionList, trie);
                string currentTypedText = searchInputBox.getText();

                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>())
                    searchResultsPanel.handleScroll(wheelEvent->delta);

                if (event->is<Event::MouseButtonPressed>()) {
                    string selectedSuggestion;
                    if (searchSuggestionBox.suggestionClicked({(float)(mousePos.x), (float)(mousePos.y)}, selectedSuggestion)) {
                        //get current text and find the last word position
                        string currentText = searchInputBox.getText();
                        size_t lastSpace = currentText.find_last_of(' ');
                        
                        if (lastSpace != string::npos) {//replace only last word
                            string newText = currentText.substr(0, lastSpace + 1) + selectedSuggestion;
                            searchInputBox.setText(newText);
                        } 
                        else { searchInputBox.setText(selectedSuggestion); }//no spaces so replace entire text
                        
                        searchSuggestionBox.hide();
                    }

                    if (backButton.contains(mousePos)) {
                        currentScreen = Screen::MAIN;   
                        searchResultsPanel.setScrollOffset(0.f);
                        searchResults.clear(); 
                        searchSuggestionBox.hide();
                    } 
                    else if (searchButton.contains(mousePos)) {
                        searchSuggestionBox.hide();
                        performSearchAction();
                    }  
                }
                else if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {    
                    if (keyEvent->code == Keyboard::Key::Enter) {
                        searchSuggestionBox.hide();
                        performSearchAction();
                    }
                }
            }

            else if (currentScreen == Screen::HISTORY) { 
                backButton.setHovered(backButton.contains(mousePos));
                clearAllHistoryButton.setHovered(clearAllHistoryButton.contains(mousePos));
                clearRecentHistoryButton.setHovered(clearRecentHistoryButton.contains(mousePos));

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) {
                        currentScreen = Screen::MAIN;
                    }
                    else if (clearAllHistoryButton.contains(mousePos)) {
                        searchHistory.clear();
                    }
                    else if (clearRecentHistoryButton.contains(mousePos)) {
                        if (searchHistory.peek() != nullptr) {
                            searchHistory.pop();
                        }
                    }
                }
            }
            else if (currentScreen == Screen::RANKED) {
                backButton.setHovered(backButton.contains(mousePos));
                rankedSearchButton.setHovered(rankedSearchButton.contains(mousePos));
                if (fileInputBox.getText().empty() && nInputBox.getText().empty() && !rankedOutput.isEmpty()) {
                    rankedOutput.clear();
                    rankedResultsPanel.setContent(rankedOutput);
                }
                fileInputBox.handleEvent(*event, {(float)(mousePos.x), (float)(mousePos.y)});
                nInputBox.handleEvent(*event, {(float)(mousePos.x), (float)(mousePos.y)});

                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    rankedResultsPanel.handleScroll(wheelEvent->delta);
                }

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) {
                        currentScreen = Screen::MAIN;
                        rankedResultsPanel.setScrollOffset(0.f);
                        rankedOutput.clear();
                    }
                    else if (rankedSearchButton.contains(mousePos)) {
                        performRankedSearchAction();
                    }
                } 
                else if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                    if (keyEvent->code == Keyboard::Key::Enter) {
                        performRankedSearchAction();
                    }
                }
            }
        }
        //RENDERING (basically drawing everything)
        window1.clear(bgColor);
        if (currentScreen == Screen::MAIN) {
            window1.draw(topBar);
            titleText.draw(window1);
            subtitleText.draw(window1);
            button1.draw(window1);
            button2.draw(window1);
            button3.draw(window1);
            window1.draw(statusBar);
            preprocTimeText.draw(window1);
        }
        else {
            backButton.draw(window1);
            
            if (currentScreen == Screen::SEARCH) {
                searchPrompt.draw(window1);
                searchInputBox.draw(window1);
                searchButton.draw(window1);
                if (searchResults.isEmpty()) {
                    searchResults.pushback(" ");
                    searchResultsPanel.setContent(searchResults); 
                }
                searchResultsPanel.draw(window1, textColor);
                searchHeader.draw(window1);
                searchSuggestionBox.draw(window1);
            }
            else if (currentScreen == Screen::HISTORY) {
                historyHeader.draw(window1);
                window1.draw(historyBox);
                clearAllHistoryButton.draw(window1);
                clearRecentHistoryButton.draw(window1);
                float y = 220.f;
                SearchHistoryNode* current = searchHistory.peek();
                int count = 1;
                
                while (current != nullptr && y <= 680.f) {
                    string historyEntry = to_string(count) + ". " + current->query;
                    myText historyItem(font, historyEntry, 16, {120.f, y}, Color::Black, Text::Regular);
                    historyItem.draw(window1);
                    
                    current = current->next;
                    y += 25.f;
                    count++;
                }

                if (searchHistory.peek() == nullptr) {
                    myText noHistoryText(font, "No search history yet", 18, {200.f, 400.f}, textColor, Text::Regular);
                    noHistoryText.draw(window1);
                }
            }
            else if (currentScreen == Screen::RANKED) {
                rankedHeader.draw(window1);
                filePrompt.draw(window1);
                fileInputBox.draw(window1);
                nPrompt.draw(window1);
                nInputBox.draw(window1);
                rankedSearchButton.draw(window1);
                if (rankedOutput.isEmpty()) {  
                    rankedOutput.pushback(" ");
                    rankedResultsPanel.setContent(rankedOutput); 
                }
                rankedResultsPanel.draw(window1, textColor);
            }
        }
        window1.display();
    }
}   