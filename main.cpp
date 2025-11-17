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

    SearchHistory searchHistory; 

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;
    string preprocTime =  "Total pre-processing Time (cleaning, tokenization, inverted index building): " + to_string(duration.count()) + " seconds...";

    RenderWindow window1(VideoMode({1800, 830}), "Mini Search Engine");
    window1.setFramerateLimit(60);
    
    // Load font
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
    
    sf::Vector2u windowSize = window1.getSize();
    sf::Vector2f windowCenter(windowSize.x / 2.f, windowSize.y / 2.f);

    Text titleText(font, "boogle.", 80);
    titleText.setFillColor(titleColor);
    titleText.setStyle(Text::Bold);
    titleText.setPosition({windowCenter.x - 140.f, 150.f});

    Text subtitleText(font, "mini search engine", 20);
    subtitleText.setFillColor(textColor);
    subtitleText.setPosition({windowCenter.x-100.f, 240.f});

    RectangleShape sepLine1({600.f, 2.f});
    sepLine1.setPosition({600.f, 500.f});
    sepLine1.setFillColor(buttonBorderColor);
    
    RectangleShape topBar({1800.f, 40.f});
    topBar.setPosition({0.f, 0.f});
    topBar.setFillColor(buttonColor);
    topBar.setOutlineThickness(1);
    topBar.setOutlineColor(buttonBorderColor);

    Text footerText(font, "boogle - copyright 1995", 14);
    footerText.setFillColor(textColor);
    footerText.setPosition({700.f, 520.f});

    Button button1(font, "Search Word", {600.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button button2(font, "View Search History", {900.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button button3(font, "Top N Ranked Words", {1200.f, 400.f}, {220.f, 60.f}, buttonBorderColor, textColor);
    Button backButton(font, "BACK", {100.f, 100.f}, {80.f, 30.f}, buttonBorderColor, textColor);

    RectangleShape statusBar({1800.f, 30.f});
    statusBar.setPosition({0.f, 800.f});
    statusBar.setFillColor(buttonColor);
    statusBar.setOutlineThickness(1);
    statusBar.setOutlineColor(buttonBorderColor);

    Text preprocTimeText(font, preprocTime, 16);
    preprocTimeText.setFillColor(textColor);
    preprocTimeText.setPosition({10.f, 805.f});

    RectangleShape searchBox({500.f, 35.f}); // search screen 
    searchBox.setPosition({600.f, 350.f});
    searchBox.setFillColor(Color::White);
    searchBox.setOutlineThickness(2);
    searchBox.setOutlineColor(buttonBorderColor);

    Text inputTextSearchPg(font, "", 18);  
    inputTextSearchPg.setFillColor(Color::Black);
    inputTextSearchPg.setPosition({610.f, 355.f});

    Button searchButton(font, "SEARCH", {1120.f, 350.f}, {80.f, 35.f}, buttonBorderColor, textColor);

    RectangleShape searchResultsBox({1600.f, 400.f});
    searchResultsBox.setPosition({100.f, 400.f});
    searchResultsBox.setFillColor(Color::White);
    searchResultsBox.setOutlineThickness(2);
    searchResultsBox.setOutlineColor(buttonBorderColor);

    Text searchHeader(font, "Web Search", 24);
    searchHeader.setFillColor(titleColor);
    searchHeader.setStyle(Text::Bold);
    searchHeader.setPosition({100.f, 320.f});

    // Scrollbar for search results
    RectangleShape searchScrollbar({15.f, 50.f});
    searchScrollbar.setPosition({1690.f, 450.f});
    searchScrollbar.setFillColor(Color(128, 128, 128, 180));
    searchScrollbar.setOutlineThickness(1);
    searchScrollbar.setOutlineColor(Color::Black);

    Text resultText(font, "", 16);
    resultText.setFillColor(Color::Black);

    Text searchPrompt(font, "Enter search term:", 18);
    searchPrompt.setFillColor(textColor);
    searchPrompt.setPosition({600.f, 320.f});

    //history screen
    Text historyHeader(font, "Search History", 24);
    historyHeader.setFillColor(titleColor);
    historyHeader.setStyle(Text::Bold);
    historyHeader.setPosition({100.f, 150.f});

    RectangleShape historyBox({1600.f, 500.f});
    historyBox.setPosition({100.f, 200.f});
    historyBox.setFillColor(Color::White);
    historyBox.setOutlineThickness(2);
    historyBox.setOutlineColor(buttonBorderColor);

    Button clearAllHistoryButton(font, "CLEAR ALL HISTORY", {1200.f, 750.f}, {160.f, 30.f}, buttonBorderColor, textColor);
    Button clearRecentHistoryButton(font, "CLEAR MOST RECENT HISTORY", {900.f, 750.f}, {160.f, 30.f}, buttonBorderColor, textColor);

    // N Ranked screen
    RectangleShape fileInputBox({200.f, 35.f});
    fileInputBox.setPosition({600.f, 300.f});
    fileInputBox.setFillColor(Color::White);
    fileInputBox.setOutlineThickness(2);
    fileInputBox.setOutlineColor(buttonBorderColor);

    RectangleShape nInputBox({200.f, 35.f});
    nInputBox.setPosition({900.f, 300.f});
    nInputBox.setFillColor(Color::White);
    nInputBox.setOutlineThickness(2);
    nInputBox.setOutlineColor(buttonBorderColor);

    Text fileInputText(font, "", 18);
    fileInputText.setFillColor(Color::Black);
    fileInputText.setPosition({610.f, 305.f});

    Text nInputText(font, "", 18);
    nInputText.setFillColor(Color::Black);
    nInputText.setPosition({910.f, 305.f});

    Button rankedSearchButton(font, "GET WORDS", {1150.f, 300.f}, {120.f, 35.f}, buttonBorderColor, textColor);

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

    Text rankedHeader(font, "Top Ranked Words", 24);
    rankedHeader.setFillColor(titleColor);
    rankedHeader.setStyle(Text::Bold);
    rankedHeader.setPosition({100.f, 150.f});

    Text filePrompt(font, "File number (1-" + to_string(allFiles.size()) + "):", 16);
    filePrompt.setFillColor(textColor);
    filePrompt.setPosition({600.f, 270.f});

    Text nPrompt(font, "Number of top words:", 16);
    nPrompt.setFillColor(textColor);
    nPrompt.setPosition({900.f, 270.f});

    string currentInput;
    safeArray<string> searchResults;

    safeArray<string> rankedOutput;//for n ranked screen
    bool clickedFilesBox = false;
    bool clickedNBox = false;
    string fileInput = "";
    string nInput = "";
    int fileN;
    int Nwords;

    float searchScrollOffset = 0.f;
    float rankedScrollOffset = 0.f;
    bool searchScrolling = false;
    bool rankedScrolling = false;
    float lastMouseY = 0.f;

    enum class Screen {MAIN, SEARCH, HISTORY, RANKED };
    Screen currentScreen = Screen::MAIN;

    while (window1.isOpen()) {
        while (const optional<Event> event = window1.pollEvent()){
            if (event->is<Event::Closed>())
                window1.close();

            Vector2i mousePos = Mouse::getPosition(window1);

            if (currentScreen == Screen::MAIN) {
                if (button1.contains(mousePos)){
                    button1.setHovered(true);
                } else button1.setHovered(false);

                if(button2.contains(mousePos)){
                    button2.setHovered(true);
                } else button2.setHovered(false);

                if (button3.contains(mousePos)){
                    button3.setHovered(true);
                } else button3.setHovered(false);

                //check if any of the three homescreen buttons is clicked
                if (event->is<Event::MouseButtonPressed>()) {
                    if (button1.contains(mousePos)) {
                        cout << "Search Word button clicked!" << endl;
                        currentScreen = Screen::SEARCH;
                        currentInput = "";
                        searchResults.clear();
                        searchScrollOffset = 0.f; // Reset scroll when switching to search
                    } 
                    else if (button2.contains(mousePos)) {
                        cout << "View History button clicked!" << endl;
                        currentScreen = Screen::HISTORY;
                    } 
                    else if (button3.contains(mousePos)) {
                        cout << "N Ranked Words button clicked!" << endl;
                        currentScreen = Screen::RANKED;
                        // Reset ranked screen state
                        rankedOutput.clear();
                        fileInput = "";
                        nInput = "";
                        clickedFilesBox = false;
                        clickedNBox = false;
                        rankedScrollOffset = 0.f; // Reset scroll when switching to ranked
                    }
                }
            } 
            
            else if (currentScreen == Screen::SEARCH){
                if(backButton.contains(mousePos)) {
                    backButton.setHovered(true);
                } else backButton.setHovered(false);
                
                if (searchButton.contains(mousePos)) {
                    searchButton.setHovered(true);
                } 
                else { searchButton.setHovered(false);}

                 //scroll events
                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    if (wheelEvent->delta >0) {
                        searchScrollOffset= max(0.f, searchScrollOffset - 20.f);
                    } else {
                        float maxScroll = max(0.f, static_cast<float>(searchResults.size())*25.f-400.f);
                        searchScrollOffset = min(maxScroll, searchScrollOffset + 20.f);
                    }
                }

                if (event->is<Event::MouseButtonPressed>()) {// if mouse button pressed, check if back button pressed or search button 
                    if (backButton.contains(mousePos)) { //back button
                        currentScreen = Screen::MAIN;
                        currentInput = "";
                        searchScrollOffset = 0.f;
                    } else if  (searchBox.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {//input box click
                        searchResults.clear();
                        currentInput.clear();
                    } 
                    else if (searchButton.contains(mousePos)) { //search button
                        cout << "Search button clicked!" << endl;
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
                        searchScrollOffset = 0.f; // Reset scroll when new search is performed
                    }  
                }
                else if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                    if (keyEvent->code == Keyboard::Key::Enter) {
                        // Perform the same search as when clicking the search button
                        cout << "Enter key pressed for search!" << endl;
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
                    }
                }
                else if (const auto* textEvent = event->getIf<Event::TextEntered>()) { //if text is entered
                    char entered = static_cast<char>(textEvent->unicode);

                    if (entered == 8 && !currentInput.empty()) {
                        currentInput.pop_back();
                    }
                    else if (entered >= 32 && entered < 127 && currentInput.size() < 48) {
                        currentInput += entered;
                    }
                }
            }

            else if (currentScreen == Screen::HISTORY) { 
                if(backButton.contains(mousePos)) {
                    backButton.setHovered(true);
                } else backButton.setHovered(false);

                if(clearAllHistoryButton.contains(mousePos)) {
                    clearAllHistoryButton.setHovered(true);
                } else clearAllHistoryButton.setHovered(false);
                
                if (clearRecentHistoryButton.contains(mousePos)) {
                    clearRecentHistoryButton.setHovered(true);
                } else clearRecentHistoryButton.setHovered(false);

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) { //back button
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
                if(backButton.contains(mousePos)) {
                    backButton.setHovered(true);
                } else backButton.setHovered(false);

                if(rankedSearchButton.contains(mousePos)) {
                    rankedSearchButton.setHovered(true);
                } else rankedSearchButton.setHovered(false);

                if (const auto* wheelEvent = event->getIf<Event::MouseWheelScrolled>()) {
                    if (wheelEvent->delta > 0) {
                        rankedScrollOffset = max(0.f, rankedScrollOffset - 20.f);
                    } else {
                        float maxScroll = max(0.f, static_cast<float>(rankedOutput.size()) * 25.f - 400.f);
                        rankedScrollOffset = min(maxScroll, rankedScrollOffset + 20.f);
                    }
                }

                if (event->is<Event::MouseButtonPressed>()) {
                    if (backButton.contains(mousePos)) {//back button
                        currentScreen = Screen::MAIN;
                        rankedOutput.clear();
                        fileInput = "";
                        nInput = "";
                        rankedScrollOffset = 0.f;
                    }
                    else if (fileInputBox.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {//file input box
                        clickedFilesBox = true;
                        clickedNBox = false;
                        rankedOutput.clear();
                    } 
                    else if (nInputBox.getGlobalBounds().contains(Vector2f(mousePos.x, mousePos.y))) {//n input box
                        clickedNBox = true;
                        clickedFilesBox = false;
                        rankedOutput.clear();
                    }
                    else if (rankedSearchButton.contains(mousePos)) { //search button on n ranked wprds screen
                        cout << "get words button clicked" << endl;
                        if (fileInput.empty() || nInput.empty()) {
                            rankedOutput.clear();
                            rankedOutput.pushback("Enter both file number and N(num of words u want to view)");
                        }
                        else {
                            try {
                                fileN = stoi(fileInput);
                                Nwords = stoi(nInput);
                                
                                if (fileN < 1 || fileN > allFiles.size()) {
                                    rankedOutput.clear();
                                    rankedOutput.pushback("File number must be between 1 and " + to_string(allFiles.size()));
                                } else if(Nwords < 1){
                                    rankedOutput.clear();
                                    rankedOutput.pushback("enter N > 0");
                                }else {
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
                            }catch (...) {
                                rankedOutput.clear();
                                rankedOutput.pushback("invalid number entered");
                            }
                        }
                        rankedScrollOffset = 0.f; // Reset scroll when new results are generated
                    }
                    else {
                        clickedFilesBox = false;
                        clickedNBox = false;
                    }
                } else if (const auto* keyEvent = event->getIf<Event::KeyPressed>()) {
                    if (keyEvent->code == Keyboard::Key::Enter) {
                        cout << "Enter key pressed for ranked words!" << endl;
                        if (fileInput.empty() || nInput.empty()) {
                            rankedOutput.clear();
                            rankedOutput.pushback("Enter both file number and N(num of words u want to view)");
                        }
                        else {
                            try {
                                fileN = stoi(fileInput);
                                Nwords = stoi(nInput);
                                
                                if (fileN < 1 || fileN > allFiles.size()) {
                                    rankedOutput.clear();
                                    rankedOutput.pushback("File number must be between 1 and " + to_string(allFiles.size()));
                                } else if(Nwords < 1){
                                    rankedOutput.clear();
                                    rankedOutput.pushback("enter N > 0");
                                }else {
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
                            }catch (...) {
                                rankedOutput.clear();
                                rankedOutput.pushback("invalid number entered");
                            }
                        }
                        rankedScrollOffset = 0.f;
                    }
                }

                //text input for file number box
                if (clickedFilesBox){
                    if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                        char entered = static_cast<char>(textEvent->unicode);

                        if (entered == 8 && !fileInput.empty()) {//backspace pressed
                            fileInput.pop_back();
                        }
                        else if (entered >= '0' && entered <= '9') {
                            fileInput += entered;
                        }
                    }
                }

                //text input for N words box
                if (clickedNBox) {
                    if (const auto* textEvent = event->getIf<sf::Event::TextEntered>()) {
                        char entered = static_cast<char>(textEvent->unicode);

                        if (entered == 8 && !nInput.empty()) {
                            nInput.pop_back();
                        }
                        else if (entered >= '0' && entered <= '9') {
                            nInput += entered;
                        }
                    }
                }
            }
        }

        //RENDERING (basically drawing everything)
        window1.clear(bgColor);
        if (currentScreen == Screen::MAIN) {
            window1.draw(topBar);

            window1.draw(titleText);
            window1.draw(subtitleText);
            button1.draw(window1);
            button2.draw(window1);
            button3.draw(window1);
            window1.draw(statusBar);
            window1.draw(preprocTimeText);
            window1.draw(sepLine1);
            window1.draw(footerText);
        }
        else {
            backButton.draw(window1);
            
            if (currentScreen == Screen::SEARCH) {
                string displayText = currentInput + '_';
                inputTextSearchPg.setString(displayText);

                window1.draw(searchPrompt);
                window1.draw(searchBox);
                searchButton.draw(window1);
                window1.draw(inputTextSearchPg);
                window1.draw(searchResultsBox);

                //scrollbar position and size is set based on content
                float searchContentHeight = static_cast<float>(searchResults.size()) * 25.f;
                float searchVisibleHeight = 400.f;
                if (searchContentHeight > searchVisibleHeight) {
                    float scrollbarHeight = (searchVisibleHeight/searchContentHeight) * 400.f;
                    float scrollbarPos = (searchScrollOffset / (searchContentHeight - searchVisibleHeight)) * (400.f - scrollbarHeight);
                    
                    searchScrollbar.setSize({15.f, scrollbarHeight});
                    searchScrollbar.setPosition({1690.f, 450.f + scrollbarPos});
                    window1.draw(searchScrollbar);
                }

                //Display search results with scroll offset
                float y = 460.f - searchScrollOffset;
                for (int i = 0; i < searchResults.size(); i++) {
                    if (y >= 460.f - 25.f && y <= 830.f) { //draw visible items
                        resultText.setString(searchResults[i]);
                        resultText.setPosition({120.f, y});
                        window1.draw(resultText);
                    }
                    y += 25.f;
                    if (y > 830.f) break;
                }
                window1.draw(searchHeader);
            }
            else if (currentScreen == Screen::HISTORY) {
                window1.draw(historyHeader);
                window1.draw(historyBox);
                clearAllHistoryButton.draw(window1);
                clearRecentHistoryButton.draw(window1);
                float y = 220.f;
                SearchHistoryNode* current = searchHistory.peek();
                int count = 1;
                
                while (current != nullptr && y <= 680.f) {
                    string historyEntry = to_string(count) + ". " + current->query;
                    Text historyItem(font, historyEntry, 16);
                    historyItem.setFillColor(Color::Black);
                    historyItem.setPosition({120.f, y});
                    window1.draw(historyItem);
                    
                    current = current->next;
                    y += 25.f;
                    count++;
                }

                if (searchHistory.peek() == nullptr) {
                    Text noHistoryText(font, "No search history yet", 18);
                    noHistoryText.setFillColor(textColor);
                    noHistoryText.setPosition({200.f, 400.f});
                    window1.draw(noHistoryText);
                }
            }
            else if (currentScreen == Screen::RANKED) {
                window1.draw(rankedHeader);

                // Update input text displays
                string fileDisplayText = fileInput + (clickedFilesBox ? '_' : ' ');
                string nDisplayText = nInput + (clickedNBox ? '_' : ' ');
                fileInputText.setString(fileDisplayText);
                nInputText.setString(nDisplayText);

                //n rank screen drawing
                window1.draw(filePrompt);
                window1.draw(fileInputBox);
                window1.draw(fileInputText);
                window1.draw(nPrompt);
                window1.draw(nInputBox);
                window1.draw(nInputText);
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
                        resultText.setString(rankedOutput[i]);
                        resultText.setPosition( {120.f, y} );
                        window1.draw(resultText);
                    }
                    y += 25.f;
                    if (y >760.f) break;
                }
            }
        }
        window1.display();
    }
}