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