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
#include "Circuit.h"


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

    virtual void draw(SDL_Renderer* renderer, bool show_grid) = 0;

    static void set_font(TTF_Font* font);

    SDL_Rect bounding_box;

    void change_rotation();

    Rotation get_rotation() { return rotation; }

    Element* get_model();

    virtual vector<Editable_Property> get_editable_properties() = 0;

    virtual vector<Connection_Point> get_connection_points() = 0;
};

class Graphical_Resistor : public Graphical_Element {
public:
    Graphical_Resistor(Resistor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;
};

class Graphical_Capacitor : public Graphical_Element {
public:
    Graphical_Capacitor(Capacitor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Inductor : public Graphical_Element {
public:
    Graphical_Inductor(Inductor* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Current_Source : public Graphical_Element {
public:
    Graphical_Current_Source(Current_Source* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Real_Diode : public Graphical_Element {
public:
    Graphical_Real_Diode(Real_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Zener_Diode : public Graphical_Element {
public:
    Graphical_Zener_Diode(Zener_Diode* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Voltage_Source : public Graphical_Element {
public:
    Graphical_Voltage_Source(Voltage_Source* model) : Graphical_Element(model) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Editable_Property> get_editable_properties() override;
    vector<Connection_Point> get_connection_points() override;

};

class Graphical_Ground : public Graphical_Element
{
private:
    SDL_Point m_position;
    Node* m_node;

public:
    Graphical_Ground(SDL_Point pos, Node* node) : Graphical_Element(nullptr), m_position(pos), m_node(node) {}

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Connection_Point> get_connection_points() override { return {}; }

    vector<Editable_Property> get_editable_properties() override { return {}; }

    Node* get_node() { return m_node; }
};

class Graphical_Net_Label : public Graphical_Element
{
private:
    SDL_Point m_position;
    std::string m_label_text;
    Node* m_node;

public:
    Graphical_Net_Label(SDL_Point pos, Node* node) : Graphical_Element(nullptr), m_position(pos), m_node(node) {}

    void set_label(const std::string& text) { m_label_text = text; }
    Node* get_node() { return m_node; }

    void draw(SDL_Renderer* renderer, bool show_grid) override;

    vector<Connection_Point> get_connection_points() override { return {}; }
    vector<Editable_Property> get_editable_properties() override { return { {"Name", m_label_text} }; }
};

class Graphical_SubCircuit : public Graphical_Element{
private:
    string m_type_name;
    SubCircuit* m_subcircuit_model;
public:
    Graphical_SubCircuit(SubCircuit* subcircuit_model, const std::string& type_name) : Graphical_Element(nullptr), m_type_name(type_name), m_subcircuit_model(subcircuit_model)
    { rotation = Rotation::Right; }

    void draw(SDL_Renderer* renderer, bool show_grids) override;
    vector<Connection_Point> get_connection_points() override;
    vector<Editable_Property> get_editable_properties() override;
    SubCircuit* get_subcircuit_model() { return m_subcircuit_model; }
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_ELEMENT_H