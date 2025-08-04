//
// Created by amirh on 2025-07-21.
//

#include "graphical_element.h"

// helper functions

inline SDL_Point snap_to_grid(int x, int y, int grid_size)
{
    int snapped_x = round((float)x / grid_size) * grid_size;
    int snapped_y = round((float)y / grid_size) * grid_size;
    return {snapped_x, snapped_y};
}

inline string format_with_suffix(double value, const string& unit) {
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

void draw_arc(SDL_Renderer* renderer, int center_x, int center_y, int radius, int start_angle_deg, int end_angle_deg, int segments = 20) {
    SDL_Point points[segments + 1];
    float angle_step = (float)(end_angle_deg - start_angle_deg) / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle_rad = (float)(start_angle_deg + i * angle_step) * M_PI / 180.0f;
        points[i].x = center_x + (int)(radius * cos(angle_rad));
        points[i].y = center_y + (int)(radius * sin(angle_rad));
    }
    SDL_RenderDrawLines(renderer, points, segments + 1);
}

TTF_Font* Graphical_Element::font = nullptr;

void Graphical_Element::set_font(TTF_Font* font_)
{
    font = font_;
}

void Graphical_Element::set_rotation_by_int(int rot)
{
    switch (rot)
    {
        case 0:
            rotation = Rotation::Right;
            break;
        case 1:
            rotation = Rotation::Left;
            break;
        case 2:
            rotation = Rotation::Up;
            break;
        case 3:
            rotation = Rotation::Down;
            break;
        default:
            rotation = Rotation::Right;
            break;
    }
}

void Graphical_Element::change_rotation()
{
    if (rotation == Rotation::Right)
        rotation = Rotation::Up;
    else if (rotation == Rotation::Up)
        rotation = Rotation::Left;
    else if (rotation == Rotation::Left)
        rotation = Rotation::Down;
    else
        rotation = Rotation::Right;

    swap(bounding_box.w, bounding_box.h);
}

SDL_Point Graphical_Element::transform_point(SDL_Point point_to_rotate)
{
    int center_x = bounding_box.x + bounding_box.w / 2;
    int center_y = bounding_box.y + bounding_box.h / 2;

    switch (rotation)
    {
        case Rotation::Up:
            return {center_x - point_to_rotate.y, center_y + point_to_rotate.x};
        case Rotation::Left:
            return {center_x - point_to_rotate.x, center_y - point_to_rotate.y};
        case Rotation::Down:
            return {center_x + point_to_rotate.y, center_y - point_to_rotate.x};
        case Rotation::Right:
        default:
            return {center_x + point_to_rotate.x, center_y + point_to_rotate.y};
    }
}

Element *Graphical_Element::get_model() { return model_element; }

// draw functions

void Graphical_Resistor::draw(SDL_Renderer *renderer, bool show_grid)
{
    // bounding box
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    // horizental or vertical
    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int lead_length = half_length / 2.5;

    // coordinates
    SDL_Point local_points[] = {
            {-half_length, 0},
            {-lead_length, 0},
            {(int)(-lead_length + (half_length - lead_length) * 0.33), 10},
            {(int)(lead_length - (half_length - lead_length) * 0.66), -10},
            {(int)(lead_length - (half_length - lead_length) * 0.33), 10},
            {lead_length, 0},
            {half_length, 0}
    };

    // draw the points
    for (size_t i = 0; i < (sizeof(local_points) / sizeof(SDL_Point)) - 1; ++i)
    {
        SDL_Point p1 = transform_point(local_points[i]);
        SDL_Point p2 = transform_point(local_points[i + 1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        double value = model_element->get_value();
        string value_str = format_with_suffix(value, " Ohms");

        int text_y_name = bounding_box.y - 20;
        int text_y_value = bounding_box.y + bounding_box.h + 5;

        render_text(renderer, font, model_element->get_name(), bounding_box.x, text_y_name);
        render_text(renderer, font, value_str, bounding_box.x, text_y_value);
    }
}

void Graphical_Capacitor::draw(SDL_Renderer *renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int plate_height = 20;
    int gap = 4;

    // local points
    SDL_Point lead1_start = {-half_length, 0};
    SDL_Point lead1_end = {-gap, 0};

    SDL_Point plate1_top = {-gap, -plate_height / 2};
    SDL_Point plate1_bottom = {-gap, plate_height / 2};

    SDL_Point plate2_top = {gap, -plate_height / 2};
    SDL_Point plate2_bottom = {gap, plate_height / 2};

    SDL_Point lead2_start = {gap, 0};
    SDL_Point lead2_end = {half_length, 0};

    // draw points
    SDL_Point p1 = transform_point(lead1_start);
    SDL_Point p2 = transform_point(lead1_end);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    p1 = transform_point(plate1_top);
    p2 = transform_point(plate1_bottom);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    p1 = transform_point(plate2_top);
    p2 = transform_point(plate2_bottom);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    p1 = transform_point(lead2_start);
    p2 = transform_point(lead2_end);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    // text
    if (model_element != nullptr)
    {
        double value = model_element->get_value();
        string value_str = format_with_suffix(value, " F");

        int text_y_name = bounding_box.y - 20;
        int text_y_value = bounding_box.y + bounding_box.h + 5;

        render_text(renderer, font, model_element->get_name(), bounding_box.x, text_y_name);
        render_text(renderer, font, value_str, bounding_box.x, text_y_value);
    }
}

void Graphical_Inductor::draw(SDL_Renderer* renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int lead_length = half_length / 3;
    int loops_total_length = (half_length - lead_length) * 2;
    int num_loops = 3;
    int loop_width = loops_total_length / num_loops;
    int radius = 10;

    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-half_length + lead_length, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({half_length - lead_length, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // drawing arc points
    for (int i = 0; i < num_loops; ++i) {
        int local_center_x = (-half_length + lead_length) + (i * loop_width) + (loop_width / 2);

        vector<SDL_Point> arc_points;
        for (int j = 0; j <= 10; ++j)
        {
            float angle_rad = (180.0f + j * 18.0f) * M_PI / 180.0f;
            arc_points.push_back(transform_point({(int)(local_center_x + radius * cos(angle_rad)),(int)(0 + radius * sin(angle_rad))}));
        }
        SDL_RenderDrawLines(renderer, arc_points.data(), arc_points.size());
    }

    // text
    if (model_element != nullptr)
    {
        double value = model_element->get_value();
        string value_str = format_with_suffix(value, " H");

        int text_y_name = bounding_box.y - 20;
        int text_y_value = bounding_box.y + bounding_box.h + 5;

        render_text(renderer, font, model_element->get_name(), bounding_box.x, text_y_name);
        render_text(renderer, font, value_str, bounding_box.x, text_y_value);
    }
}

void Graphical_Current_Source::draw(SDL_Renderer* renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // circle
    vector<SDL_Point> circle_points;
    for (int i = 0; i < 360; i += 10)
    {
        float angle_rad = (float)i * M_PI / 180.0f;
        circle_points.push_back(transform_point({(int)(radius * cos(angle_rad)),(int)(radius * sin(angle_rad))}));
    }
    SDL_RenderDrawLines(renderer, circle_points.data(), circle_points.size());

    // arrow
    SDL_Point arrow_points[] = {
            {-radius / 2, 0}, {radius / 2, 0},
            {radius / 2, 0}, {(int)(radius / 2) - 5, -5},
            {radius / 2, 0}, {(int)(radius / 2) - 5, 5}
    };
    for (size_t i = 0; i < 6; i+=2) {
        SDL_Point p1 = transform_point(arrow_points[i]);
        SDL_Point p2 = transform_point(arrow_points[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        double value = model_element->get_value();
        string value_str = format_with_suffix(value, " A");

        int text_y_name = bounding_box.y - 20;
        int text_y_value = bounding_box.y + bounding_box.h + 5;

        render_text(renderer, font, model_element->get_name(), bounding_box.x, text_y_name);
        render_text(renderer, font, value_str, bounding_box.x, text_y_value);
    }
}

void Graphical_Real_Diode::draw(SDL_Renderer* renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int body_length = (half_length * 2) * 0.7;
    int half_body = body_length / 2;

    // local points
    SDL_Point lead1 = {-half_length, 0};
    SDL_Point tri_base_center = {-half_body, 0};
    SDL_Point tri_top = {-half_body, -15};
    SDL_Point tri_bottom = {-half_body, 15};
    SDL_Point tri_tip = {half_body, 0};
    SDL_Point bar_top = {half_body, -15};
    SDL_Point bar_bottom = {half_body, 15};
    SDL_Point lead2 = {half_length, 0};

    // transform all points
    SDL_Point p_lead1 = transform_point(lead1);
    SDL_Point p_tri_base = transform_point(tri_base_center);
    SDL_Point p_tri_top = transform_point(tri_top);
    SDL_Point p_tri_bottom = transform_point(tri_bottom);
    SDL_Point p_tri_tip = transform_point(tri_tip);
    SDL_Point p_bar_top = transform_point(bar_top);
    SDL_Point p_bar_bottom = transform_point(bar_bottom);
    SDL_Point p_lead2 = transform_point(lead2);

    // leads, triangle, bar
    SDL_RenderDrawLine(renderer, p_lead1.x, p_lead1.y, p_tri_base.x, p_tri_base.y);
    SDL_RenderDrawLine(renderer, p_tri_top.x, p_tri_top.y, p_tri_tip.x, p_tri_tip.y);
    SDL_RenderDrawLine(renderer, p_tri_bottom.x, p_tri_bottom.y, p_tri_tip.x, p_tri_tip.y);
    SDL_RenderDrawLine(renderer, p_tri_top.x, p_tri_top.y, p_tri_bottom.x, p_tri_bottom.y);
    SDL_RenderDrawLine(renderer, p_bar_top.x, p_bar_top.y, p_bar_bottom.x, p_bar_bottom.y);
    SDL_RenderDrawLine(renderer, p_tri_tip.x, p_tri_tip.y, p_lead2.x, p_lead2.y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Zener_Diode::draw(SDL_Renderer* renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int body_length = (half_length * 2) * 0.7;
    int half_body = body_length / 2;

    // local points
    SDL_Point lead1 = {-half_length, 0};
    SDL_Point tri_base_center = {-half_body, 0};
    SDL_Point tri_top = {-half_body, -15};
    SDL_Point tri_bottom = {-half_body, 15};
    SDL_Point tri_tip = {half_body, 0};
    SDL_Point bar_top = {half_body, -15};
    SDL_Point bar_bottom = {half_body, 15};
    SDL_Point lead2 = {half_length, 0};

    // transform all points
    SDL_Point p_lead1 = transform_point(lead1);
    SDL_Point p_tri_base = transform_point(tri_base_center);
    SDL_Point p_tri_top = transform_point(tri_top);
    SDL_Point p_tri_bottom = transform_point(tri_bottom);
    SDL_Point p_tri_tip = transform_point(tri_tip);
    SDL_Point p_bar_top = transform_point(bar_top);
    SDL_Point p_bar_bottom = transform_point(bar_bottom);
    SDL_Point p_lead2 = transform_point(lead2);

    // leads, triangle, bar
    SDL_RenderDrawLine(renderer, p_lead1.x, p_lead1.y, p_tri_base.x, p_tri_base.y);
    SDL_RenderDrawLine(renderer, p_tri_top.x, p_tri_top.y, p_tri_tip.x, p_tri_tip.y);
    SDL_RenderDrawLine(renderer, p_tri_bottom.x, p_tri_bottom.y, p_tri_tip.x, p_tri_tip.y);
    SDL_RenderDrawLine(renderer, p_tri_top.x, p_tri_top.y, p_tri_bottom.x, p_tri_bottom.y);
    SDL_RenderDrawLine(renderer, p_bar_top.x, p_bar_top.y, p_bar_bottom.x, p_bar_bottom.y);
    SDL_RenderDrawLine(renderer, p_tri_tip.x, p_tri_tip.y, p_lead2.x, p_lead2.y);

    // wings
    SDL_Point wing1_start = {half_body, -15};
    SDL_Point wing1_end = {half_body - 8, -10};
    SDL_Point wing2_start = {half_body, 15};
    SDL_Point wing2_end = {half_body + 8, 10};

    SDL_Point p1 = transform_point(wing1_start);
    SDL_Point p2 = transform_point(wing1_end);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    p1 = transform_point(wing2_start);
    p2 = transform_point(wing2_end);
    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Voltage_Source::draw(SDL_Renderer* renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // circle
    vector<SDL_Point> circle_points;
    for (int i = 0; i < 360; i += 10)
    {
        float angle_rad = (float)i * M_PI / 180.0f;
        circle_points.push_back(transform_point({(int)(radius * cos(angle_rad)),(int)(radius * sin(angle_rad))}));
    }
    SDL_RenderDrawLines(renderer, circle_points.data(), circle_points.size());

    // V symbol
//    SDL_Point V_points[] = {
//            {0, +radius / 2}, {(int)(radius / 2) - 5, -(int)(radius / 2)},
//            {0, +radius / 2}, {-(int)(radius / 2) + 5, -(int)(radius / 2)},
//            {(int)(radius / 2) - 5, -(int)(radius / 2)}, {(int)(radius / 2) + 3, -(int)(radius / 2)},
//            {-(int)(radius / 2) + 5, -(int)(radius / 2)}, {-(int)(radius / 2) - 3, -(int)(radius / 2)}
//    };

    // - + symbol
    SDL_Point plus_minus[] = {
            {-5 , 0}, {- 15, 0},
            {5, 0}, {15, 0},
            {10, 5}, {10, -5}
    };

    for (size_t i = 0; i < 6; i+=2) {
        SDL_Point p1 = transform_point(plus_minus[i]);
        SDL_Point p2 = transform_point(plus_minus[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        int text_y_name = bounding_box.y - 20;
        int text_y_value = bounding_box.y + bounding_box.h + 5;

        render_text(renderer, font, model_element->get_name(), bounding_box.x, text_y_name);

        string to_be_printed = "";

        if (auto* dc = dynamic_cast<DC_Source*>(model_element))
        {
            double value = dc->get_value_at(0, 0);
            to_be_printed = format_with_suffix(value, "V");
        }
        else if (auto* sine = dynamic_cast<Sine_Source*>(model_element))
        {
            double off, amp, freq, phase;
            sine->get_parameters(off, amp, freq, phase);

            stringstream ss;
            ss << "SIN(" << format_with_suffix(off, " ") << format_with_suffix(amp, " ") << format_with_suffix(freq, "");
            if (phase != 0.0)
            {
                ss << " " << format_with_suffix(phase, "");
            }
            ss << ")";
            to_be_printed = ss.str();
        }
        else if (auto* pulse = dynamic_cast<Pulse_Source*>(model_element))
        {
            double v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period;
            pulse->get_parameters(v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period);

            stringstream ss;
            ss << "PULSE(" << format_with_suffix(v_initial, " ") << format_with_suffix(v_pulsed, "");
            if (time_delay != 0.0) ss << " " << format_with_suffix(time_delay, "");
            if (time_rise != 0.0) ss << " " << format_with_suffix(time_rise, "");
            if (time_fall != 0.0) ss << " " << format_with_suffix(time_fall, "");
            if (pulse_width != 0.0) ss << " " << format_with_suffix(pulse_width, "");
            if (period != 0.0) ss << " " << format_with_suffix(period, "");
            ss << ")";
            to_be_printed = ss.str();
        }
        else if (auto* square = dynamic_cast<Square_Source*>(model_element))
        {
            double v_down, v_up, time_delay, square_width, period;
            square->get_parameters(v_down, v_up, time_delay, square_width, period);

            stringstream ss;
            ss << "SQUARE(" << format_with_suffix(v_down, " ") << format_with_suffix(v_up, "");
            if (time_delay != 0.0) ss << " " << format_with_suffix(time_delay, "");
            if (square_width != 0.0) ss << " " << format_with_suffix(square_width, "");
            if (period != 0.0) ss << " " << format_with_suffix(period, "");
            ss << ")";
            to_be_printed = ss.str();
        }
        else if (auto* tri = dynamic_cast<Triangular_Source*>(model_element))
        {
            double v_initial, v_peak, time_delay, period;
            tri->get_parameters(v_initial, v_peak, time_delay, period);

            stringstream ss;
            ss << "TRI(" << format_with_suffix(v_initial, " ") << format_with_suffix(v_peak, "");
            if (time_delay != 0.0) ss << " " << format_with_suffix(time_delay, "");
            if (period != 0.0) ss << " " << format_with_suffix(period, "");
            ss << ")";
            to_be_printed = ss.str();
        }
        else if (auto* delta = dynamic_cast<Delta_Dirac*>(model_element))
        {
            double delta_value, not_delta_value, time_of_delta;
            delta->get_parameters(delta_value, not_delta_value, time_of_delta);
            to_be_printed = "DELTA(" + format_with_suffix(time_of_delta, "s)");
        }
        else
        {
            to_be_printed = format_with_suffix(model_element->get_value(), "V");
        }


        render_text(renderer, font, to_be_printed, bounding_box.x, text_y_value);
    }
}

void Graphical_Ground::draw(SDL_Renderer *renderer, bool show_grid)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawLine(renderer, m_position.x, m_position.y, m_position.x, m_position.y + 10);

    int y = m_position.y + 10;
    SDL_RenderDrawLine(renderer, m_position.x - 12, y, m_position.x + 12, y);
    SDL_RenderDrawLine(renderer, m_position.x - 8,  y + 4, m_position.x + 8,  y + 4);
    SDL_RenderDrawLine(renderer, m_position.x - 4,  y + 8, m_position.x + 4,  y + 8);
}

void Graphical_Net_Label::draw(SDL_Renderer *renderer, bool show_grid)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawLine(renderer, m_position.x, m_position.y, m_position.x + 15, m_position.y);

    draw_circle(renderer, m_position.x, m_position.y, 3);

    if (!m_label_text.empty())
    {
        render_text(renderer, font, m_label_text, m_position.x + 20, m_position.y - 8, {0, 0, 0, 255});
    }
}

void Graphical_SubCircuit::draw(SDL_Renderer* renderer, bool show_grid)
{
    // bounding box
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    vector<Connection_Point> points = get_connection_points();

    for (const auto& point : points)
    {
        SDL_Rect square = {
                point.pos.x - 5,
                point.pos.y - 5,
                10,
                10
        };

        SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);
        SDL_RenderFillRect(renderer, &square);
    }

    // text
    if (m_subcircuit_model != nullptr)
    {
        int text_width, text_height;
        TTF_SizeText(font, m_type_name.c_str(), &text_width, &text_height);

        int text_x = bounding_box.x + (bounding_box.w - text_width) / 2;
        int text_y = bounding_box.y + (bounding_box.h - text_height) / 2;

        render_text(renderer, font, m_type_name, text_x, text_y);
    }
}

void Graphical_VCVS::draw(SDL_Renderer *renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = 40;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // controller leads
    SDL_Point lead3_p1 = transform_point({0, -half_length});
    SDL_Point lead3_p2 = transform_point({0, -radius});
    SDL_RenderDrawLine(renderer, lead3_p1.x, lead3_p1.y, lead3_p2.x, lead3_p2.y);

    SDL_Point lead4_p1 = transform_point({0, radius});
    SDL_Point lead4_p2 = transform_point({0, half_length});
    SDL_RenderDrawLine(renderer, lead4_p1.x, lead4_p1.y, lead4_p2.x, lead4_p2.y);

    // diomond
    SDL_Point p_top = transform_point({0, -radius});
    SDL_Point p_right = transform_point({radius, 0});
    SDL_Point p_bottom = transform_point({0, radius});
    SDL_Point p_left = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, p_top.x, p_top.y, p_right.x, p_right.y);
    SDL_RenderDrawLine(renderer, p_right.x, p_right.y, p_bottom.x, p_bottom.y);
    SDL_RenderDrawLine(renderer, p_bottom.x, p_bottom.y, p_left.x, p_left.y);
    SDL_RenderDrawLine(renderer, p_left.x, p_left.y, p_top.x, p_top.y);

    // + -
    SDL_Point plus_minus[] = {
            {5, 0}, {15, 0},
            {10, 5}, {10, -5},
            {-5, 0}, {-15, 0}
    };
    for (size_t i = 0; i < 6; i += 2) {
        SDL_Point p1 = transform_point(plus_minus[i]);
        SDL_Point p2 = transform_point(plus_minus[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
        string value_str = format_with_suffix(model_element->get_value(), "");
        render_text(renderer, font, value_str, bounding_box.x, bounding_box.y + bounding_box.h + 5);
    }
}

void Graphical_VCCS::draw(SDL_Renderer *renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = 40;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // controller leads
    SDL_Point lead3_p1 = transform_point({0, -half_length});
    SDL_Point lead3_p2 = transform_point({0, -radius});
    SDL_RenderDrawLine(renderer, lead3_p1.x, lead3_p1.y, lead3_p2.x, lead3_p2.y);

    SDL_Point lead4_p1 = transform_point({0, radius});
    SDL_Point lead4_p2 = transform_point({0, half_length});
    SDL_RenderDrawLine(renderer, lead4_p1.x, lead4_p1.y, lead4_p2.x, lead4_p2.y);

    // diomond
    SDL_Point p_top = transform_point({0, -radius});
    SDL_Point p_right = transform_point({radius, 0});
    SDL_Point p_bottom = transform_point({0, radius});
    SDL_Point p_left = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, p_top.x, p_top.y, p_right.x, p_right.y);
    SDL_RenderDrawLine(renderer, p_right.x, p_right.y, p_bottom.x, p_bottom.y);
    SDL_RenderDrawLine(renderer, p_bottom.x, p_bottom.y, p_left.x, p_left.y);
    SDL_RenderDrawLine(renderer, p_left.x, p_left.y, p_top.x, p_top.y);

    // arrow-
    SDL_Point arrow_points[] = {
            {-radius / 2, 0}, {radius / 2, 0},
            {radius / 2, 0}, {(int)(radius / 2) - 5, -5},
            {radius / 2, 0}, {(int)(radius / 2) - 5, 5}
    };
    for (size_t i = 0; i < 6; i += 2)
    {
        SDL_Point p1 = transform_point(arrow_points[i]);
        SDL_Point p2 = transform_point(arrow_points[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
        string value_str = format_with_suffix(model_element->get_value(), "");
        render_text(renderer, font, value_str, bounding_box.x, bounding_box.y + bounding_box.h + 5);
    }
}

void Graphical_CCCS::draw(SDL_Renderer *renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = 40;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);

    // diomond
    SDL_Point p_top = transform_point({0, -radius});
    SDL_Point p_right = transform_point({radius, 0});
    SDL_Point p_bottom = transform_point({0, radius});
    SDL_Point p_left = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, p_top.x, p_top.y, p_right.x, p_right.y);
    SDL_RenderDrawLine(renderer, p_right.x, p_right.y, p_bottom.x, p_bottom.y);
    SDL_RenderDrawLine(renderer, p_bottom.x, p_bottom.y, p_left.x, p_left.y);
    SDL_RenderDrawLine(renderer, p_left.x, p_left.y, p_top.x, p_top.y);

    // arrow
    SDL_Point arrow_points[] = {
            {-radius / 2, 0}, {radius / 2, 0},
            {radius / 2, 0}, {(int)(radius / 2) - 5, -5},
            {radius / 2, 0}, {(int)(radius / 2) - 5, 5}
    };
    for (size_t i = 0; i < 6; i += 2)
    {
        SDL_Point p1 = transform_point(arrow_points[i]);
        SDL_Point p2 = transform_point(arrow_points[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
        string value_str = format_with_suffix(model_element->get_value(), "");
        render_text(renderer, font, value_str, bounding_box.x, bounding_box.y + bounding_box.h + 5);
    }
}

void Graphical_CCVS::draw(SDL_Renderer *renderer, bool show_grid)
{
    if (show_grid)
    {
        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &bounding_box);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = 40;
    int radius = 20;

    // leads
    SDL_Point lead1_p1 = transform_point({-half_length, 0});
    SDL_Point lead1_p2 = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, lead1_p1.x, lead1_p1.y, lead1_p2.x, lead1_p2.y);

    SDL_Point lead2_p1 = transform_point({radius, 0});
    SDL_Point lead2_p2 = transform_point({half_length, 0});
    SDL_RenderDrawLine(renderer, lead2_p1.x, lead2_p1.y, lead2_p2.x, lead2_p2.y);


    // diomond
    SDL_Point p_top = transform_point({0, -radius});
    SDL_Point p_right = transform_point({radius, 0});
    SDL_Point p_bottom = transform_point({0, radius});
    SDL_Point p_left = transform_point({-radius, 0});
    SDL_RenderDrawLine(renderer, p_top.x, p_top.y, p_right.x, p_right.y);
    SDL_RenderDrawLine(renderer, p_right.x, p_right.y, p_bottom.x, p_bottom.y);
    SDL_RenderDrawLine(renderer, p_bottom.x, p_bottom.y, p_left.x, p_left.y);
    SDL_RenderDrawLine(renderer, p_left.x, p_left.y, p_top.x, p_top.y);

    // + -
    SDL_Point plus_minus[] = {
            {5, 0}, {15, 0},
            {10, 5}, {10, -5},
            {-5, 0}, {-15, 0}
    };
    for (size_t i = 0; i < 6; i += 2)
    {
        SDL_Point p1 = transform_point(plus_minus[i]);
        SDL_Point p2 = transform_point(plus_minus[i+1]);
        SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    }

    // text
    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
        string value_str = format_with_suffix(model_element->get_value(), "");
        render_text(renderer, font, value_str, bounding_box.x, bounding_box.y + bounding_box.h + 5);
    }
}

// get properties functions

vector<Editable_Property> Graphical_Resistor::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Resistance (Ohms)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Capacitor::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Capacitance (F)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Inductor::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Inductance (H)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Current_Source::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Current (A)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Real_Diode::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Voltage (V)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Zener_Diode::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Voltage (V)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_Voltage_Source::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});

    if (auto* dc = dynamic_cast<DC_Source*>(model_element))
    {
        props.push_back({"Value (V)", to_string(dc->get_value_at(0, 0))});
    }
    else if (auto* sine = dynamic_cast<Sine_Source*>(model_element))
    {
        double off, amp, freq, phase;
        sine->get_parameters(off, amp, freq, phase);
        props.push_back({"Offset (V)", to_string(off)});
        props.push_back({"Amplitude (V)", to_string(amp)});
        props.push_back({"Frequency (Hz)", to_string(freq)});
        props.push_back({"Phase (deg)", to_string(phase)});
    }
    else if (auto* pulse = dynamic_cast<Pulse_Source*>(model_element))
    {
        double v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period;
        pulse->get_parameters(v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period);
        props.push_back({"Initial V", to_string(v_initial)});
        props.push_back({"Pulsed V", to_string(v_pulsed)});
        props.push_back({"Delay (s)", to_string(time_delay)});
        props.push_back({"Rise Time (s)", to_string(time_rise)});
        props.push_back({"Fall Time (s)", to_string(time_fall)});
        props.push_back({"Pulse Width (s)", to_string(pulse_width)});
        props.push_back({"Period (s)", to_string(period)});
    }
    else if (auto* square = dynamic_cast<Square_Source*>(model_element))
    {
        double v_down, v_up, time_delay, square_width, period;
        square->get_parameters(v_down, v_up, time_delay, square_width, period);
        props.push_back({"Low V", to_string(v_down)});
        props.push_back({"High V", to_string(v_up)});
        props.push_back({"Delay (s)", to_string(time_delay)});
        props.push_back({"Pulse Width (s)", to_string(square_width)});
        props.push_back({"Period (s)", to_string(period)});
    }
    else if (auto* tri = dynamic_cast<Triangular_Source*>(model_element))
    {
        double v_initial, v_peak, time_delay, period;
        tri->get_parameters(v_initial, v_peak, time_delay, period);
        props.push_back({"Initial V", to_string(v_initial)});
        props.push_back({"Peak V", to_string(v_peak)});
        props.push_back({"Delay (s)", to_string(time_delay)});
        props.push_back({"Period (s)", to_string(period)});
    }
    else if (auto* delta = dynamic_cast<Delta_Dirac*>(model_element))
    {
        double delta_val, not_delta_val, time;
        delta->get_parameters(delta_val, not_delta_val, time);
        props.push_back({"Time (s)", to_string(time)});
    }

    return props;
}

vector<Editable_Property> Graphical_SubCircuit::get_editable_properties()
{
    vector<Editable_Property> props;
    props.push_back({"Name", m_type_name});
    return props;
}

vector<Editable_Property> Graphical_VCVS::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Voltage (V)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_VCCS::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Current (A)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_CCVS::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Voltage (V)", to_string(model_element->get_value())});
    return props;
}

vector<Editable_Property> Graphical_CCCS::get_editable_properties()
{
    vector<Editable_Property> props;

    props.push_back({"Name", model_element->get_name()});
    props.push_back({"Current (A)", to_string(model_element->get_value())});
    return props;
}

// get connection points functions

vector<Connection_Point> Graphical_Resistor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Capacitor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Inductor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Current_Source::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Real_Diode::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Zener_Diode::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_Voltage_Source::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_SubCircuit::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }
    auto* model = get_subcircuit_model();
    return {
            {final_screen_start, model ? model->getInput() : nullptr, start_port_orientation},
            {final_screen_end, model ? model->getOutput() : nullptr, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_VCVS::get_connection_points()
{
    int half_length = 40;

    SDL_Point local_left = {-half_length, 0};
    SDL_Point local_right = {half_length, 0};
    SDL_Point local_top = {0, -half_length};
    SDL_Point local_bottom = {0, half_length};

    // transform
    SDL_Point final_left = snap_to_grid(transform_point(local_left).x, transform_point(local_left).y, 10);
    SDL_Point final_right = snap_to_grid(transform_point(local_right).x, transform_point(local_right).y, 10);
    SDL_Point final_top = snap_to_grid(transform_point(local_top).x, transform_point(local_top).y, 10);
    SDL_Point final_bottom = snap_to_grid(transform_point(local_bottom).x, transform_point(local_bottom).y, 10);

    // correct orientation
    Rotation rot_left, rot_right, rot_top, rot_bottom;
    switch (this->rotation)
    {
        case Rotation::Right:
            rot_left = Rotation::Left; rot_right = Rotation::Right; rot_top = Rotation::Up; rot_bottom = Rotation::Down;
            break;
        case Rotation::Down:
            rot_left = Rotation::Up; rot_right = Rotation::Down; rot_top = Rotation::Right; rot_bottom = Rotation::Left;
            break;
        case Rotation::Left:
            rot_left = Rotation::Right; rot_right = Rotation::Left; rot_top = Rotation::Down; rot_bottom = Rotation::Up;
            break;
        case Rotation::Up:
            rot_left = Rotation::Down; rot_right = Rotation::Up; rot_top = Rotation::Left; rot_bottom = Rotation::Right;
            break;
    }

    auto* vcvs_model = dynamic_cast<VCVS*>(model_element);
    auto output_nodes = vcvs_model->get_nodes();
    auto control_nodes = vcvs_model->get_dependent_nodes();

    return {
            {final_left,   output_nodes.second,  rot_left},
            {final_right,  output_nodes.first,   rot_right},
            {final_top,    control_nodes.first,  rot_top},
            {final_bottom, control_nodes.second, rot_bottom}
    };
}

vector<Connection_Point> Graphical_VCCS::get_connection_points()
{
    int half_length = 40;

    SDL_Point local_left = {-half_length, 0};
    SDL_Point local_right = {half_length, 0};
    SDL_Point local_top = {0, -half_length};
    SDL_Point local_bottom = {0, half_length};

    // transform
    SDL_Point final_left = snap_to_grid(transform_point(local_left).x, transform_point(local_left).y, 10);
    SDL_Point final_right = snap_to_grid(transform_point(local_right).x, transform_point(local_right).y, 10);
    SDL_Point final_top = snap_to_grid(transform_point(local_top).x, transform_point(local_top).y, 10);
    SDL_Point final_bottom = snap_to_grid(transform_point(local_bottom).x, transform_point(local_bottom).y, 10);

    // correct orientation
    Rotation rot_left, rot_right, rot_top, rot_bottom;
    switch (this->rotation)
    {
        case Rotation::Right:
            rot_left = Rotation::Left; rot_right = Rotation::Right; rot_top = Rotation::Up; rot_bottom = Rotation::Down;
            break;
        case Rotation::Down:
            rot_left = Rotation::Up; rot_right = Rotation::Down; rot_top = Rotation::Right; rot_bottom = Rotation::Left;
            break;
        case Rotation::Left:
            rot_left = Rotation::Right; rot_right = Rotation::Left; rot_top = Rotation::Down; rot_bottom = Rotation::Up;
            break;
        case Rotation::Up:
            rot_left = Rotation::Down; rot_right = Rotation::Up; rot_top = Rotation::Left; rot_bottom = Rotation::Right;
            break;
    }

    auto* vccs_model = dynamic_cast<VCCS*>(model_element);
    auto output_nodes = vccs_model->get_nodes();
    auto control_nodes = vccs_model->get_dependent_nodes();

    return {
            {final_left,   output_nodes.second,  rot_left},
            {final_right,  output_nodes.first,   rot_right},
            {final_top,    control_nodes.first,  rot_top},
            {final_bottom, control_nodes.second, rot_bottom}
    };
}

vector<Connection_Point> Graphical_CCVS::get_connection_points()
{
    int half_length = 40;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

vector<Connection_Point> Graphical_CCCS::get_connection_points()
{
    int half_length = 40;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    // transforming points
    SDL_Point raw_screen_start = transform_point(local_start);
    SDL_Point raw_screen_end = transform_point(local_end);

    SDL_Point final_screen_start = snap_to_grid(raw_screen_start.x, raw_screen_start.y, 10);
    SDL_Point final_screen_end = snap_to_grid(raw_screen_end.x, raw_screen_end.y, 10);

    Rotation start_port_orientation;
    Rotation end_port_orientation;

    switch (this->rotation)
    {
        case Rotation::Right:
            start_port_orientation = Rotation::Left;
            end_port_orientation = Rotation::Right;
            break;
        case Rotation::Down:
            start_port_orientation = Rotation::Up;
            end_port_orientation = Rotation::Down;
            break;
        case Rotation::Left:
            start_port_orientation = Rotation::Right;
            end_port_orientation = Rotation::Left;
            break;
        case Rotation::Up:
            start_port_orientation = Rotation::Down;
            end_port_orientation = Rotation::Up;
            break;
    }

    auto nodes = model_element->get_nodes();

    return {
            {final_screen_start, nodes.first, start_port_orientation},
            {final_screen_end, nodes.second, end_port_orientation}
    };
}

// get info text

string Graphical_Resistor::get_info_text()
{
    info_text = model_element->get_name() + " " + format_with_suffix(model_element->get_value(), " Ohms");
    return info_text;
}

string Graphical_Capacitor::get_info_text()
{
    info_text = model_element->get_name() + " " + format_with_suffix(model_element->get_value(), " F");
    return info_text;
}

string Graphical_Inductor::get_info_text()
{
    info_text = model_element->get_name() + " " + format_with_suffix(model_element->get_value(), " H");
    return info_text;
}

string Graphical_Current_Source::get_info_text()
{
    info_text = model_element->get_name() + " " + format_with_suffix(model_element->get_value(), " A");
    return info_text;
}

string Graphical_Real_Diode::get_info_text()
{
    info_text = model_element->get_name();
    return info_text;
}

string Graphical_Zener_Diode::get_info_text()
{
    info_text = model_element->get_name();
    return info_text;
}

string Graphical_Voltage_Source::get_info_text()
{
    if (auto* dc = dynamic_cast<DC_Source*>(model_element))
    {
        info_text = format_with_suffix(model_element->get_value(), " V");
    }
    else if (auto* sine = dynamic_cast<Sine_Source*>(model_element))
    {
        double off, amp, freq, phase;
        sine->get_parameters(off, amp, freq, phase);

        info_text = "Offset=" + format_with_suffix(off, "V") +
                    ", Ampl=" + format_with_suffix(amp, "V") +
                    ", Freq=" + format_with_suffix(freq, "Hz");
    }
    else if (auto* pulse = dynamic_cast<Pulse_Source*>(model_element))
    {
        double v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period;
        pulse->get_parameters(v_initial, v_pulsed, time_delay, time_rise, time_fall, pulse_width, period);

        info_text = "V1=" + format_with_suffix(v_initial, "V") +
                    ", V2=" + format_with_suffix(v_pulsed, "V") +
                    ", Td=" + format_with_suffix(time_delay, "s") +
                    ", PW=" + format_with_suffix(pulse_width, "s") +
                    ", Per=" + format_with_suffix(period, "s");
    }
    else if (auto* square = dynamic_cast<Square_Source*>(model_element))
    {
        double v_down, v_up, time_delay, square_width, period;
        square->get_parameters(v_down, v_up, time_delay, square_width, period);

        info_text = "V1=" + format_with_suffix(v_down, "V") +
                    ", V2=" + format_with_suffix(v_up, "V") +
                    ", Td=" + format_with_suffix(time_delay, "s") +
                    ", PW=" + format_with_suffix(square_width, "s") +
                    ", Per=" + format_with_suffix(period, "s");
    }
    else if (auto* tri = dynamic_cast<Triangular_Source*>(model_element))
    {
        double v_initial, v_peak, time_delay, period;
        tri->get_parameters(v_initial, v_peak, time_delay, period);

        info_text = "V1=" + format_with_suffix(v_initial, "V") +
                    ", V2=" + format_with_suffix(v_peak, "V") +
                    ", Td=" + format_with_suffix(time_delay, "s") +
                    ", Per=" + format_with_suffix(period, "s");
    }
    else if (auto* delta = dynamic_cast<Delta_Dirac*>(model_element))
    {
        double delta_value, not_delta_value, time_of_delta;
        delta->get_parameters(delta_value, not_delta_value, time_of_delta);

        info_text = "Time=" + format_with_suffix(time_of_delta, "s");
    }

    return info_text;
}

string Graphical_Ground::get_info_text()
{
    info_text = "Is Ground, Node Name = " + m_node->get_name();
    return info_text;
}

string Graphical_Net_Label::get_info_text()
{
    info_text = "Net Name = " + m_label_text;
    return info_text;
}

string Graphical_SubCircuit::get_info_text()
{
    info_text = "Black Box Name = " + m_subcircuit_model->get_name();
    return info_text;
}

string Graphical_VCVS::get_info_text()
{
    return "";
}

string Graphical_VCCS::get_info_text()
{
    return "";

}

string Graphical_CCVS::get_info_text()
{
    return "";

}

string Graphical_CCCS::get_info_text()
{
    return "";

}