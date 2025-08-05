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

enum class Tool_Bar_Action { Wire, Components_Menu, Grid, Net_Label, File, Configure_Analysis, Run, Probe, Math_Operation, Save, Delete , Sub_Circuit, New_File };

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

struct SubC_Menu_Item {
    string name;
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
    // window handling
    Uint32 main_window_id;

    int m_window_width;
    int m_window_height;

    // guid lines on the actions
    string info_bar_text;

    // new file
    bool new_file = false;

    // dragging
    bool is_dragging = false;
    int dragged_element_index = -1;
    SDL_Point drag_offset = {0, 0};

    // show grids
    bool show_grids = false;

    // item menu
    bool elements_menu = false;
    vector<Menu_Item> menu_items;
    int selected_menu_item_index = -1;
    int hovered_menu_item_index = -1;

    // sub_circuits menu
    bool sub_circuit_menu = false;
    vector<SubC_Menu_Item> SubC_menu_items;
    int selected_SubC_menu_item_index = -1;
    int hovered_SubC_item_index = -1;

    // sub_circuit creation handling
    bool create_SubC_mode = false;
    SDL_Rect create_new_SubC_button_rect;
    Node* node1 = nullptr;
    Node* node2 = nullptr;

    // getting the name of the sub_circuit
    string new_SubC_name;
    bool naming_SubC_menu_active = false;
    string text_input_buffer;
    SDL_Rect text_input_box_rect;
    SDL_Rect text_input_ok_rect;
    SDL_Rect text_input_cancel_rect;

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
    bool is_wiring = false;
    vector<Connection_Point> new_wire_points;

    // grounding
    bool is_grounding = false;

    // net labeling
    bool is_labeling = false;

    // top menu
    vector<Toolbar_Button> toolbar_buttons;
    int hovered_button_index = -1;

    // deleting element or wire
    bool is_deleting = false;

    // file menu
    bool is_file_menu_open = false;
    int selected_file_index = -1;
    int hovered_file_index = -1;
    vector<SDL_Rect> file_button_rects;
    SDL_Rect file_ok_button_rect;
    SDL_Rect file_cancel_button_rect;

    // saving circuit
    bool is_saving = false;
    string current_file_name;
    string current_file_address;

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
    string math_expression_string;
    vector<Signal> math_terms;
    int math_selected_element_index = -1;
    bool is_editing_constant = false;
    string math_constant_buffer = "1.0";
    vector<SDL_Rect> math_element_buttons;
    SDL_Rect constant_textbox_rect;
    SDL_Rect op_plus_button, op_minus_button, op_clear_button, op_execute_button;

    // plot window
    unique_ptr<Plot_View> plot_view;
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
    enum class Feedback_Cursor_State { None, Loading, Finished };
    Feedback_Cursor_State feedback_cursor_state = Feedback_Cursor_State::None;
    Uint32 feedback_cursor_timer_start = 0;
    SDL_Cursor* default_cursor = nullptr;
    SDL_Cursor* probe_cursor = nullptr;
    SDL_Cursor* crosshair_cursor = nullptr;
    SDL_Cursor* grounding_cursor = nullptr;
    SDL_Cursor* math_operation_cursor = nullptr;
    SDL_Cursor* deleting_cursor = nullptr;
    SDL_Cursor* dragging_cursor = nullptr;
    SDL_Cursor* not_valid_cursor = nullptr;
    SDL_Cursor* loading_cursor = nullptr;
    SDL_Cursor* finished_action_cursor = nullptr;

    // helper functions
    Node* find_node_at(SDL_Point pos, Controller* C);
    Graphical_Element* find_element_at(SDL_Point pos, Controller* C);
    Graphical_Wire* find_wire_at(SDL_Point pos, Controller* C);
    int find_connection_point_at(Graphical_Element* element, SDL_Point click_pos);
    void initialize_menu();
    void initialize_SubC_menu(Controller* C);
    void draw_component_menu(SDL_Renderer* renderer, TTF_Font* font);
    void draw_properties_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_grid(SDL_Renderer* renderer);
    void initialize_toolbar(TTF_Font* font);
    void draw_toolbar(SDL_Renderer* renderer, TTF_Font* font);
    void draw_configure_analysis(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_math_operation_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_save_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_file_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_SubC_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C);
    void draw_text_input_menu(SDL_Renderer* renderer, TTF_Font* font);
    void draw_status_bar(SDL_Renderer* renderer, TTF_Font* font);
    void add_math_term(bool is_subtraction, Controller* C);
    void execute_math_operation();
    void initialize_configure_analysis_menu(Controller* C);

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

    bool handle_saving_events(SDL_Event& event, Controller* C);

    bool handle_deleting_events(SDL_Event& event, Controller* C);

    bool handle_file_menu_events(SDL_Event& event, Controller* C);

    bool handle_SubC_menu_events(SDL_Event& event, Controller* C);

    bool handle_SubC_creation_events(SDL_Event& event, Controller* C);

    bool handle_text_input_menu_events(SDL_Event& event);

public:
    bool run (Controller* C);
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
