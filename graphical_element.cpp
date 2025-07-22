//
// Created by amirh on 2025-07-21.
//

#include "graphical_element.h"

// helper functions

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

void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius)
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

void Graphical_Resistor::draw(SDL_Renderer *renderer)
{
    // bounding box
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

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

void Graphical_Capacitor::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

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

void Graphical_Inductor::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

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

void Graphical_Current_Source::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 255);

    int half_length = max(bounding_box.w, bounding_box.h) / 2;
    int radius = 15;

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

void Graphical_Real_Diode::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

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

void Graphical_Zener_Diode::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);

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

// get connection points functions

vector<Connection_Point> Graphical_Resistor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}

vector<Connection_Point> Graphical_Capacitor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}

vector<Connection_Point> Graphical_Inductor::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}

vector<Connection_Point> Graphical_Current_Source::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}

vector<Connection_Point> Graphical_Real_Diode::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}

vector<Connection_Point> Graphical_Zener_Diode::get_connection_points()
{
    int half_length = std::max(bounding_box.w, bounding_box.h) / 2;
    SDL_Point local_start = {-half_length, 0};
    SDL_Point local_end = {half_length, 0};

    auto nodes = model_element->get_nodes();

    return {
            {transform_point(local_start), nodes.first},
            {transform_point(local_end), nodes.second}
    };
}