//
// Created by amirh on 2025-07-21.
//

#ifndef PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H
#define PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H

#include "Libraries.h"
#include "Element.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Current_Source.h"
#include "Real_Diode.h"
#include "Zener_Diode.h"

class Graphical_Element
{
protected:
    Element* model_element;

    static TTF_Font* font;
public:
    Graphical_Element(Element* element_model) : model_element(element_model) {}
    virtual ~Graphical_Element() = default;

    virtual void draw(SDL_Renderer* renderer) = 0;

    static void set_font(TTF_Font* font);

    SDL_Rect bounding_box;

};

class Graphical_Resistor : public Graphical_Element {
public:
    Graphical_Resistor(Resistor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

class Graphical_Capacitor : public Graphical_Element {
public:
    Graphical_Capacitor(Capacitor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

class Graphical_Inductor : public Graphical_Element {
public:
    Graphical_Inductor(Inductor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

class Graphical_Current_Source : public Graphical_Element {
public:
    Graphical_Current_Source(Current_Source* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

class Graphical_Real_Diode : public Graphical_Element {
public:
    Graphical_Real_Diode(Real_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

class Graphical_Zener_Diode : public Graphical_Element {
public:
    Graphical_Zener_Diode(Zener_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H
