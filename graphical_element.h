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
#include "Voltage_Source.h"


// helpers

inline void render_text(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y, SDL_Color color = {0, 0, 0, 255})
{
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dest_rect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

enum class Rotation{Right, Left, Up, Down};

struct Editable_Property {
    string label;
    string value_as_string;
};

struct Connection_Point {
    SDL_Point pos;
    Node* node;
    Rotation rotation;
};

// main classes

class Graphical_Element
{
protected:
    Element* model_element;

    static TTF_Font* font;

    Rotation rotation;

    SDL_Point transform_point(SDL_Point point_to_rotate);
public:
    Graphical_Element(Element* element_model) : model_element(element_model) { rotation = Rotation::Right; }
    virtual ~Graphical_Element() = default;

    virtual void draw(SDL_Renderer* renderer) = 0;

    static void set_font(TTF_Font* font);

    SDL_Rect bounding_box;

    void change_rotation();

    Element* get_model();

    virtual vector<Editable_Property> get_editable_properties() = 0;

    virtual vector<Connection_Point> get_connection_points() = 0;
};

class Graphical_Resistor : public Graphical_Element {
public:
    Graphical_Resistor(Resistor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;
};

class Graphical_Capacitor : public Graphical_Element {
public:
    Graphical_Capacitor(Capacitor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Inductor : public Graphical_Element {
public:
    Graphical_Inductor(Inductor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Current_Source : public Graphical_Element {
public:
    Graphical_Current_Source(Current_Source* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Real_Diode : public Graphical_Element {
public:
    Graphical_Real_Diode(Real_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Zener_Diode : public Graphical_Element {
public:
    Graphical_Zener_Diode(Zener_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Voltage_Source : public Graphical_Element {
public:
    Graphical_Voltage_Source(Voltage_Source* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H
