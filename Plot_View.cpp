//
// Created by amirh on 2025-07-25.
//

#include "Plot_View.h"

// helper functions

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


Plot_View::Plot_View()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw runtime_error("SDL could not initialize! SDL_Error: " + string(SDL_GetError()));
    }

    if (TTF_Init() == -1)
    {
        SDL_Quit();
        throw runtime_error("SDL_ttf could not initialize! TTF_Error: " + string(TTF_GetError()));
    }

    m_font = TTF_OpenFont(FONT , 16);
    if (!m_font)
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
}

Plot_View::~Plot_View()
{
    if (m_renderer) SDL_DestroyRenderer(m_renderer);
    if (m_window) SDL_DestroyWindow(m_window);
}

void Plot_View::add_signal(const Signal &new_signal)
{
    m_signals.push_back(new_signal);
}

void Plot_View::delete_all_signals()
{
    m_signals.clear();
}

SDL_Point Plot_View::world_to_screen(double time, double voltage)
{
    // mapping time in x axis
    int screen_x = m_plot_area.x + (int)(((time - m_min_time) / (m_max_time - m_min_time)) * m_plot_area.w);
    // mapping voltage in y axis
    int screen_y = m_plot_area.y + m_plot_area.h - (int)(((voltage - m_min_voltage) / (m_max_voltage - m_min_voltage)) * m_plot_area.h);
    return {screen_x, screen_y};
}

// main functions

void Plot_View::auto_zoom()
{
    if (m_signals.empty()) return;

    // finding the min and the max to scale
    m_min_time = m_signals[0].data_points.front().second;
    m_max_time = m_signals[0].data_points.front().second;
    m_min_voltage = m_signals[0].data_points.front().first;
    m_max_voltage = m_signals[0].data_points.front().first;

    for (const auto& signal : m_signals)
    {
        for (const auto& point : signal.data_points)
        {
            if (point.second < m_min_time) m_min_time = point.second;
            if (point.second > m_max_time) m_max_time = point.second;
            if (point.first < m_min_voltage) m_min_voltage = point.first;
            if (point.first > m_max_voltage) m_max_voltage = point.first;
        }
    }

    // symmetrize and add margin
    if (m_max_voltage == m_min_voltage)
    {
        m_max_voltage += 0.5;
        m_min_voltage -= 0.5;
    }
    double voltage_range = m_max_voltage - m_min_voltage;
    m_min_voltage -= voltage_range * 0.1;
    m_max_voltage += voltage_range * 0.1;

    double time_range = m_max_time - m_min_time;
    if (time_range == 0) { time_range = 1.0; }
    m_min_time -= time_range * 0.1;
    m_max_time += time_range * 0.1;
}

bool Plot_View::handle_event(SDL_Event& event)
{
    if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(m_window))
    {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            return false;
        }

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_SPACE:
                    auto_zoom();
                    break;
            }
        }
    }

    // panning
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    bool ctrl_is_pressed = keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL];

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && ctrl_is_pressed)
    {

        m_is_panning = true;
        m_pan_start_pos = { event.button.x, event.button.y };

    }

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        m_is_panning = false;
    }

    if (event.type == SDL_MOUSEMOTION && m_is_panning)
    {
        // cursor movment
        int dx = event.motion.x - m_pan_start_pos.x;
        int dy = event.motion.y - m_pan_start_pos.y;

        // convert that pixel delta to a world coordinate delta
        double time_per_pixel = (m_max_time - m_min_time) / m_plot_area.w;
        double voltage_per_pixel = (m_max_voltage - m_min_voltage) / m_plot_area.h;

        // shifting the min and max
        m_min_time -= dx * time_per_pixel;
        m_max_time -= dx * time_per_pixel;
        m_min_voltage += dy * voltage_per_pixel;
        m_max_voltage += dy * voltage_per_pixel;

        m_pan_start_pos = { event.motion.x, event.motion.y };
    }

    // zoom in and out
    if (event.type == SDL_MOUSEWHEEL)
    {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        // mouse in the plot
//        if (!SDL_PointInRect(&SDL_Point{mouse_x, mouse_y}, &m_plot_area))
//        {
//            return true;
//        }

        // world coordinates under the mouse
        double time_before_zoom = m_min_time + ((double)(mouse_x - m_plot_area.x) / m_plot_area.w) * (m_max_time - m_min_time);
        double voltage_before_zoom = m_min_voltage + ((double)(m_plot_area.y + m_plot_area.h - mouse_y) / m_plot_area.h) * (m_max_voltage - m_min_voltage);

        // zoom factor
        double zoom_factor = 1.1;
        if (event.wheel.y > 0)
        {
            m_max_time = time_before_zoom + (m_max_time - time_before_zoom) / zoom_factor;
            m_min_time = time_before_zoom - (time_before_zoom - m_min_time) / zoom_factor;
            m_max_voltage = voltage_before_zoom + (m_max_voltage - voltage_before_zoom) / zoom_factor;
            m_min_voltage = voltage_before_zoom - (voltage_before_zoom - m_min_voltage) / zoom_factor;
        }
        else if (event.wheel.y < 0)
        {
            m_max_time = time_before_zoom + (m_max_time - time_before_zoom) * zoom_factor;
            m_min_time = time_before_zoom - (time_before_zoom - m_min_time) * zoom_factor;
            m_max_voltage = voltage_before_zoom + (m_max_voltage - voltage_before_zoom) * zoom_factor;
            m_min_voltage = voltage_before_zoom - (voltage_before_zoom - m_min_voltage) * zoom_factor;
        }
    }

    return true;
}

void Plot_View::render()
{
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);
    SDL_RenderClear(m_renderer);

    if (m_signals.empty())
    {
        SDL_RenderPresent(m_renderer);
        return;
    }

    // plot area
    int width, height;
    SDL_GetWindowSize(m_window, &width, &height);
    m_plot_area = {50, 50, width - 100, height - 100};

    // plot area
    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(m_renderer, &m_plot_area);

    // y axis
    const int num_y_ticks = 11;
    if (num_y_ticks > 1)
    {
        double y_tick_value_step = (m_max_voltage - m_min_voltage) / (num_y_ticks - 1);
        for (int i = 0; i < num_y_ticks; ++i)
        {
            double tick_value = m_min_voltage + (i * y_tick_value_step);

            int screen_y = m_plot_area.y + m_plot_area.h - (int)(((tick_value - m_min_voltage) / (m_max_voltage - m_min_voltage)) * m_plot_area.h);

            // grid line
            SDL_SetRenderDrawColor(m_renderer, 220, 220, 220, 255);
            SDL_RenderDrawLine(m_renderer, m_plot_area.x, screen_y, m_plot_area.x + m_plot_area.w, screen_y);


            stringstream ss;
            ss << fixed << setprecision(1) << tick_value;
            string tick_text = ss.str();

            // text label
            int text_w, text_h;
            TTF_SizeText(m_font, tick_text.c_str(), &text_w, &text_h);
            render_text(m_renderer, m_font, tick_text, m_plot_area.x - text_w - 5, screen_y - text_h / 2);
        }
    }

    // x axis
    const int num_x_ticks = 11;
    if (num_x_ticks > 1)
    {
        double x_tick_value_step = (m_max_time - m_min_time) / (num_x_ticks - 1);
        for (int i = 0; i < num_x_ticks; ++i)
        {
            double tick_value = m_min_time + (i * x_tick_value_step);

            int screen_x = m_plot_area.x + (int)(((tick_value - m_min_time) / (m_max_time - m_min_time)) * m_plot_area.w);

            // grid line
            SDL_SetRenderDrawColor(m_renderer, 220, 220, 220, 255);
            SDL_RenderDrawLine(m_renderer, screen_x, m_plot_area.y, screen_x, m_plot_area.y + m_plot_area.h);
            stringstream ss;
            ss << fixed << setprecision(1) << tick_value;
            string tick_text = ss.str();

            // text label
            int text_w, text_h;
            TTF_SizeText(m_font, tick_text.c_str(), &text_w, &text_h);
            render_text(m_renderer, m_font, tick_text, screen_x - text_w / 2, m_plot_area.y + m_plot_area.h + 5);
        }
    }


    // drawing all signals
    for (const auto& signal : m_signals)
    {
        // colors baby!
        SDL_SetRenderDrawColor(m_renderer, signal.color.r, signal.color.g, signal.color.b, signal.color.a);

        if (signal.data_points.size() < 2) continue;

        for (size_t i = 0; i < signal.data_points.size() - 1; ++i)
        {
            SDL_Point p1 = world_to_screen(signal.data_points[i].second, signal.data_points[i].first);
            SDL_Point p2 = world_to_screen(signal.data_points[i+1].second, signal.data_points[i+1].first);
            SDL_RenderDrawLine(m_renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }

    // legends!
    if (!m_signals.empty() && FONT)
    {
        const int legend_y_offset = 15;
        const int box_size = 15;
        const int gap = 5;

        double slot_width = (double)m_plot_area.w / m_signals.size();

        for (int i = 0; i < m_signals.size(); ++i)
        {
            const auto& signal = m_signals[i];

            // text size
            int text_width, text_height;
            TTF_SizeText(m_font, signal.name.c_str(), &text_width, &text_height);

            // total width
            int total_item_width = box_size + gap + text_width;

            // x position to center
            double slot_start_x = m_plot_area.x + (i * slot_width);
            int item_start_x = slot_start_x + (slot_width - total_item_width) / 2;

            // drawing the box
            SDL_Rect color_box = {item_start_x, m_plot_area.y + legend_y_offset, box_size, box_size};
            SDL_SetRenderDrawColor(m_renderer, signal.color.r, signal.color.g, signal.color.b, 255);
            SDL_RenderFillRect(m_renderer, &color_box);

            // displaying text
            int text_x = item_start_x + box_size + gap;
            int text_y = m_plot_area.y + legend_y_offset;
            render_text(m_renderer, m_font, signal.name, text_x, text_y);
        }
    }

    SDL_RenderPresent(m_renderer);
}