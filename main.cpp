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
        while (getline(file, line)) {
            string cleaned = removePunctuation(line);
            tokenize(cleaned, currentFile.tokens);
        }
        file.close();

        generateFrequency(currentFile.tokens, currentFile.freqTable);
        allFiles.pushback(currentFile);
    }
    InvertedIndexHashTable globalIndex;
    buildInvertedIndex(allFiles, globalIndex);

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    string preprocTime =  "Total pre-processing Time (cleaning, tokenization, inverted index building): " + to_string(duration.count()) + " seconds...";

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
    
    myText titleText(font, "boogle.", 80, {windowCenter.x - 140.f, 150.f}, titleColor, Text::Bold);
    myText subtitleText(font, "mini search engine", 20, {windowCenter.x-100.f, 240.f}, textColor, Text::Regular);
    myText footerText(font, "boogle - copyright 1995", 14, {700.f, 520.f}, textColor, Text::Regular);
    myText preprocTimeText(font, preprocTime, 16, {10.f, 805.f}, textColor, Text::Regular);
    myText searchPrompt(font, "Enter search term:", 18, {600.f, 320.f}, textColor, Text::Regular);
    myText historyHeader(font, "Search History", 24, {100.f, 150.f}, titleColor, Text::Bold);
    myText rankedHeader(font, "Top Ranked Words", 24, {100.f, 150.f}, titleColor, Text::Bold);
    myText filePrompt(font, "File number (1-" + to_string(allFiles.size()) + "):", 16, {600.f, 270.f}, textColor, Text::Regular);
    myText nPrompt(font, "Number of top words:", 16, {900.f, 270.f}, textColor, Text::Regular);
    myText searchHeader(font, "Web Search", 24, {100.f, 320.f}, titleColor, Text::Bold);
    
    InputBox searchInputBox(font, {600.f, 350.f}, {500.f, 35.f});
    InputBox fileInputBox(font, {600.f, 300.f}, {200.f, 35.f});
    InputBox nInputBox(font, {900.f, 300.f}, {200.f, 35.f});
    
    Button searchButton(font, "SEARCH", {1120.f, 350.f}, {80.f, 35.f}, buttonBorderColor, textColor);
    Button button1(font, "Search Word", {600.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button button2(font, "View Search History", {900.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button button3(font, "Top N Ranked Words", {1200.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button backButton(font, "BACK", {100.f, 100.f}, {80.f, 30.f}, buttonBorderColor, textColor);
    Button clearAllHistoryButton(font, "CLEAR ALL HISTORY", {1200.f, 750.f}, {160.f, 30.f}, buttonBorderColor, textColor);
    Button clearRecentHistoryButton(font, "CLEAR MOST RECENT HISTORY", {900.f, 750.f}, {160.f, 30.f}, buttonBorderColor, textColor);
    Button rankedSearchButton(font, "GET WORDS", {1150.f, 300.f}, {120.f, 35.f}, buttonBorderColor, textColor);

    RectangleShape statusBar({1800.f, 30.f});
    statusBar.setPosition({0.f, 800.f});
    statusBar.setFillColor(buttonColor);
    statusBar.setOutlineThickness(1);
    statusBar.setOutlineColor(buttonBorderColor);

    RectangleShape searchResultsBox({1600.f, 400.f});
    searchResultsBox.setPosition({100.f, 400.f});
    searchResultsBox.setFillColor(Color::White);
    searchResultsBox.setOutlineThickness(2);
    searchResultsBox.setOutlineColor(buttonBorderColor);

    RectangleShape searchScrollbar({15.f, 50.f});
    searchScrollbar.setPosition({1690.f, 450.f});
    searchScrollbar.setFillColor(Color(128, 128, 128, 180));
    searchScrollbar.setOutlineThickness(1);
    searchScrollbar.setOutlineColor(Color::Black);

    RectangleShape historyBox({1600.f, 500.f});
    historyBox.setPosition({100.f, 200.f});
    historyBox.setFillColor(Color::White);
    historyBox.setOutlineThickness(2);
    historyBox.setOutlineColor(buttonBorderColor);

    RectangleShape rankedResultsBox({1600.f, 430.f});
    rankedResultsBox.setPosition({100.f, 350.f});
    rankedResultsBox.setFillColor(Color::White);
    rankedResultsBox.setOutlineThickness(2);
    rankedResultsBox.setOutlineColor(buttonBorderColor);

    RectangleShape rankedScrollbar({15.f, 50.f});
    rankedScrollbar.setPosition({1690.f, 350.f});
    rankedScrollbar.setFillColor(Color(128, 128, 128, 180));
    rankedScrollbar.setOutlineThickness(1);
    rankedScrollbar.setOutlineColor(Color::Black);

    safeArray<string> searchResults;
    safeArray<string> rankedOutput; // for n ranked screen
    float searchScrollOffset = 0.f;
    float rankedScrollOffset = 0.f;
    enum class Screen {MAIN, SEARCH, HISTORY, RANKED };
    Screen currentScreen = Screen::MAIN;

    auto performSearchAction = [&]() {
        cout << "Search performed!" << endl;
        string currentInput = searchInputBox.getText();
        safeArray<FileNode> results = performSearch(globalIndex, currentInput);
        searchResults.clear();

        if (currentInput.empty()) {
            searchResults.pushback("Please enter a search term");
        } 
        else if (results.size() == 0) {
            searchResults.pushback(string("No results found for '") + currentInput + string("'"));
            searchHistory.push(currentInput);
        }
        else {
            searchHistory.push(currentInput);
            
            for (int i = 0; i < results.size(); ++i) {
                int matchCount = results[i].frequency / 10000;
                int actualFrequency = results[i].frequency % 10000;
                string line = to_string(i + 1) + ". " + results[i].filename + 
                            " (" + to_string(matchCount) + " words matched, " + 
                            to_string(actualFrequency) + " occurrences)";
                searchResults.pushback(line);
            }
        }
        searchScrollOffset = 0.f;
    };

    auto performRankedSearchAction = [&]() {
        cout << "Get words performed!" << endl;
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
        rankedScrollOffset = 0.f;
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
                        searchScrollOffset = 0.f;
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
                        rankedScrollOffset = 0.f;
                    }
                }
            } 
            
            else if (currentScreen == Screen::SEARCH) {
                backButton.setHovered(backButton.contains(mousePos));
                searchButton.setHovered(searchButton.contains(mousePos));
                searchInputBox.handleEvent(*event, {mousePos.x, mousePos.y});

                // Scroll events
                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    if (wheelEvent->delta > 0) {
                        searchScrollOffset = max(0.f, searchScrollOffset - 20.f);
                    } else {
                        float maxScroll = max(0.f, static_cast<float>(searchResults.size()) * 25.f - 400.f);
                        searchScrollOffset = min(maxScroll, searchScrollOffset + 20.f);
                    }
                }

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) {
                        currentScreen = Screen::MAIN;
                        searchScrollOffset = 0.f;
                    } 
                    else if (searchButton.contains(mousePos)) {
                        performSearchAction();
                    }  
                }
                else if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                    if (keyEvent->code == Keyboard::Key::Enter) {
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
                fileInputBox.handleEvent(*event, {mousePos.x, mousePos.y});
                nInputBox.handleEvent(*event, {mousePos.x, mousePos.y});

                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    if (wheelEvent->delta > 0) {
                        rankedScrollOffset = max(0.f, rankedScrollOffset - 20.f);
                    } else {
                        float maxScroll = max(0.f, static_cast<float>(rankedOutput.size()) * 25.f - 400.f);
                        rankedScrollOffset = min(maxScroll, rankedScrollOffset + 20.f);
                    }
                }

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) {
                        currentScreen = Screen::MAIN;
                        rankedScrollOffset = 0.f;
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
            preprocTimeText.draw(window1);
            button1.draw(window1);
            button2.draw(window1);
            button3.draw(window1);
            window1.draw(statusBar);
            footerText.draw(window1);
        }
        else {
            backButton.draw(window1);
            
            if (currentScreen == Screen::SEARCH) {
                searchPrompt.draw(window1);
                searchInputBox.draw(window1);
                searchButton.draw(window1);
                window1.draw(searchResultsBox);

                // Scrollbar
                float searchContentHeight = static_cast<float>(searchResults.size()) * 25.f;
                float searchVisibleHeight = 400.f;
                if (searchContentHeight > searchVisibleHeight) {
                    float scrollbarHeight = (searchVisibleHeight/searchContentHeight) * 400.f;
                    float scrollbarPos = (searchScrollOffset / (searchContentHeight - searchVisibleHeight)) * (400.f - scrollbarHeight);
                    
                    searchScrollbar.setSize({15.f, scrollbarHeight});
                    searchScrollbar.setPosition({1690.f, 450.f + scrollbarPos});
                    window1.draw(searchScrollbar);
                }

                float y = 460.f - searchScrollOffset;
                for (int i = 0; i < searchResults.size(); i++) {
                    if (y >= 460.f - 25.f && y <= 830.f) {
                        myText resultLine(font, searchResults[i], 16, {120.f, y}, textColor, Text::Regular);
                        resultLine.draw(window1);
                    }
                    y += 25.f;
                    if (y > 830.f) break;
                }
                searchHeader.draw(window1);
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
                window1.draw(rankedResultsBox);

                float rankedContentHeight = static_cast<float>(rankedOutput.size()) * 25.f;
                float rankedVisibleHeight = 400.f;
                if (rankedContentHeight > rankedVisibleHeight) {
                    float scrollbarHeight = (rankedVisibleHeight / rankedContentHeight) * 400.f;
                    float maxScrollOffset = rankedContentHeight- rankedVisibleHeight;
                    float scrollbarPos = (rankedScrollOffset/maxScrollOffset)*(400.f - scrollbarHeight);
                    
                    rankedScrollbar.setSize({15.f,scrollbarHeight});
                    rankedScrollbar.setPosition({1690.f,350.f +scrollbarPos});
                    window1.draw(rankedScrollbar);
                }

                float y = 360.f - rankedScrollOffset;
                for (int i = 0; i < rankedOutput.size(); i++){
                    if (y >= 360.f && y <= 760.f) { //drawing visible items
                        myText resultLine(font, rankedOutput[i], 16, {120.f, y}, textColor, Text::Regular);
                        resultLine.draw(window1);
                    }
                    y += 25.f;
                    if (y >760.f) break;
                }
            }
        }
        window1.display();
    }
}
