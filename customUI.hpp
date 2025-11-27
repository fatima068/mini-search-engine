#ifndef CUSTOM_UI_HPP
#define CUSTOM_UI_HPP
#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Window/Event.hpp>
using namespace sf;
using namespace std;

class ScrollablePanel {
    private:
    RectangleShape container;
    RectangleShape scrollbar;
    safeArray<string> content;
    float scrollOffset;
    float lineHeight;
    Font& font;

    public:
    ScrollablePanel(Font& fontRef, Vector2f position, Vector2f size, float lineH = 25.f) :font(fontRef), scrollOffset(0.f), lineHeight(lineH) {
        container.setSize(size);
        container.setPosition(position);
        container.setFillColor(Color::White);
        container.setOutlineThickness(2);
        container.setOutlineColor(Color(128, 128, 128));  
        scrollbar.setSize({15.f, 50.f});
        scrollbar.setFillColor(Color(128, 128, 128, 180));
    }
        
    void setContent(const safeArray<string>& newContent) { content = newContent;}
    
    void handleScroll(float delta) {
        float maxOffset = max(0.f, getContentHeight() - container.getSize().y);
        scrollOffset = clamp(scrollOffset - delta * 20.f, 0.f, maxOffset);
    }

    void draw(RenderWindow& window, Color textColor = Color::Black) {
        window.draw(container);
        
        float startY = container.getPosition().y;
        float endY = startY + container.getSize().y;
        float y = startY - scrollOffset;
        
        for (int i = 0; i < content.size(); i++) {
            if (y >= startY - lineHeight && y <= endY) {
                Text text( font,content[i], 16);
                text.setFillColor(textColor);
                text.setPosition({container.getPosition().x + 10.f, y});
                window.draw(text);
            }
            y += lineHeight;
            if (y > endY) break;
        }
        if (getContentHeight() > container.getSize().y) {
            updateScrollbar();
            window.draw(scrollbar);
        }
    }
    
    float getScrollOffset() const { return scrollOffset; }
    void setScrollOffset(float offset) { scrollOffset = offset; }

    private:
    float getContentHeight() const {
        return content.size() * lineHeight;
    }
    
    void updateScrollbar() {
        float visibleRatio = container.getSize().y / getContentHeight();
        float scrollbarHeight = container.getSize().y * visibleRatio;
        float scrollRatio = scrollOffset / (getContentHeight() - container.getSize().y);
        float scrollbarY = container.getPosition().y + scrollRatio * (container.getSize().y - scrollbarHeight);
        
        scrollbar.setSize({15.f, scrollbarHeight});
        scrollbar.setPosition({container.getPosition().x + container.getSize().x - 15.f, scrollbarY});
    }
    
    float clamp(float value, float min, float max) {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
};

class SuggestionBox {
private:
    RectangleShape box;
    Font& font;
    safeArray<string> suggestions;
    bool visible;

public:
    SuggestionBox(Font& fontRef, Vector2f position, Vector2f size) : 
        font(fontRef), visible(false) {
        
        box.setSize(size);
        box.setPosition(position);
        box.setFillColor(Color::White);
        box.setOutlineThickness(1);
        box.setOutlineColor(Color(200, 200, 200));
    }

    void setSuggestions(const safeArray<string>& newSuggestions) {
        suggestions = newSuggestions;
        visible = !suggestions.isEmpty();
        float suggestionHeight = min(4, suggestions.size()) * 25.f;
        box.setSize({box.getSize().x, suggestionHeight});
    }

    void hide() {
        visible = false;
    }

    bool isVisible() const {
        return visible;
    }

    bool suggestionClicked(Vector2f mousePos, string& selectedSuggestion) {
        if (!visible || !box.getGlobalBounds().contains(mousePos)) {
            return false;
        }
        
        float relativeY = mousePos.y - box.getPosition().y;
        int suggestionIndex = static_cast<int>(relativeY / 25.f);
        
        if (suggestionIndex >= 0 && suggestionIndex < suggestions.size() && suggestionIndex < 4) {
            selectedSuggestion = suggestions[suggestionIndex];
            return true;
        }
        return false;
    }

    void setPosition(Vector2f position) {
        box.setPosition(position);
    }

    void draw(RenderWindow& window) {
        if (!visible) return;
        
        window.draw(box);
            
        for (int i = 0; i < suggestions.size() && i < 4; i++) {
            Text suggestionText(font, suggestions[i], 16);
            suggestionText.setFillColor(Color::Black);
            suggestionText.setPosition({
                box.getPosition().x + 5.f,
                box.getPosition().y + i * 25.f + 2.f
            });
            window.draw(suggestionText);
        }
    }
};

class InputBox {
private:
    RectangleShape box;
    Text text;
    string content;
    bool isActive;
    Clock cursorClock;
    bool cursorVisible;
    unsigned int maxLength;

public:
    InputBox(Font& font, Vector2f boxPosition, Vector2f size, unsigned int maxLen = 48) : isActive(false), cursorVisible(true), maxLength(maxLen), text( font, "",18) {
        
        box.setSize(size);
        box.setPosition(boxPosition);
        box.setFillColor(Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(Color(128, 128, 128));
        
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(Color::Black);
        text.setPosition({boxPosition.x + 5, boxPosition.y + 5});
    }

    Vector2f getSize() const { return box.getSize(); }

    void handleEvent(const Event& event, Vector2f mousePos) {
        if (event.is<Event::MouseButtonPressed>()) {    
            bool wasActive = isActive;
            isActive = box.getGlobalBounds().contains(mousePos);
            box.setOutlineColor(isActive ? Color::Blue : Color(128, 128, 128));
            
            if (isActive && !wasActive) {
                cursorClock.restart();
            }
            return;
        }
        
        if (!isActive) return;
        
        if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
            handleTextInput(textEvent->unicode);
        }
    }

    void handleEvent(const Event& event, Vector2f mousePos, SuggestionBox &searchSuggestionBox, safeArray<string> &suggestionList, Trie &trie) {
    if (event.is<Event::MouseButtonPressed>()) {    
        bool wasActive = isActive;
        string empty = " ";
        bool clickOnInput = box.getGlobalBounds().contains(mousePos); //is input box clikced or suggestion box?
        bool clickOnSuggestion = searchSuggestionBox.isVisible() && searchSuggestionBox.suggestionClicked(mousePos, empty);
        
        isActive = clickOnInput || clickOnSuggestion;
        box.setOutlineColor(isActive ? Color::Blue : Color(128, 128, 128));
        
        if (isActive && !wasActive) {
            cursorClock.restart();

            if (!content.empty()) {
                suggestionList = trie.getSuggestions(content);
                searchSuggestionBox.setSuggestions(suggestionList);
                Vector2f inputPos = getPosition();
                Vector2f inputSize = getSize();
                searchSuggestionBox.setPosition({inputPos.x, inputPos.y + inputSize.y});
            }
        } else if (!isActive && !clickOnSuggestion) {
            searchSuggestionBox.hide();
        }
        return;
    }
    
    if (!isActive) return;
    
    if (const auto* textEvent = event.getIf<Event::TextEntered>()) {
        handleTextInput(textEvent->unicode);
        if (!content.empty()) {
            suggestionList = trie.getSuggestions(content);
            searchSuggestionBox.setSuggestions(suggestionList);

            Vector2f inputPos = getPosition();
            Vector2f inputSize = getSize();
            searchSuggestionBox.setPosition({inputPos.x, inputPos.y + inputSize.y});
        } 
        else
            searchSuggestionBox.hide();
    }
}
    
    string getText()const { return content; }
    void setText(const string& newText) { content = newText; updateDisplayText(); }
    void clear() { content.clear(); updateDisplayText(); }
    bool getActive() const { return isActive; }
    
    void draw(RenderWindow& window) {
        window.draw(box);
        
        if (isActive && cursorClock.getElapsedTime().asSeconds() > 0.5f) {//cursor blinking
            cursorVisible = !cursorVisible;
            cursorClock.restart();
        }        
        string displayText = content + (isActive && cursorVisible ? "_" : "");
        text.setString(displayText);
        window.draw(text);
    }

    Vector2f getPosition() const { return box.getPosition(); }

    private:
    void handleTextInput(uint32_t unicode) {
        if (unicode == 8 && !content.empty()) { // Backspace
            content.pop_back();
        }
        else if (unicode >= 32 && unicode < 127 && content.size() < maxLength) {
            content += static_cast<char>(unicode);
        }
        updateDisplayText();
    }
    
    void updateDisplayText() {
        text.setString(content);
    }
};

class myText{
    private:
    Text text;

    public:
    myText(Font& font, const string& str, unsigned int size, Vector2f position, Color fontColor, Text::Style style): text(font, str, size) {
        text.setPosition(position);
        text.setFillColor(fontColor);
        text.setStyle(style);
    }

    void draw(RenderWindow& window) {
        window.draw(text);
    }

    void setString(const string& str) {
        text.setString(str);
    }
    void setPosition(Vector2f position) {
        text.setPosition(position);
    }
};

class Button {
private:
    RectangleShape shape;
    Text text;
    Color normalColor, hoverColor;
    bool isHovered;

public:
    Button(Font& font, const string& label, Vector2f position, Vector2f size, Color borderColor, Color textColor):normalColor(192, 192, 192), hoverColor(160, 160, 160), isHovered(false), text(font, label, 16) {
        
        shape.setSize(size);
        shape.setPosition(position);
        shape.setOrigin(shape.getGeometricCenter());
        shape.setFillColor(normalColor);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(borderColor);

        text.setFillColor(textColor);         
        text.setPosition({position.x-40, position.y-10});
        text.setStyle(Text::Bold);
    }
    
    bool contains(Vector2f point) {return shape.getGlobalBounds().contains(point); }

    bool contains(Vector2i point) {
        return shape.getGlobalBounds().contains(static_cast<Vector2f>(point));
    }
    
    void setHovered(bool hover) { 
        isHovered = hover;
        if(hover ==true){
            shape.setFillColor(hoverColor);
        } else shape.setFillColor(normalColor);
        //figure out how to add position change on hover 
    }
    
    void draw(RenderWindow& window){ 
        window.draw(shape); 
        window.draw(text); 
    }
    
    Vector2f getPosition() const {return shape.getPosition();}
    void setPosition(Vector2f pos) {shape.setPosition(pos); }
};
#endif
