//
// Created by amirh on 2025-07-20.
//

#ifndef PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
#define PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H

#include "Libraries.h"
#include "Errors.h"
#include "Controller.h"
#include <SDL.h>
#include "elements_graphics.h"
#include "Graphical_Wire.h"


struct MenuItem {
    string name;
    Element_Type type;
    string subtype_tag;
    SDL_Rect rect;
};

struct Point_Comparator {
    bool operator()(const SDL_Point& a, const SDL_Point& b) const {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    }
};

class graphical_view
{
private:
    TTF_Font* m_font = nullptr;

    int m_window_width;
    int m_window_height;

    bool m_is_dragging = false;
    int m_dragged_element_index = -1;
    SDL_Point m_drag_offset = {0, 0};

    bool elements_menu = false;

    vector<MenuItem> menu_items;
    int selected_menu_item_index = -1;

    bool editing = false;
    int edited_element_index = -1;
    vector<string> edit_buffers;
    int active_edit_box = -1;
    SDL_Rect ok_button_rect;
    SDL_Rect cancel_button_rect;
    vector<SDL_Rect> property_rects;  // store text boxes

    // griding the workspace
    const int GRID_SIZE = 10;

    // wiring mode
    bool m_is_wiring = false;
    vector<Connection_Point> new_wire_points;

    // helper functions
    void initialize_menu();
    void draw_component_menu(SDL_Renderer* renderer, TTF_Font* font);
    void draw_properties_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);

    void draw_grid(SDL_Renderer* renderer);

    // main functions
    bool handle_events(SDL_Event& event, Controller* C);

    bool handle_menu_events(SDL_Event& event, Controller* C);

    bool handle_edit_properties_menu(SDL_Event& event, Controller* C);

    bool handle_wiring_events(SDL_Event& event, Controller* C);

public:
    bool run (Controller* C);
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
