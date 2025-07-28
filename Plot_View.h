//
// Created by amirh on 2025-07-25.
//

#ifndef PROJECT_PHAZE_ONE_PLOT_VIEW_H
#define PROJECT_PHAZE_ONE_PLOT_VIEW_H

#include "Libraries.h"
#include "Node.h"

enum class Unit{V, A, W, s};

struct Cursor {
    double X;
    double Y;
    size_t data_point_index; // Index of the point it's snapped to
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

    SDL_Rect m_plot_area;
    double m_min_x = 0, m_max_x = 1;
    double m_min_y = -1, m_max_y = 1;

    TTF_Font* m_font = nullptr;

    // panning
    bool m_is_panning = false;
    SDL_Point m_pan_start_pos;

    // cursor
    optional<Cursor> m_cursor1;
    optional<Cursor> m_cursor2;
    bool m_cursor_mode_active = false;
    bool m_place_first_cursor_next = true;

    // helper functions
    SDL_Point world_to_screen(double time, double voltage);

public:
    Plot_View();
    ~Plot_View();

    void set_y_unit(Unit u) { y_axis_unit = u; }
    void set_x_unit(Unit u) { x_axis_unit = u; }

    vector<Signal> get_all_signals() { return m_signals; }

    void add_signal(const Signal& new_signal);
    void delete_all_signals();
    bool handle_event(SDL_Event& event);
    void auto_zoom();
    void render();
};


#endif //PROJECT_PHAZE_ONE_PLOT_VIEW_H
