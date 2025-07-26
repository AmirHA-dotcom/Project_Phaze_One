//
// Created by amirh on 2025-07-25.
//

#ifndef PROJECT_PHAZE_ONE_PLOT_VIEW_H
#define PROJECT_PHAZE_ONE_PLOT_VIEW_H

#include "Libraries.h"
#include "Node.h"

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

    SDL_Rect m_plot_area;
    double m_min_time = 0, m_max_time = 1;
    double m_min_voltage = -1, m_max_voltage = 1;

    TTF_Font* m_font = nullptr;

    // panning
    bool m_is_panning = false;
    SDL_Point m_pan_start_pos;



    // helper functions
    SDL_Point world_to_screen(double time, double voltage);

public:
    Plot_View();
    ~Plot_View();

    void add_signal(const Signal& new_signal);
    void delete_all_signals();
    bool handle_event(SDL_Event& event);
    void auto_zoom();
    void render();
};


#endif //PROJECT_PHAZE_ONE_PLOT_VIEW_H
