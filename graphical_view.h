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


struct MenuItem {
    string name;
    Element_Type type;
    SDL_Rect rect;
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

    // helper functions
    void initialize_menu();
    void draw_component_menu(SDL_Renderer* renderer, TTF_Font* font);

    // main functions
    bool handle_events(SDL_Event& event, Controller* C);

    bool handle_menu_events(SDL_Event& event, Controller* C);

public:
    bool run (Controller* C);
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
