//
// Created by amirh on 2025-07-25.
//

#ifndef PROJECT_PHAZE_ONE_PLOT_VIEW_H
#define PROJECT_PHAZE_ONE_PLOT_VIEW_H

#include "Libraries.h"
#include "Node.h"

enum class Plot_Event_Status{ Handled, Not_Handled, Close_Request };

enum class Unit{V, A, W, s, dB, Hz, deg, Dec, Oct};

struct Cursor {
    double X;
    double Y;
    size_t data_point_index;
};

struct Signal {
    string name;
    vector<pair<double, double>> data_points;   // y, x
    SDL_Color color;
};

class Plot_View
{
private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    vector<Signal> m_signals;

    Unit y_axis_unit = Unit::V;
    Unit x_axis_unit = Unit::s;

    SDL_Rect plot_area;
    double min_x = 0, max_x = 1;
    double min_y = -1, max_y = 1;

    TTF_Font* m_font = nullptr;

    // panning
    bool is_panning = false;
    SDL_Point pan_start_pos;

    // cursor
    optional<Cursor> cursor1;
    optional<Cursor> cursor2;
    bool cursor_mode_active = false;
    bool place_first_cursor_next = true;

    // color menu
    bool color_menu_active = false;
    int signal_to_edit_index = -1;
    SDL_Point color_menu_pos;

    vector<SDL_Rect> legend_rects;
    vector<SDL_Rect> color_picker_buttons;

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

    // helper functions
    SDL_Point world_to_screen(double time, double voltage);
    void draw_color_picker_menu();
public:
    Plot_View();
    ~Plot_View();

    Uint32 get_window_id() const { return SDL_GetWindowID(m_window); }

    void set_y_unit(Unit u) { y_axis_unit = u; }
    void set_x_unit(Unit u) { x_axis_unit = u; }

    vector<Signal> get_all_signals() { return m_signals; }

    void add_signal(const Signal& new_signal);
    void delete_all_signals();
    Plot_Event_Status  handle_event(SDL_Event& event);
    void auto_zoom();
    void render();
};


#endif //PROJECT_PHAZE_ONE_PLOT_VIEW_H
