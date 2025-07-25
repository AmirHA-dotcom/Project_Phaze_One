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

    // helper functions
    void draw_scene();
    // Converts a data point (time, voltage) to a screen coordinate
    SDL_Point world_to_screen(double time, double voltage);

public:
    ~Plot_View();

    void add_signal(const Signal& new_signal);

    void run();

};


#endif //PROJECT_PHAZE_ONE_PLOT_VIEW_H
