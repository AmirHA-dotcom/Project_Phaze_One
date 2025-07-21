//
// Created by amirh on 2025-07-21.
//

#ifndef PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H
#define PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H

#include "Libraries.h"
#include "Element.h"
#include "Resistor.h"
#include "Capacitor.h"

class Graphical_Element
{
protected:
    Element* model_element;
public:
    Graphical_Element(Element* element_model) : model_element(element_model) {}
    virtual ~Graphical_Element() = default;

    virtual void draw(SDL_Renderer* renderer) = 0;

    SDL_Rect bounding_box;

};

class Graphical_Resistor : public Graphical_Element {
public:
    Graphical_Resistor(Resistor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }
};

class Graphical_Capacitor : public Graphical_Element {
public:
    Graphical_Capacitor(Capacitor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        int x = bounding_box.x;
        int y = bounding_box.y;
        int w = bounding_box.w;
        int h = bounding_box.h;

        SDL_RenderDrawLine(renderer, x + w/2 - 5, y + 10, x + w/2 - 5, y + h - 10);
        SDL_RenderDrawLine(renderer, x + w/2 + 5, y + 10, x + w/2 + 5, y + h - 10);
    }
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H
