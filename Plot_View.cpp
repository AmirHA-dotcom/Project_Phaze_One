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

inline string format_with_suffix(double value, const string& unit)
{
    if (value == 0.0) return "0.00 " + unit;

    static const struct { double threshold; const char* suffix; } suffixes[] = {
            {1e12, "T"}, {1e9, "G"}, {1e6, "M"}, {1e3, "k"},
            {1.0,  ""},
            {1e-3, "m"}, {1e-6, "u"}, {1e-9, "n"}, {1e-12, "p"}, {1e-15, "f"}
    };

    double abs_value = abs(value);
    string prefix = (value < 0) ? "-" : "";

    for (const auto& s : suffixes)
    {
        if (abs_value >= s.threshold)
        {
            double scaled_value = value / s.threshold;
            stringstream ss;
            ss << fixed << setprecision(2) << scaled_value;
            return prefix + ss.str() + s.suffix + unit;
        }
    }
    // using scientific notation for very small numbers
    stringstream ss;
    ss << scientific << setprecision(2) << value;
    return ss.str() + unit;
}

inline void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius)
{
    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int err = tx - (radius << 1);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, center_x + x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y - y);
        SDL_RenderDrawPoint(renderer, center_x - x, center_y + y);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x + y, center_y + x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y - x);
        SDL_RenderDrawPoint(renderer, center_x - y, center_y + x);

        if (err <= 0) {
            y++;
            err += ty;
            ty += 2;
        }
        if (err > 0) {
            x--;
            tx += 2;
            err += tx - (radius << 1);
        }
    }
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

SDL_Point Plot_View::world_to_screen(double x, double y)
{
    // mapping in x axis
    int screen_x = plot_area.x + (int)(((x - min_x) / (max_x - min_x)) * plot_area.w);
    // mapping in y axis
    int screen_y = plot_area.y + plot_area.h - (int)(((y - min_y) / (max_y - min_y)) * plot_area.h);
    return {screen_x, screen_y};
}

// main functions

void Plot_View::auto_zoom()
{
    if (m_signals.empty()) return;

    // finding the min and the max to scale
    min_x = m_signals[0].data_points.front().second;
    max_x = m_signals[0].data_points.front().second;
    min_y = m_signals[0].data_points.front().first;
    max_y = m_signals[0].data_points.front().first;

    for (const auto& signal : m_signals)
    {
        for (const auto& point : signal.data_points)
        {
            if (point.second < min_x) min_x = point.second;
            if (point.second > max_x) max_x = point.second;
            if (point.first < min_y) min_y = point.first;
            if (point.first > max_y) max_y = point.first;
        }
    }

    // symmetrize and add margin
    if (max_y == min_y)
    {
        max_y += 0.5;
        min_y -= 0.5;
    }
    double voltage_range = max_y - min_y;
    min_y -= voltage_range * 0.1;
    max_y += voltage_range * 0.1;

    double time_range = max_x - min_x;
    if (time_range == 0) { time_range = 1.0; }
    min_x -= time_range * 0.1;
    max_x += time_range * 0.1;
}

bool Plot_View::handle_event(SDL_Event& event)
{
    if (event.type == SDL_WINDOWEVENT && event.window.windowID == SDL_GetWindowID(m_window))
    {
        if (event.window.event == SDL_WINDOWEVENT_CLOSE)
        {
            return false;
        }
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_SPACE:
                auto_zoom();
                break;

            case SDLK_c:
                cursor_mode_active = !cursor_mode_active;
                if (!cursor_mode_active)
                {
                    cursor1.reset();
                    cursor2.reset();
                }
                break;
        }
    }

    // colors menu
    if (m_color_menu_active) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            SDL_Point mouse_pos = { event.button.x, event.button.y };
            bool clicked_in_menu = false;
            for (int i = 0; i < m_color_picker_buttons.size(); ++i) {
                if (SDL_PointInRect(&mouse_pos, &m_color_picker_buttons[i])) {
                    // A color was clicked! Apply it to the signal and close the menu.
                    m_signals[m_signal_to_edit_index].color = default_colors[i];
                    m_color_menu_active = false;
                    clicked_in_menu = true;
                    break;
                }
            }
            // If the user clicks anywhere, close the menu
            m_color_menu_active = false;
        }
        return true; // Consume all events while the menu is open
    }

    // panning
    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    bool ctrl_is_pressed = keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL];

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && ctrl_is_pressed)
    {

        is_panning = true;
        pan_start_pos = {event.button.x, event.button.y };

    }

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        is_panning = false;
    }

    if (event.type == SDL_MOUSEMOTION && is_panning)
    {
        // cursor movement
        int dx = event.motion.x - pan_start_pos.x;
        int dy = event.motion.y - pan_start_pos.y;

        // convert that pixel delta to a world coordinate delta
        double time_per_pixel = (max_x - min_x) / plot_area.w;
        double voltage_per_pixel = (max_y - min_y) / plot_area.h;

        // shifting the min and max
        min_x -= dx * time_per_pixel;
        max_x -= dx * time_per_pixel;
        min_y += dy * voltage_per_pixel;
        max_y += dy * voltage_per_pixel;

        pan_start_pos = {event.motion.x, event.motion.y };
    }

    // zoom in and out
    if (event.type == SDL_MOUSEWHEEL)
    {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);

        // world coordinates under the mouse
        double time_before_zoom = min_x + ((double)(mouse_x - plot_area.x) / plot_area.w) * (max_x - min_x);
        double voltage_before_zoom = min_y + ((double)(plot_area.y + plot_area.h - mouse_y) / plot_area.h) * (max_y - min_y);

        // zoom factor
        double zoom_factor = 1.1;
        if (event.wheel.y > 0)
        {
            max_x = time_before_zoom + (max_x - time_before_zoom) / zoom_factor;
            min_x = time_before_zoom - (time_before_zoom - min_x) / zoom_factor;
            max_y = voltage_before_zoom + (max_y - voltage_before_zoom) / zoom_factor;
            min_y = voltage_before_zoom - (voltage_before_zoom - min_y) / zoom_factor;
        }
        else if (event.wheel.y < 0)
        {
            max_x = time_before_zoom + (max_x - time_before_zoom) * zoom_factor;
            min_x = time_before_zoom - (time_before_zoom - min_x) * zoom_factor;
            max_y = voltage_before_zoom + (max_y - voltage_before_zoom) * zoom_factor;
            min_y = voltage_before_zoom - (voltage_before_zoom - min_y) * zoom_factor;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        if (!cursor_mode_active)
        {
            SDL_Point mouse_pos = { event.button.x, event.button.y };
            for (int i = 0; i < m_legend_rects.size(); ++i) {
                if (SDL_PointInRect(&mouse_pos, &m_legend_rects[i])) {
                    m_color_menu_active = true;
                    m_signal_to_edit_index = i;
                    m_color_menu_pos = { event.button.x, event.button.y };
                    return true; // Event handled, don't place a cursor
                }
            }
        }

        // place cursor
        if (m_signals.empty() || m_signals[0].data_points.empty()) return true;

        // convert screen X coordinate to world x
        double time_at_click = min_x + ((double)(event.button.x - plot_area.x) / plot_area.w) * (max_x - min_x);

        // finding the closest data point on the first signal
        const auto& data = m_signals[0].data_points;
        size_t closest_index = 0;
        double min_dist = abs(data[0].second - time_at_click);

        for (size_t i = 1; i < data.size(); ++i)
        {
            double dist = abs(data[i].second - time_at_click);
            if (dist < min_dist)
            {
                min_dist = dist;
                closest_index = i;
            }
        }

        Cursor new_cursor;
        new_cursor.X = data[closest_index].second;
        new_cursor.Y = data[closest_index].first;
        new_cursor.data_point_index = closest_index;

        if (place_first_cursor_next)
        {
            cursor1 = new_cursor;
        }
        else
        {
            cursor2 = new_cursor;
        }
        place_first_cursor_next = !place_first_cursor_next;
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
    plot_area = {100, 50, width - 100, height - 100};

    SDL_SetRenderDrawColor(m_renderer, 100, 100, 100, 255);
    SDL_RenderDrawRect(m_renderer, &plot_area);

    // y axis
    const int num_y_ticks = 11;
    if (num_y_ticks > 1)
    {
        double y_tick_value_step = (max_y - min_y) / (num_y_ticks - 1);
        for (int i = 0; i < num_y_ticks; ++i)
        {
            double tick_value = min_y + (i * y_tick_value_step);

            int screen_y = plot_area.y + plot_area.h - (int)(((tick_value - min_y) / (max_y - min_y)) * plot_area.h);

            // grid line
            SDL_SetRenderDrawColor(m_renderer, 220, 220, 220, 255);
            SDL_RenderDrawLine(m_renderer, plot_area.x, screen_y, plot_area.x + plot_area.w, screen_y);
            string unit = " ";
            if (y_axis_unit == Unit::V) {unit += "V"; }
            else if (y_axis_unit == Unit::A) {unit += "A"; }
            else if (y_axis_unit == Unit::W) {unit += "W"; }

            string tick_text = format_with_suffix(tick_value, unit);

            // text label
            int text_w, text_h;
            TTF_SizeText(m_font, tick_text.c_str(), &text_w, &text_h);
            render_text(m_renderer, m_font, tick_text, plot_area.x - text_w - 5, screen_y - text_h / 2);
        }
    }

    // x axis
    const int num_x_ticks = 11;
    if (num_x_ticks > 1)
    {
        double x_tick_value_step = (max_x - min_x) / (num_x_ticks - 1);
        for (int i = 0; i < num_x_ticks; ++i)
        {
            double tick_value = min_x + (i * x_tick_value_step);

            int screen_x = plot_area.x + (int)(((tick_value - min_x) / (max_x - min_x)) * plot_area.w);

            // grid line
            SDL_SetRenderDrawColor(m_renderer, 220, 220, 220, 255);
            SDL_RenderDrawLine(m_renderer, screen_x, plot_area.y, screen_x, plot_area.y + plot_area.h);

            string unit = " ";
            if (y_axis_unit == Unit::s) {unit += "s"; }

            string tick_text = format_with_suffix(tick_value, unit);

            // text label
            int text_w, text_h;
            TTF_SizeText(m_font, tick_text.c_str(), &text_w, &text_h);
            render_text(m_renderer, m_font, tick_text, screen_x - text_w / 2, plot_area.y + plot_area.h + 5);
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
        const int legend_y_offset = 45;
        const int box_size = 15;
        const int gap = 5;

        double slot_width = (double)plot_area.w / m_signals.size();

        for (int i = 0; i < m_signals.size(); ++i)
        {
            const auto& signal = m_signals[i];

            // text size
            int text_width, text_height;
            TTF_SizeText(m_font, signal.name.c_str(), &text_width, &text_height);

            // total width
            int total_item_width = box_size + gap + text_width;

            // x position to center
            double slot_start_x = plot_area.x + (i * slot_width);
            int item_start_x = slot_start_x + (slot_width - total_item_width) / 2;

            // drawing the box
            SDL_Rect color_box = {item_start_x, plot_area.y - legend_y_offset, box_size, box_size};
            SDL_SetRenderDrawColor(m_renderer, signal.color.r, signal.color.g, signal.color.b, 255);
            SDL_RenderFillRect(m_renderer, &color_box);

            // displaying text
            int text_x = item_start_x + box_size + gap;
            int text_y = plot_area.y - legend_y_offset;
            render_text(m_renderer, m_font, signal.name, text_x, text_y);

            // saving the rects to click
            SDL_Rect legend_item_rect = { item_start_x, text_y, total_item_width, text_height };
            m_legend_rects.push_back(legend_item_rect);
        }
    }

    // cursors
    if (cursor_mode_active)
    {
        auto draw_cursor = [&](const Cursor& cursor){
            // convert cursor x to screen x
            int screen_x = world_to_screen(cursor.X, cursor.Y).x;
            int screen_y = world_to_screen(cursor.X, cursor.Y).y;

            // drawing a circle
//            SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
//            draw_circle(m_renderer, cursor.X, cursor.Y, 20);

            // drawing dashes
            SDL_SetRenderDrawColor(m_renderer, 255, 0, 0, 255);
            for (int y = plot_area.y; y < plot_area.y + plot_area.h; y += 10)
            {
                SDL_RenderDrawLine(m_renderer, screen_x, y, screen_x, y + 5);
            }
            for (int x = plot_area.x; x < plot_area.x + plot_area.w; x += 10)
            {
                SDL_RenderDrawLine(m_renderer, x, screen_y, x + 5, screen_y);
            }
        };

        if (cursor1) draw_cursor(*cursor1);
        if (cursor2) draw_cursor(*cursor2);

        // info box
        if (cursor1 && cursor2)
        {
            double delta_y = cursor2->Y - cursor1->Y;
            double delta_x = cursor2->X - cursor1->X;
            double slope = (delta_x == 0) ? 0 : (delta_y / delta_x);

            string y_unit = "";
            if (y_axis_unit == Unit::V) { y_unit += "V"; }
            else if (y_axis_unit == Unit::A) { y_unit += "A"; }
            else if (y_axis_unit == Unit::W) {y_unit += "W"; }

            string x_unit = "";
            if (x_axis_unit == Unit::s) { x_unit += "s"; }

            stringstream ss;
            ss << "H-Diff: " << format_with_suffix(delta_x, " " + x_unit);
            string h_diff_str = ss.str(); ss.str("");
            ss << "V-Diff: " << format_with_suffix(delta_y, " " + y_unit);
            string v_diff_str = ss.str(); ss.str("");
            ss << "Slope: " << format_with_suffix(slope, " " + y_unit + "/" + x_unit);
            string slope_str = ss.str();

            // box
            SDL_Rect info_box = {plot_area.x + 10, plot_area.y + 10, 200, 70};
            SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 220);
            SDL_RenderFillRect(m_renderer, &info_box);
            SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(m_renderer, &info_box);

            // text
            render_text(m_renderer, m_font, h_diff_str, info_box.x + 5, info_box.y + 5);
            render_text(m_renderer, m_font, v_diff_str, info_box.x + 5, info_box.y + 25);
            render_text(m_renderer, m_font, slope_str, info_box.x + 5, info_box.y + 45);
        }
    }

    if (m_color_menu_active)
    {
        draw_color_picker_menu();
    }

    SDL_RenderPresent(m_renderer);
}

void Plot_View::draw_color_picker_menu()
{
    const int swatch_size = 20;
    const int padding = 5;
    const int items_per_row = 5;
    const int num_rows = (default_colors.size() + items_per_row - 1) / items_per_row;

    int menu_width = (swatch_size * items_per_row) + (padding * (items_per_row + 1));
    int menu_height = (swatch_size * num_rows) + (padding * (num_rows + 1));

    SDL_Rect panel = { m_color_menu_pos.x, m_color_menu_pos.y, menu_width, menu_height };

    // background
    SDL_SetRenderDrawColor(m_renderer, 50, 58, 69, 255);
    SDL_RenderFillRect(m_renderer, &panel);

    m_color_picker_buttons.clear();
    for (int i = 0; i < default_colors.size(); ++i)
    {
        int row = i / items_per_row;
        int col = i % items_per_row;

        SDL_Rect swatch_rect ={
                panel.x + padding + col * (swatch_size + padding),
                panel.y + padding + row * (swatch_size + padding),
                swatch_size,
                swatch_size
        };
        m_color_picker_buttons.push_back(swatch_rect);

        // draw the color swatch
        const auto& color = default_colors[i];
        SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(m_renderer, &swatch_rect);
    }
}