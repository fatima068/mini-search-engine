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

    void handleEvent(const Event& event, Vector2f mousePos) {
        // Use pattern matching for SFML 3.0
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
