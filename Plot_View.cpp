//
// Created by amirh on 2025-07-25.
//

#include "Plot_View.h"

// helper functions

Plot_View::~Plot_View()
{
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);
}

void Plot_View::add_signal(const Signal &new_signal)
{
    m_signals.push_back(new_signal);
}

SDL_Point Plot_View::world_to_screen(double time, double voltage)
{
    // Map time to the horizontal axis of the plot area
    int screen_x = m_plot_area.x + (int)(((time - m_min_time) / (m_max_time - m_min_time)) * m_plot_area.w);
    // Map voltage to the vertical axis (inverted, since screen Y is 0 at the top)
    int screen_y = m_plot_area.y + m_plot_area.h - (int)(((voltage - m_min_voltage) / (m_max_voltage - m_min_voltage)) * m_plot_area.h);
    return {screen_x, screen_y};
}

// main functions

void Plot_View::run()
{
    cout << "run called" << endl;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw runtime_error("SDL could not initialize! SDL_Error: " + string(SDL_GetError()));
    }

    if (TTF_Init() == -1)
    {
        SDL_Quit();
        throw runtime_error("SDL_ttf could not initialize! TTF_Error: " + string(TTF_GetError()));
    }

    TTF_Font* font = TTF_OpenFont(FONT , 16);
    if (!font)
    {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
    }

    m_window = SDL_CreateWindow("Analysis Plot", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (!m_window)
    {
        throw runtime_error("Could not create plot window!");
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
    if (!m_renderer)
    {
        throw runtime_error("Could not create plot renderer!");
    }

    bool plot_running = true;
    SDL_Event event;

    while (plot_running)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                plot_running = false;
            }
        }

        draw_scene();
    }
}

void Plot_View::draw_scene()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    if (m_signals.empty() || m_signals[0].data_points.empty())
    {
        SDL_RenderPresent(m_renderer);
        return;
    }

    // plot area
    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);
    m_plot_area = {50, 50, width - 100, height - 100};

    // min and max
    const auto& data = m_signals[0].data_points;
    m_min_time = data.front().second;
    m_max_time = data.back().second;
    m_min_voltage = data[0].first;
    m_max_voltage = data[0].first;

    for (const auto& point : data)
    {
        if (point.first < m_min_voltage) m_min_voltage = point.first;
        if (point.first > m_max_voltage) m_max_voltage = point.first;
    }

    if (m_max_voltage == m_min_voltage)
    {
        m_max_voltage += 0.5;
        m_min_voltage -= 0.5;
    }

    // margin
    double voltage_range = m_max_voltage - m_min_voltage;
    m_min_voltage -= voltage_range * 0.1;
    m_max_voltage += voltage_range * 0.1;

    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(m_renderer, &m_plot_area);

    // showing the signal
    const auto& signal = m_signals[0];
    SDL_SetRenderDrawColor(m_renderer, signal.color.r, signal.color.g, signal.color.b, signal.color.a);

    for (size_t i = 0; i < data.size() - 1; ++i)
    {
        SDL_Point p1 = world_to_screen(data[i].second, data[i].first);
        SDL_Point p2 = world_to_screen(data[i+1].second, data[i+1].first);
        SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
    }

    SDL_RenderPresent(m_renderer);
}
