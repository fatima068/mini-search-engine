#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
using namespace sf;
#include <iostream>
using namespace std;    

int main() {
    unsigned int width = 1200;
    unsigned int height = 800;
    RenderWindow window(VideoMode({ width, height }), "buggle.");

    Font font;
    if (!font.openFromFile("ARIAL.ttf")) {
        cout << "Error: Could not load font.\n";
        return -1;
    }

    // --- Optional Title (like Google logo text) ---
    Text title(font, "buggle.", 60);
    title.setFillColor(Color(66, 133, 244)); // Google blue tone
    title.setPosition({ 480, 150 }); // centered manually

    // --- Search Bar Setup ---
    float boxWidth = 700;
    float boxHeight = 50;
    float boxX = (1200 - boxWidth - 150) / 2; // center + room for button
    float boxY = 300;

    RectangleShape textBox(Vector2f(boxWidth, boxHeight));
    textBox.setPosition({ boxX, boxY });
    textBox.setFillColor(Color::White);
    textBox.setOutlineThickness(2);
    textBox.setOutlineColor(Color(200, 200, 200));

    Text userInput(font, "", 24);
    userInput.setFillColor(Color::Black);
    userInput.setPosition({ boxX + 15, boxY + 10 });

    string inputText;
    bool isTyping = false;

    // --- Search Button ---
    RectangleShape searchButton(Vector2f(120, 50));
    searchButton.setPosition({ boxX + boxWidth + 20, boxY });
    searchButton.setFillColor(Color(66, 133, 244)); // blue

    Text buttonText(font, "Search", 24);
    buttonText.setFillColor(Color::White);
    buttonText.setPosition({ searchButton.getPosition().x + 15, searchButton.getPosition().y + 10 });

    // --- Placeholder text ---
    Text placeholder(font, "Search...", 24);
    placeholder.setFillColor(Color(150, 150, 150));
    placeholder.setPosition({ boxX + 15, boxY + 10 });

    // --- Main Loop ---
    while (window.isOpen()) {
        // new SFML 3 event polling style
        while (auto event = window.pollEvent()) {

            // --- Window Close ---
            if (event->is<Event::Closed>())
                window.close();

            // --- Mouse Click Logic ---
            if (const auto* mousePressed = event->getIf<Event::MouseButtonPressed>()) {
                if (mousePressed->button == Mouse::Button::Left) {
                    Vector2f clickPos(mousePressed->position);

                    if (textBox.getGlobalBounds().contains(clickPos)) {
                        isTyping = true;
                    }
                    else {
                        isTyping = false;
                    }

                    if (searchButton.getGlobalBounds().contains(clickPos)) {
                        cout << "Searching for: " << inputText << endl;
                        // TODO: Call your backend search(inputText)
                    }
                }
            }

            // --- Typing Logic ---
            if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                if (isTyping) {
                    char32_t unicode = textEvent->unicode;

                    if (unicode == U'\b') { // backspace
                        if (!inputText.empty())
                            inputText.pop_back();
                    }
                    else if (unicode < 128 && unicode != U'\r') {
                        inputText += static_cast<char>(unicode);
                    }

                    userInput.setString(inputText);
                }
            }
        }

        // --- Hover Effect for Button ---
        Vector2i mousePos = Mouse::getPosition(window);
        if (searchButton.getGlobalBounds().contains(static_cast<Vector2f>(mousePos)))
            searchButton.setFillColor(Color(52, 120, 240));
        else
            searchButton.setFillColor(Color(66, 133, 244));

        // --- Rendering ---
        window.clear(Color(245, 245, 245)); // light gray background
        window.draw(title);
        window.draw(textBox);

        if (inputText.empty() && !isTyping)
            window.draw(placeholder);
        else
            window.draw(userInput);

        window.draw(searchButton);
        window.draw(buttonText);
        window.display();
    }

    return 0;
}
