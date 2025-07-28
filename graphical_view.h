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
#include "Plot_View.h"

enum class Analysis_Mode { Transient, AC_Sweep, Phase_Sweep };

enum class AC_Sweep_Type { Octave, Decade, Linear };

enum class Tool_Bar_Action { Wire, Components_Menu, Grid, Net_Label, File, Configure_Analysis, Run, Probe, Math_Operation };

struct Toolbar_Button {
    SDL_Rect rect;
    string text_label;
    Tool_Bar_Action action;
};

struct Menu_Item {
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

    // dragging
    bool m_is_dragging = false;
    int m_dragged_element_index = -1;
    SDL_Point m_drag_offset = {0, 0};

    // show grids
    bool show_grids = false;

    // item menu
    bool elements_menu = false;
    vector<Menu_Item> menu_items;
    int selected_menu_item_index = -1;

    // editing
    bool editing = false;
    int edited_element_index = -1;
    vector<string> edit_buffers;
    int active_edit_box = -1;
    SDL_Rect ok_button_rect;
    SDL_Rect cancel_button_rect;
    vector<SDL_Rect> property_rects;

    // griding the workspace
    const int GRID_SIZE = 10;

    // wiring mode
    bool m_is_wiring = false;
    vector<Connection_Point> new_wire_points;

    // grounding
    bool is_grounding = false;

    // net labeling
    bool is_labeling = false;

    // top menu
    vector<Toolbar_Button> m_toolbar_buttons;
    int m_hovered_button_index = -1;

    // configure analysis
    bool is_configuring_analysis = false;
    Analysis_Mode current_analysis_mode = Analysis_Mode::Transient;
    AC_Sweep_Type AC_sweep_type = AC_Sweep_Type::Linear;
    SDL_Rect transient_tab_rect, ac_sweep_tab_rect, phase_sweep_tab_rect;
    SDL_Rect octave_button_rect, decade_button_rect, linear_button_rect;

    // probe mode
    bool probe_mode = false;

    // doing math on signals
    bool math_operation_mode = false;
    string m_math_expression_string;
    optional<Signal> m_math_result_signal;
    char m_math_next_operator = ' ';
    vector<SDL_Rect> m_math_element_buttons;
    SDL_Rect m_op_plus_button, m_op_minus_button, m_op_clear_button, m_op_execute_button;

    // plot window
    unique_ptr<Plot_View> m_plot_view;
    vector<SDL_Color> default_colors = {
            {0,   255, 0,   255}, // GREEN
            {0,   0,   255, 255}, // BLUE
            {0,   255, 255, 255}, // CYAN
            {255, 0,   0,   255}, // RED
            {255, 0,   255, 255}, // MAGENTA
            {255, 255, 0,   255}, // YELLOW
            {255, 165, 0,   255}, // ORANGE
            {128, 0,   128, 255}, // PURPLE
            {0,   128, 0,   255}, // DARK GREEN
            {255, 105, 180, 255}, // HOT PINK
            {0,   128, 128, 255}, // TEAL
            {165, 42,  42,  255}, // BROWN
            {128, 128, 128, 255}, // GRAY
            {255, 215, 0,   255}, // GOLD
            {75,  0,   130, 255}, // INDIGO
    };
    int color_index = 0;

    // cursors
    SDL_Cursor* m_default_cursor = nullptr;
    SDL_Cursor* m_probe_cursor = nullptr;
    SDL_Cursor* m_crosshair_cursor = nullptr;

    // helper functions
    Node* find_node_at(SDL_Point pos, Controller* C);
    Graphical_Element* find_element_at(SDL_Point pos, Controller* C);
    void initialize_menu();
    void draw_component_menu(SDL_Renderer* renderer, TTF_Font* font);
    void draw_properties_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_grid(SDL_Renderer* renderer);
    void initialize_toolbar(TTF_Font* font);
    void draw_toolbar(SDL_Renderer* renderer, TTF_Font* font);
    void draw_configure_analysis(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_math_operation_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);

    // main functions
    bool handle_events(SDL_Event& event, Controller* C);

    bool handle_menu_events(SDL_Event& event, Controller* C);

    bool handle_edit_properties_menu(SDL_Event& event, Controller* C);

    bool handle_wiring_events(SDL_Event& event, Controller* C);

    bool handle_grounding_events(SDL_Event& event, Controller* C);

    bool handle_net_labeling_events(SDL_Event& event, Controller* C);

    bool handle_toolbar_events(SDL_Event& event, Controller* C);

    bool handle_configure_analysis_events(SDL_Event& event, Controller* C);

    bool handle_probing_events(SDL_Event& event, Controller* C);

    bool handle_math_operation_events(SDL_Event& event, Controller* C);

public:
    bool run (Controller* C);
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
