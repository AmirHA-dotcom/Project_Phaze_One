//
// Created by amirh on 2025-07-20.
//

#include "graphical_view.h"

// helper functions

const char* FONT = "D:/Fonts/Roboto/static/Roboto-Regular.ttf";
const char* PROBE = "D://Images//probe_cursor.png";
const char* DELETE = "D://Images//sissors_cursor.png";
const char* GROUND = "D://Images//grounding_cursor.png";
const char* ICON = "D://Images//SparkSense2.png";
const char* TICK = "D://Images//tick_40x40.png";

//const char* FONT = "/Users/arian/Desktop/OOP/PNGs & FONTs/Athelas.ttc";
//const char* PROBE = "/Users/arian/Desktop/OOP/PNGs & FONTs/probe_cursor.png";
//const char* DELETE = "/Users/arian/Desktop/OOP/PNGs & FONTs/sissors_cursor.png";
//const char* GROUND = "/Users/arian/Desktop/OOP/PNGs & FONTs/grounding_cursor.png";
//const char* ICON = "/Users/arian/Desktop/OOP/PNGs & FONTs/SparkSense2.png";
//const char* TICK = "/Users/arian/Desktop/OOP/PNGs & FONTs/tick_40x40.png";


inline SDL_Point snap_to_grid(int x, int y, int grid_size)
{
    int snapped_x = round((float)x / grid_size) * grid_size;
    int snapped_y = round((float)y / grid_size) * grid_size;
    return {snapped_x, snapped_y};
}

float dist_to_segment(SDL_Point p, SDL_Point v, SDL_Point w)
{
    float l2 = (v.x - w.x)*(v.x - w.x) + (v.y - w.y)*(v.y - w.y);
    if (l2 == 0.0) return sqrt((p.x - v.x)*(p.x - v.x) + (p.y - v.y)*(p.y - v.y));
    float t = ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2;
    t = max(0.0f, min(1.0f, t));
    SDL_Point projection = { int(v.x + t * (w.x - v.x)), int(v.y + t * (w.y - v.y)) };
    return sqrt((p.x - projection.x)*(p.x - projection.x) + (p.y - projection.y)*(p.y - projection.y));
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

Node* graphical_view::find_node_at(SDL_Point pos, Controller* C)
{
    const float CLICK_TOLERANCE = 10.0f;
    auto& wires = C->get_graphical_wires();

    for (const auto& wire : wires)
    {
        for (size_t i = 0; i < wire->path.size() - 1; ++i)
        {
            SDL_Point p1 = wire->path[i];
            SDL_Point p2 = wire->path[i+1];

            if (dist_to_segment(pos, p1, p2) < CLICK_TOLERANCE)
            {
                return wire->start_node;
            }
        }
    }

    auto& elements = C->get_graphical_elements();
    for (const auto& element : elements)
    {
        auto connection_points = element->get_connection_points();
        for (const auto& point : connection_points)
        {
            if (abs(pos.x - point.pos.x) < 5 && abs(pos.y - point.pos.y) < 5)
            {
                return point.node;
            }
        }
    }

    return nullptr;
}

Graphical_Element* graphical_view::find_element_at(SDL_Point pos, Controller *C)
{
    auto& elements = C->get_graphical_elements();

    for (int i = elements.size() - 1; i >= 0; --i)
    {
        if (SDL_PointInRect(&pos, &elements[i]->bounding_box))
        {
            return elements[i].get();
        }
    }
    return nullptr;
}

Graphical_Wire* graphical_view::find_wire_at(SDL_Point pos, Controller *C)
{
    const float CLICK_TOLERANCE = 10.0f;
    auto& wires = C->get_graphical_wires();

    for (const auto& wire : wires)
    {
        for (size_t i = 0; i < wire->path.size() - 1; ++i)
        {
            SDL_Point p1 = wire->path[i];
            SDL_Point p2 = wire->path[i+1];

            if (dist_to_segment(pos, p1, p2) < CLICK_TOLERANCE)
            {
                return wire.get();
            }
        }
    }
    return nullptr;
}

int graphical_view::find_connection_point_at(Graphical_Element* element, SDL_Point click_pos)
{
    if (!element)
    {
        return -1;
    }

    vector<Connection_Point> connection_points = element->get_connection_points();

    const int CLICK_RADIUS = 20;

    for (int i = 0; i < connection_points.size(); ++i)
    {
        const SDL_Point& point_pos = connection_points[i].pos;

        // a small rectangle around the connection point
        SDL_Rect target_rect = {
                point_pos.x - CLICK_RADIUS,
                point_pos.y - CLICK_RADIUS,
                CLICK_RADIUS * 2,
                CLICK_RADIUS * 2
        };

        if (SDL_PointInRect(&click_pos, &target_rect))
        {
            return i;
        }
    }

    return -1;
}

vector<pair<double, double>> generate_data_for_element_V(Graphical_Element* element, Controller* C)
{
    vector<pair<double, double>> data_points;
    double start_time, stop_time, time_step;
    C->get_tran_params(start_time, stop_time, time_step);

    double voltage;
    if (time_step > 0)
    {
        for (double time = start_time; time < stop_time - time_step; time += time_step)
        {
            voltage = element->get_model()->get_voltage_at_time(time);
            data_points.push_back({voltage, time});
        }
    }
    return data_points;
}

vector<pair<double, double>> generate_data_for_element_I(Graphical_Element* element, Controller* C)
{
    vector<pair<double, double>> data_points;
    double start_time, stop_time, time_step;
    C->get_tran_params(start_time, stop_time, time_step);

    double current;
    if (time_step > 0)
    {
        for (double time = start_time; time < stop_time - time_step; time += time_step)
        {
            current = element->get_model()->get_current(time, time_step);
            data_points.push_back({current, time});
        }
    }
    return data_points;
}

vector<pair<double, double>> generate_data_for_element_P(Graphical_Element* element, Controller* C)
{
    vector<pair<double, double>> data_points;
    double start_time, stop_time, time_step;
    C->get_tran_params(start_time, stop_time, time_step);

    double voltage, current, power;
    if (time_step > 0) {
        for (double time = start_time; time < stop_time - time_step; time += time_step)
        {
            voltage = element->get_model()->get_voltage_at_time(time);
            current = element->get_model()->get_current(time, time_step);
            power = current * voltage;
            data_points.push_back({power, time});
        }
    }
    return data_points;
}

void graphical_view::draw_grid(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    for (int x = 0; x < m_window_width; x += GRID_SIZE)
    {
        SDL_RenderDrawLine(renderer, x, 0, x, m_window_height);
    }
    for (int y = 0; y < m_window_height; y += GRID_SIZE)
    {
        SDL_RenderDrawLine(renderer, 0, y, m_window_width, y);
    }
}

void graphical_view::initialize_menu()
{
    menu_items.clear();
    menu_items.push_back({"Resistor", Element_Type::Resistor});
    menu_items.push_back({"Capacitor", Element_Type::Capacitor});
    menu_items.push_back({"Inductor", Element_Type::Inductor});
    menu_items.push_back({"Current Source", Element_Type::Current_Source});
    menu_items.push_back({"Diode", Element_Type::Real_Diode});
    menu_items.push_back({"Zener Diode", Element_Type::Zener_Diode});
    menu_items.push_back({"DC Source", Element_Type::Voltage_Source, "DC"});
    menu_items.push_back({"SIN Source", Element_Type::Voltage_Source, "SIN"});
    menu_items.push_back({"Pulse Source", Element_Type::Voltage_Source, "Pulse"});
    menu_items.push_back({"Delta_Dirac Source", Element_Type::Voltage_Source, "Delta"});
    menu_items.push_back({"Square Source", Element_Type::Voltage_Source, "Square"});
    menu_items.push_back({"Triangular Source", Element_Type::Voltage_Source, "Triangular"});
    menu_items.push_back({"Waveform Source", Element_Type::Voltage_Source, "Waveform"});
    menu_items.push_back({"AC Source", Element_Type::Voltage_Source, "AC"});
    menu_items.push_back({"VCVS", Element_Type::VC_Voltage_Source});
    menu_items.push_back({"VCCS", Element_Type::VC_Current_Source});
    menu_items.push_back({"CCVS", Element_Type::CC_Voltage_Source});
    menu_items.push_back({"CCCS", Element_Type::CC_Current_source});
}

void graphical_view::initialize_SubC_menu(Controller* C)
{
    SubC_menu_items.clear();
    for (const auto& sub_c : C->subCircuits)
        SubC_menu_items.push_back({sub_c->get_name()});
}

void graphical_view::draw_component_menu(SDL_Renderer* renderer, TTF_Font* font)
{
    const SDL_Color SELECTED = {80, 88, 99, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color HOVERED = {60, 68, 80, 255};

    SDL_Rect menu_panel = {200, 80, 800, 600};
    SDL_Rect preview_panel = {menu_panel.x + 550, menu_panel.y + 50, 200, 200};

    // panels
    SDL_SetRenderDrawColor(renderer, 50, 58, 69, 255);
    SDL_RenderFillRect(renderer, &menu_panel);
    SDL_SetRenderDrawColor(renderer, 236, 239, 244, 255);
    SDL_RenderFillRect(renderer, &preview_panel);

    // items
    int start_y = menu_panel.y + 60;
    render_text(renderer, font, "Components:", menu_panel.x + 20, menu_panel.y + 30, TEXT_COLOR);
    for (int i = 0; i < menu_items.size(); ++i)
    {
        if (i == selected_menu_item_index)
        {
            SDL_SetRenderDrawColor(renderer, SELECTED.r, SELECTED.g, SELECTED.b, SELECTED.a);
            SDL_RenderFillRect(renderer, &menu_items[i].rect);
        }
        else if (i == hovered_menu_item_index)
        {
            SDL_SetRenderDrawColor(renderer, HOVERED.r, HOVERED.g, HOVERED.b, HOVERED.a);
            SDL_RenderFillRect(renderer, &menu_items[i].rect);
        }
        menu_items[i].rect = {menu_panel.x + 20, start_y + (i * 30), 200, 25};
        render_text(renderer, font, menu_items[i].name, menu_items[i].rect.x, menu_items[i].rect.y, TEXT_COLOR);
    }

    int column2_x = menu_panel.x + 250;
    render_text(renderer, font, "Sub_Circuits:", column2_x, menu_panel.y + 30, TEXT_COLOR);
    for (int i = 0; i < SubC_menu_items.size(); ++i)
    {
        if (i == selected_SubC_menu_item_index)
        {
            SDL_SetRenderDrawColor(renderer, SELECTED.r, SELECTED.g, SELECTED.b, SELECTED.a);
            SDL_RenderFillRect(renderer, &SubC_menu_items[i].rect);
        }
        else if (i == hovered_SubC_item_index)
        {
            SDL_SetRenderDrawColor(renderer, HOVERED.r, HOVERED.g, HOVERED.b, HOVERED.a);
            SDL_RenderFillRect(renderer, &SubC_menu_items[i].rect);
        }
        SubC_menu_items[i].rect = {column2_x, start_y + (i * 30), 200, 25};
        render_text(renderer, font, SubC_menu_items[i].name, SubC_menu_items[i].rect.x + 5, SubC_menu_items[i].rect.y + 3, TEXT_COLOR);
    }

    // preview
    if (selected_menu_item_index != -1)
    {
        switch (menu_items[selected_menu_item_index].type)
        {
            case Element_Type::Resistor:
            {
                Graphical_Resistor preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Capacitor:
            {
                Graphical_Capacitor preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Inductor:
            {
                Graphical_Inductor preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Current_Source:
            {
                Graphical_Current_Source preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Real_Diode:
            {
                Graphical_Real_Diode preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Zener_Diode:
            {
                Graphical_Zener_Diode preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::Voltage_Source:
            {
                Graphical_Voltage_Source preview(nullptr);
                if (menu_items[selected_menu_item_index].subtype_tag == "AC")
                {
                    preview.make_AC();
                }
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::VC_Voltage_Source:
            {
                Graphical_VCVS preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::VC_Current_Source:
            {
                Graphical_VCCS preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::CC_Voltage_Source:
            {
                Graphical_CCVS preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
            case Element_Type::CC_Current_source:
            {
                Graphical_CCCS preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer, false);
                break;
            }
        }
    }
    else if (selected_SubC_menu_item_index != -1)
    {
        Graphical_SubCircuit preview(nullptr, "");
        preview.bounding_box = preview_panel;
        preview.draw(renderer, false);
    }
}

void graphical_view::draw_properties_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color TEXT_BOX_BG = {33, 37, 41, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color HIGHLIGHT_COLOR = {52, 152, 219, 255};

    int menu_width = 400;
    int menu_height = 420;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);

    auto& element = C->get_graphical_elements()[edited_element_index];
    vector<Editable_Property> props = element->get_editable_properties();

    property_rects.clear();

    int start_y = menu_panel.y + 50;
    int row_height = 40;
    for (int i = 0; i < props.size(); ++i) {
        render_text(renderer, font, props[i].label, menu_panel.x + 20, start_y + (i * row_height), TEXT_COLOR);

        SDL_Rect textbox_rect = {menu_panel.x + 180, start_y + (i * row_height) - 5, 200, 30};
        property_rects.push_back(textbox_rect);

        SDL_SetRenderDrawColor(renderer, TEXT_BOX_BG.r, TEXT_BOX_BG.g, TEXT_BOX_BG.b, TEXT_BOX_BG.a);
        SDL_RenderFillRect(renderer, &textbox_rect);

        if (i == active_edit_box) {
            SDL_SetRenderDrawColor(renderer, HIGHLIGHT_COLOR.r, HIGHLIGHT_COLOR.g, HIGHLIGHT_COLOR.b, HIGHLIGHT_COLOR.a);
        } else {
            SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
        }
        SDL_RenderDrawRect(renderer, &textbox_rect);

        if (i < edit_buffers.size()) {
            render_text(renderer, font, edit_buffers[i], textbox_rect.x + 5, textbox_rect.y + 5, TEXT_COLOR);
        }
    }

    ok_button_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 100, 30};
    cancel_button_rect = {menu_panel.x + menu_width - 110, menu_panel.y + menu_height - 50, 100, 30};

    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderFillRect(renderer, &ok_button_rect);
    SDL_RenderFillRect(renderer, &cancel_button_rect);

    render_text(renderer, font, "OK", ok_button_rect.x + 40, ok_button_rect.y + 5, TEXT_COLOR);
    render_text(renderer, font, "Cancel", cancel_button_rect.x + 25, cancel_button_rect.y + 5, TEXT_COLOR);
}

void graphical_view::initialize_toolbar(TTF_Font* font)
{
    toolbar_buttons.clear();

    const vector<pair<string, Tool_Bar_Action>> button_data = {
            {"New", Tool_Bar_Action::New_File},
            {"Grid", Tool_Bar_Action::Grid},
            {"Delete", Tool_Bar_Action::Delete},
            {"Wire", Tool_Bar_Action::Wire},
            {"Net Label", Tool_Bar_Action::Net_Label},
            {"Components", Tool_Bar_Action::Components_Menu},
            {"Sub_Circuits", Tool_Bar_Action::Sub_Circuit},
            {"File", Tool_Bar_Action::File},
            {"Analysis", Tool_Bar_Action::Configure_Analysis},
            {"Run", Tool_Bar_Action::Run},
            {"Probe", Tool_Bar_Action::Probe},
            {"M_Operation", Tool_Bar_Action::Math_Operation},
            {"Save", Tool_Bar_Action::Save}
    };

    int current_x = 10;
    const int button_height = 30;
    const int y_pos = 5;
    const int internal_padding = 15;
    const int external_padding = 10;

    for (const auto& data : button_data)
    {
        int text_width, text_height;
        TTF_SizeText(font, data.first.c_str(), &text_width, &text_height);

        int button_width = text_width + (2 * internal_padding);

        toolbar_buttons.push_back({
            {current_x, y_pos, button_width, button_height},
            data.first,
            data.second
        });

        current_x += button_width + external_padding;
    }
}

void graphical_view::draw_toolbar(SDL_Renderer* renderer, TTF_Font* font)
{
    const int TOOLBAR_HEIGHT = 40;
    const SDL_Color TOOLBAR_BG = {240, 240, 240, 255};
    const SDL_Color BUTTON_BG = {200, 200, 200, 255};
    const SDL_Color TEXT_COLOR = {0, 0, 0, 255};

    SDL_Rect toolbar_rect = {0, 0, m_window_width, TOOLBAR_HEIGHT};
    SDL_SetRenderDrawColor(renderer, TOOLBAR_BG.r, TOOLBAR_BG.g, TOOLBAR_BG.b, TOOLBAR_BG.a);
    SDL_RenderFillRect(renderer, &toolbar_rect);

    for (int i = 0; i < toolbar_buttons.size(); ++i)
    {
        if (i == hovered_button_index)
        {
            SDL_SetRenderDrawColor(renderer, BUTTON_BG.r, BUTTON_BG.g, BUTTON_BG.b, BUTTON_BG.a);
            SDL_RenderFillRect(renderer, &toolbar_buttons[i].rect);
        }

        int text_width, text_height;
        TTF_SizeText(font, toolbar_buttons[i].text_label.c_str(), &text_width, &text_height);

        int text_x = toolbar_buttons[i].rect.x + (toolbar_buttons[i].rect.w - text_width) / 2;
        int text_y = toolbar_buttons[i].rect.y + (toolbar_buttons[i].rect.h - text_height) / 2;

        render_text(renderer, font, toolbar_buttons[i].text_label, text_x, text_y, TEXT_COLOR);
    }
}

void graphical_view::initialize_configure_analysis_menu(Controller* C)
{
    edit_buffers.clear();
    active_edit_box = -1;

    switch (current_analysis_mode)
    {
        case Analysis_Mode::Transient:
        {
            double start, stop, step;
            C->get_tran_params(start, stop, step);
            edit_buffers.push_back(std::to_string(start));
            edit_buffers.push_back(std::to_string(stop));
            edit_buffers.push_back(std::to_string(step));
        }
            break;
        case Analysis_Mode::AC_Sweep:
        {
            double start, stop, step;
            AC_Sweep_Type type;
            C->get_ac_params(start, stop, step, type);
            C->get_ac_params(start, stop, step, type);
            this->AC_sweep_type = type;
            cout << "INITIALIZING MENU: AC_sweep_type set to " << (int)this->AC_sweep_type << endl;
            this->AC_sweep_type = type;
            edit_buffers.push_back(std::to_string(start));
            edit_buffers.push_back(std::to_string(stop));
            edit_buffers.push_back(std::to_string(step));
        }
            break;
        case Analysis_Mode::Phase_Sweep:
            edit_buffers.resize(4, "");
            break;
    }
}

void graphical_view::draw_configure_analysis(SDL_Renderer *renderer, TTF_Font *font, Controller* C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color TEXT_BOX_BG = {33, 37, 41, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color HIGHLIGHT_COLOR = {52, 152, 219, 255};
    const SDL_Color TAB_BG = {40, 45, 53, 255};

    int menu_width = 450;
    int menu_height = 300;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);

    // main tab buttons
    int tab_width = menu_width / 3;
    transient_tab_rect = {menu_panel.x, menu_panel.y, tab_width, 40};
    ac_sweep_tab_rect = {menu_panel.x + tab_width, menu_panel.y, tab_width, 40};
    phase_sweep_tab_rect = {menu_panel.x + (2 * tab_width), menu_panel.y, tab_width, 40};

    // transient tab
    SDL_SetRenderDrawColor(renderer, (current_analysis_mode == Analysis_Mode::Transient) ? HIGHLIGHT_COLOR.r : TAB_BG.r, (current_analysis_mode == Analysis_Mode::Transient) ? HIGHLIGHT_COLOR.g : TAB_BG.g, (current_analysis_mode == Analysis_Mode::Transient) ? HIGHLIGHT_COLOR.b : TAB_BG.b, 255);
    SDL_RenderFillRect(renderer, &transient_tab_rect);
    render_text(renderer, font, "Transient", transient_tab_rect.x + 25, transient_tab_rect.y + 10, TEXT_COLOR);

    // AC sweep tab
    SDL_SetRenderDrawColor(renderer, (current_analysis_mode == Analysis_Mode::AC_Sweep) ? HIGHLIGHT_COLOR.r : TAB_BG.r, (current_analysis_mode == Analysis_Mode::AC_Sweep) ? HIGHLIGHT_COLOR.g : TAB_BG.g, (current_analysis_mode == Analysis_Mode::AC_Sweep) ? HIGHLIGHT_COLOR.b : TAB_BG.b, 255);
    SDL_RenderFillRect(renderer, &ac_sweep_tab_rect);
    render_text(renderer, font, "AC Sweep", ac_sweep_tab_rect.x + 30, ac_sweep_tab_rect.y + 10, TEXT_COLOR);

    // phase sweep tab
    SDL_SetRenderDrawColor(renderer, (current_analysis_mode == Analysis_Mode::Phase_Sweep) ? HIGHLIGHT_COLOR.r : TAB_BG.r, (current_analysis_mode == Analysis_Mode::Phase_Sweep) ? HIGHLIGHT_COLOR.g : TAB_BG.g, (current_analysis_mode == Analysis_Mode::Phase_Sweep) ? HIGHLIGHT_COLOR.b : TAB_BG.b, 255);
    SDL_RenderFillRect(renderer, &phase_sweep_tab_rect);
    render_text(renderer, font, "Phase Sweep", phase_sweep_tab_rect.x + 20, phase_sweep_tab_rect.y + 10, TEXT_COLOR);

    // options
    vector<string> labels;
    property_rects.clear();
    int start_y = menu_panel.y + 60;
    int row_height = 40;

    switch (current_analysis_mode)
    {
        case Analysis_Mode::Transient:
            labels = {"Start Time", "Stop Time", "Max Time Step"};
            break;
        case Analysis_Mode::AC_Sweep:
            labels = {"Start Frequency", "Stop Frequency", "Number of Points"};
            // sweep type selection buttons
            render_text(renderer, font, "Type of Sweep:", menu_panel.x + 20, start_y, TEXT_COLOR);
            octave_button_rect = {menu_panel.x + 180, start_y, 80, 30};
            decade_button_rect = {menu_panel.x + 270, start_y, 80, 30};
            linear_button_rect = {menu_panel.x + 360, start_y, 80, 30};

            SDL_SetRenderDrawColor(renderer, (AC_sweep_type == AC_Sweep_Type::Octave) ? HIGHLIGHT_COLOR.r : BORDER_COLOR.r, (AC_sweep_type == AC_Sweep_Type::Octave) ? HIGHLIGHT_COLOR.g : BORDER_COLOR.g, (AC_sweep_type == AC_Sweep_Type::Octave) ? HIGHLIGHT_COLOR.b : BORDER_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &octave_button_rect);
            render_text(renderer, font, "Octave", octave_button_rect.x + 15, octave_button_rect.y + 5, TEXT_COLOR);

            SDL_SetRenderDrawColor(renderer, (AC_sweep_type == AC_Sweep_Type::Decade) ? HIGHLIGHT_COLOR.r : BORDER_COLOR.r, (AC_sweep_type == AC_Sweep_Type::Decade) ? HIGHLIGHT_COLOR.g : BORDER_COLOR.g, (AC_sweep_type == AC_Sweep_Type::Decade) ? HIGHLIGHT_COLOR.b : BORDER_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &decade_button_rect);
            render_text(renderer, font, "Decade", decade_button_rect.x + 15, decade_button_rect.y + 5, TEXT_COLOR);

            SDL_SetRenderDrawColor(renderer, (AC_sweep_type == AC_Sweep_Type::Linear) ? HIGHLIGHT_COLOR.r : BORDER_COLOR.r, (AC_sweep_type == AC_Sweep_Type::Linear) ? HIGHLIGHT_COLOR.g : BORDER_COLOR.g, (AC_sweep_type == AC_Sweep_Type::Linear) ? HIGHLIGHT_COLOR.b : BORDER_COLOR.b, 255);
            SDL_RenderFillRect(renderer, &linear_button_rect);
            render_text(renderer, font, "Linear", linear_button_rect.x + 15, linear_button_rect.y + 5, TEXT_COLOR);

            start_y += row_height;
            break;
        case Analysis_Mode::Phase_Sweep:
            labels = {"Base Frequency", "Start Phase", "Stop Phase", "Number of Points"};
            break;
    }

    // text input boxes
    for (int i = 0; i < labels.size(); ++i)
    {
        render_text(renderer, font, labels[i], menu_panel.x + 20, start_y + (i * row_height), TEXT_COLOR);
        SDL_Rect textbox_rect = {menu_panel.x + 180, start_y + (i * row_height) - 5, 200, 30};
        property_rects.push_back(textbox_rect);

        SDL_SetRenderDrawColor(renderer, TEXT_BOX_BG.r, TEXT_BOX_BG.g, TEXT_BOX_BG.b, TEXT_BOX_BG.a);
        SDL_RenderFillRect(renderer, &textbox_rect);
        SDL_SetRenderDrawColor(renderer, (i == active_edit_box) ? HIGHLIGHT_COLOR.r : BORDER_COLOR.r, (i == active_edit_box) ? HIGHLIGHT_COLOR.g : BORDER_COLOR.g, (i == active_edit_box) ? HIGHLIGHT_COLOR.b : BORDER_COLOR.b, 255);
        SDL_RenderDrawRect(renderer, &textbox_rect);

        if (i < edit_buffers.size())
        {
            render_text(renderer, font, edit_buffers[i], textbox_rect.x + 5, textbox_rect.y + 5, TEXT_COLOR);
        }
    }

    // OK cancel
    ok_button_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 100, 30};
    cancel_button_rect = {menu_panel.x + menu_width - 110, menu_panel.y + menu_height - 50, 100, 30};
    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderFillRect(renderer, &ok_button_rect);
    SDL_RenderFillRect(renderer, &cancel_button_rect);
    render_text(renderer, font, "OK", ok_button_rect.x + 40, ok_button_rect.y + 5, TEXT_COLOR);
    render_text(renderer, font, "Cancel", cancel_button_rect.x + 25, cancel_button_rect.y + 5, TEXT_COLOR);
}

void graphical_view::draw_math_operation_menu(SDL_Renderer *renderer, TTF_Font *font, Controller *C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255}, TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color DISPLAY_BG = {33, 37, 41, 255}, BUTTON_BG = {80, 88, 99, 255};
    const SDL_Color SELECTED_BG = {52, 152, 219, 255}, DISABLED_BG = {60, 68, 80, 255};

    int menu_width = 830;
    int menu_height = 700;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);

    // expression display box
    SDL_Rect display_rect = {menu_panel.x + 10, menu_panel.y + 10, menu_width - 20, 40};
    SDL_SetRenderDrawColor(renderer, DISPLAY_BG.r, DISPLAY_BG.g, DISPLAY_BG.b, DISPLAY_BG.a);
    SDL_RenderFillRect(renderer, &display_rect);
    render_text(renderer, font, math_expression_string, display_rect.x + 5, display_rect.y + 10, TEXT_COLOR);

    int builder_y = menu_panel.y + 60;
    render_text(renderer, font, "Build a Term:", menu_panel.x + 20, builder_y, TEXT_COLOR);

    // constant input box
    render_text(renderer, font, "Constant (K):", menu_panel.x + 20, builder_y + 30, TEXT_COLOR);
    constant_textbox_rect = {menu_panel.x + 150, builder_y + 25, 100, 30};
    SDL_SetRenderDrawColor(renderer, DISPLAY_BG.r, DISPLAY_BG.g, DISPLAY_BG.b, DISPLAY_BG.a);
    SDL_RenderFillRect(renderer, &constant_textbox_rect);
    SDL_SetRenderDrawColor(renderer, is_editing_constant ? SELECTED_BG.r : BUTTON_BG.r, is_editing_constant ? SELECTED_BG.g : BUTTON_BG.g, is_editing_constant ? SELECTED_BG.b : BUTTON_BG.b, 255);
    SDL_RenderDrawRect(renderer, &constant_textbox_rect);
    render_text(renderer, font, math_constant_buffer, constant_textbox_rect.x + 5, constant_textbox_rect.y + 5, TEXT_COLOR);

    render_text(renderer, font, "Signal Type:", menu_panel.x + 280, builder_y + 30, TEXT_COLOR);
    if (current_analysis_mode == Analysis_Mode::Transient)
    {
        // type of signal (VIP!)
        v_button_rect = {menu_panel.x + 380, builder_y + 25, 30, 30};
        i_button_rect = {menu_panel.x + 415, builder_y + 25, 30, 30};
        p_button_rect = {menu_panel.x + 450, builder_y + 25, 30, 30};

        // voltage button
        SDL_SetRenderDrawColor(renderer,
                               (math_selected_signal_type == Signal_Type::Voltage) ? SELECTED_BG.r : BUTTON_BG.r,
                               (math_selected_signal_type == Signal_Type::Voltage) ? SELECTED_BG.g : BUTTON_BG.g,
                               (math_selected_signal_type == Signal_Type::Voltage) ? SELECTED_BG.b : BUTTON_BG.b,
                               255
        );
        SDL_RenderFillRect(renderer, &v_button_rect);
        render_text(renderer, font, "V", v_button_rect.x + 10, v_button_rect.y + 5);

        // current button
        SDL_SetRenderDrawColor(renderer,
                               (math_selected_signal_type == Signal_Type::Current) ? SELECTED_BG.r : BUTTON_BG.r,
                               (math_selected_signal_type == Signal_Type::Current) ? SELECTED_BG.g : BUTTON_BG.g,
                               (math_selected_signal_type == Signal_Type::Current) ? SELECTED_BG.b : BUTTON_BG.b,
                               255
        );
        SDL_RenderFillRect(renderer, &i_button_rect);
        render_text(renderer, font, "I", i_button_rect.x + 10, i_button_rect.y + 5);

        // power button
        SDL_SetRenderDrawColor(renderer,
                               (math_selected_signal_type == Signal_Type::Power) ? SELECTED_BG.r : BUTTON_BG.r,
                               (math_selected_signal_type == Signal_Type::Power) ? SELECTED_BG.g : BUTTON_BG.g,
                               (math_selected_signal_type == Signal_Type::Power) ? SELECTED_BG.b : BUTTON_BG.b,
                               255
        );
        SDL_RenderFillRect(renderer, &p_button_rect);
        render_text(renderer, font, "P", p_button_rect.x + 10, p_button_rect.y + 5);
    }
    else if (current_analysis_mode == Analysis_Mode::AC_Sweep || current_analysis_mode == Analysis_Mode::Phase_Sweep)
    {
        // type of signal (MP!)
        mag_button_rect = {menu_panel.x + 380, builder_y + 25, 60, 30};
        phase_button_rect = {menu_panel.x + 445, builder_y + 25, 60, 30};

        // mag button
        SDL_SetRenderDrawColor(renderer,
                               (math_selected_signal_type == Signal_Type::Mag) ? SELECTED_BG.r : BUTTON_BG.r,
                               (math_selected_signal_type == Signal_Type::Mag) ? SELECTED_BG.g : BUTTON_BG.g,
                               (math_selected_signal_type == Signal_Type::Mag) ? SELECTED_BG.b : BUTTON_BG.b,
                               255
        );
        SDL_RenderFillRect(renderer, &mag_button_rect);
        render_text(renderer, font, "Mag", mag_button_rect.x + 15, mag_button_rect.y + 5);

        // phase button
        SDL_SetRenderDrawColor(renderer,
                               (math_selected_signal_type == Signal_Type::Phase) ? SELECTED_BG.r : BUTTON_BG.r,
                               (math_selected_signal_type == Signal_Type::Phase) ? SELECTED_BG.g : BUTTON_BG.g,
                               (math_selected_signal_type == Signal_Type::Phase) ? SELECTED_BG.b : BUTTON_BG.b,
                               255
        );
        SDL_RenderFillRect(renderer, &phase_button_rect);
        render_text(renderer, font, "Phase", phase_button_rect.x + 10, phase_button_rect.y + 5);
    }

    // list of elements
    render_text(renderer, font, "Element:", menu_panel.x + 20, builder_y + 70, TEXT_COLOR);
    math_element_buttons.clear();
    int list_y = builder_y + 95;
    for (int i = 0; i < C->get_graphical_elements().size(); ++i)
    {
        const auto& element = C->get_graphical_elements()[i];
        if (dynamic_cast<Graphical_Ground*>(element.get()) || dynamic_cast<Graphical_SubCircuit*>(element.get()) || dynamic_cast<Graphical_Net_Label*>(element.get())) continue;

        SDL_Rect item_rect = {menu_panel.x + 20, list_y + (i * 35), 230, 30};
        math_element_buttons.push_back(item_rect);

        SDL_SetRenderDrawColor(renderer, (i == math_selected_element_index) ? SELECTED_BG.r : BUTTON_BG.r, (i == math_selected_element_index) ? SELECTED_BG.g : BUTTON_BG.g, (i == math_selected_element_index) ? SELECTED_BG.b : BUTTON_BG.b, 255);
        SDL_RenderFillRect(renderer, &item_rect);
        render_text(renderer, font, element->get_model()->get_name(), item_rect.x + 5, item_rect.y + 5, TEXT_COLOR);
    }

    // nodes
    int column2_x = menu_panel.x + 270;
    render_text(renderer, font, "Nodes:", column2_x, builder_y + 70, TEXT_COLOR);
    math_node_buttons.clear();
    const auto& nodes = C->circuit->get_Nodes();
    for (int i = 0; i < nodes.size(); ++i) {
        const auto& node = nodes[i];
        if (node->is_the_node_ground()) continue;

        SDL_Rect item_rect = {column2_x, list_y + (i * 35), 230, 30};
        math_node_buttons.push_back(item_rect);

        bool is_selected = (i == math_selected_node_index);
        SDL_SetRenderDrawColor(renderer, is_selected ? SELECTED_BG.r : BUTTON_BG.r, is_selected ? SELECTED_BG.g : BUTTON_BG.g, is_selected ? SELECTED_BG.b : BUTTON_BG.b, 255);
        SDL_RenderFillRect(renderer, &item_rect);
        render_text(renderer, font, node->get_name(), item_rect.x + 5, item_rect.y + 5, TEXT_COLOR);
    }

    // add and subtract buttons
    int add_term_y = menu_panel.y + 75;
    op_plus_button = {menu_panel.x + 520, add_term_y, 140, 40};
    op_minus_button = {menu_panel.x + 670, add_term_y, 140, 40};
    bool term_ready = (math_selected_element_index != -1 || math_selected_node_index != -1);
    SDL_SetRenderDrawColor(renderer, term_ready ? BUTTON_BG.r : DISABLED_BG.r, term_ready ? BUTTON_BG.g : DISABLED_BG.g, term_ready ? BUTTON_BG.b : DISABLED_BG.b, 255);
    SDL_RenderFillRect(renderer, &op_plus_button);
    SDL_RenderFillRect(renderer, &op_minus_button);
    render_text(renderer, font, "Add Term (+)", op_plus_button.x + 10, op_plus_button.y + 10, TEXT_COLOR);
    render_text(renderer, font, "Subtract Term (-)", op_minus_button.x + 5, op_minus_button.y + 10, TEXT_COLOR);

    // action buttons
    int bottom_y = menu_panel.y + menu_height - 50;
    op_clear_button = {menu_panel.x + menu_width - 220, bottom_y, 100, 40};
    op_execute_button = {menu_panel.x + menu_width - 110, bottom_y, 100, 40};
    bool can_execute = !math_terms.empty();
    SDL_SetRenderDrawColor(renderer, BUTTON_BG.r, BUTTON_BG.g, BUTTON_BG.b, BUTTON_BG.a);
    SDL_RenderFillRect(renderer, &op_clear_button);
    SDL_SetRenderDrawColor(renderer, can_execute ? BUTTON_BG.r : DISABLED_BG.r, can_execute ? BUTTON_BG.g : DISABLED_BG.g, can_execute ? BUTTON_BG.b : DISABLED_BG.b, 255);
    SDL_RenderFillRect(renderer, &op_execute_button);
    render_text(renderer, font, "Clear", op_clear_button.x + 25, op_clear_button.y + 10, TEXT_COLOR);
    render_text(renderer, font, "Execute", op_execute_button.x + 15, op_execute_button.y + 10, TEXT_COLOR);
}

void graphical_view::add_math_term(bool is_subtraction, Controller* C)
{
    if (math_selected_element_index == -1 && math_selected_node_index == -1) return;

    // constant
    double k = 1.0;
    try
    {
        k = toValue(math_constant_buffer);
    }
    catch (const exception& e)
    {
        cerr << "Error: Invalid value" << e.what() << endl;
        return;
    }
    if (is_subtraction) { k *= -1.0; }

    Signal new_term;
    vector<pair<double, double>> data;
    string name;
    string prefix;

    if (math_selected_element_index != -1)
    {
        auto& elements = C->get_graphical_elements();
        if (math_selected_element_index >= elements.size() || elements[math_selected_element_index]->get_model() == nullptr) return;
        Graphical_Element* selected_element = elements[math_selected_element_index].get();
        name = selected_element->get_model()->get_name();

        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            switch (math_selected_signal_type)
            {
                case Signal_Type::Voltage: prefix = "V"; data = generate_data_for_element_V(selected_element, C); break;
                case Signal_Type::Current: prefix = "I"; data = generate_data_for_element_I(selected_element, C); break;
                case Signal_Type::Power:   prefix = "P"; data = generate_data_for_element_P(selected_element, C); break;
            }
        }

        else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            switch (math_selected_signal_type)
            {
                case Signal_Type::Mag:
                {
                    prefix = "Mag";
                    pair<Node*, Node*> nodes = selected_element->get_model()->get_nodes();
                    vector<double> freq1, mag1, freq2, mag2;
                    for (const auto& ac_data : C->circuit->getAcVoltage())
                    {
                        if (nodes.first == ac_data.first)
                        {
                            freq1 = get<0>(ac_data.second);
                            mag1 = get<1>(ac_data.second);
                        }
                        else if (nodes.second == ac_data.first)
                        {
                            freq2 = get<0>(ac_data.second);
                            mag2 = get<1>(ac_data.second);
                        }
                    }

                    for (size_t i = 0; i < min(min(freq1.size(), mag1.size()), min(freq2.size(), mag2.size())); ++i)
                    {
                        data.push_back({mag1[i] - mag2[i], freq1[i]});
                    }
                    break;
                }
                case Signal_Type::Phase:
                {
                    prefix = "Phase";
                    pair<Node*, Node*> nodes = selected_element->get_model()->get_nodes();
                    vector<double> freq1, mag1, freq2, mag2;
                    for (const auto& ac_data : C->circuit->getAcVoltage())
                    {
                        if (nodes.first == ac_data.first)
                        {
                            freq1 = get<0>(ac_data.second);
                            mag1 = get<2>(ac_data.second);
                        }
                        else if (nodes.second == ac_data.first)
                        {
                            freq2 = get<0>(ac_data.second);
                            mag2 = get<2>(ac_data.second);
                        }
                    }

                    for (size_t i = 0; i < min(min(freq1.size(), mag1.size()), min(freq2.size(), mag2.size())); ++i)
                    {
                        data.push_back({mag1[i] - mag2[i], freq1[i]});
                    }
                    break;
                }
            }
        }

        else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            switch (math_selected_signal_type)
            {
                case Signal_Type::Mag:
                {
                    prefix = "Mag";
                    pair<Node*, Node*> nodes = selected_element->get_model()->get_nodes();
                    vector<double> freq1, mag1, freq2, mag2;
                    for (const auto& ac_data : C->circuit->getPhaseVoltage())
                    {
                        if (nodes.first == ac_data.first)
                        {
                            freq1 = get<0>(ac_data.second);
                            mag1 = get<1>(ac_data.second);
                        }
                        else if (nodes.second == ac_data.first)
                        {
                            freq2 = get<0>(ac_data.second);
                            mag2 = get<1>(ac_data.second);
                        }
                    }

                    for (size_t i = 0; i < min(min(freq1.size(), mag1.size()), min(freq2.size(), mag2.size())); ++i)
                    {
                        data.push_back({mag1[i] - mag2[i], freq1[i]});
                    }
                    break;
                }
                case Signal_Type::Phase:
                {
                    prefix = "Phase";
                    pair<Node*, Node*> nodes = selected_element->get_model()->get_nodes();
                    vector<double> freq1, mag1, freq2, mag2;
                    for (const auto& ac_data : C->circuit->getPhaseVoltage())
                    {
                        if (nodes.first == ac_data.first)
                        {
                            freq1 = get<0>(ac_data.second);
                            mag1 = get<2>(ac_data.second);
                        }
                        else if (nodes.second == ac_data.first)
                        {
                            freq2 = get<0>(ac_data.second);
                            mag2 = get<2>(ac_data.second);
                        }
                    }

                    for (size_t i = 0; i < min(min(freq1.size(), mag1.size()), min(freq2.size(), mag2.size())); ++i)
                    {
                        data.push_back({mag1[i] - mag2[i], freq1[i]});
                    }
                    break;
                }
            }
        }
    }
    else if (math_selected_node_index != -1)
    {
        k *= -1;
        const auto& nodes = C->circuit->get_Nodes();
        if (math_selected_node_index >= nodes.size()) return;
        Node* selected_node = nodes[math_selected_node_index];

        name = selected_node->get_name();
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            prefix = "V";
            data = selected_node->get_all_voltages();
        }

        else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            switch (math_selected_signal_type)
            {
                case Signal_Type::Mag:
                {
                    prefix = "Mag";
                    vector<double> freq, mag;
                    for (const auto& ac_data : C->circuit->getAcVoltage())
                    {
                        if (selected_node == ac_data.first)
                        {
                            freq = get<0>(ac_data.second);
                            mag = get<1>(ac_data.second);
                            break;
                        }
                    }

                    for (size_t i = 0; i < std::min(freq.size(), mag.size()); ++i)
                    {
                        data.push_back({mag[i], freq[i]});
                    }
                    break;
                }
                case Signal_Type::Phase:
                {
                    prefix = "Phase";
                    vector<double> freq, phase;
                    for (const auto& ac_data : C->circuit->getAcVoltage())
                    {
                        if (selected_node == ac_data.first)
                        {
                            freq = get<0>(ac_data.second);
                            phase = get<2>(ac_data.second);
                            break;
                        }
                    }

                    for (size_t i = 0; i < min(freq.size(), phase.size()); ++i)
                    {
                        data.push_back({phase[i], freq[i]});
                    }
                    break;
                }
            }
        }

        else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            switch (math_selected_signal_type)
            {
                case Signal_Type::Mag:
                {
                    prefix = "Mag";
                    vector<double> freq, mag;
                    for (const auto& ac_data : C->circuit->getPhaseVoltage())
                    {
                        if (selected_node == ac_data.first)
                        {
                            freq = get<0>(ac_data.second);
                            mag = get<1>(ac_data.second);
                            break;
                        }
                    }

                    for (size_t i = 0; i < min(freq.size(), mag.size()); ++i)
                    {
                        data.push_back({mag[i], freq[i]});
                    }
                    break;
                }
                case Signal_Type::Phase:
                {
                    prefix = "Phase";
                    vector<double> freq, phase;
                    for (const auto& ac_data : C->circuit->getPhaseVoltage())
                    {
                        if (selected_node == ac_data.first)
                        {
                            freq = get<0>(ac_data.second);
                            phase = get<2>(ac_data.second);
                            break;
                        }
                    }

                    for (size_t i = 0; i < min(freq.size(), phase.size()); ++i)
                    {
                        data.push_back({phase[i], freq[i]});
                    }
                    break;
                }
            }
        }
    }

    for (const auto& point : data)
    {
        double x_val = point.first;  // x, y
        double y_val = point.second;
        //new_term.data_points.push_back({x_val, y_val * k});
        new_term.data_points.push_back({point.first * k, point.second});
    }
    math_terms.push_back(new_term);

    if (!math_expression_string.empty())
    {
        math_expression_string += (is_subtraction) ? " - " : " + ";
    }
    math_expression_string += math_constant_buffer + " * " + prefix + "(" + name + ")";

    math_constant_buffer = "1.0";
    math_selected_element_index = -1;
    math_selected_node_index = -1;
}

void graphical_view::execute_math_operation()
{
    if (math_terms.empty()) return;

    Signal final_signal = math_terms[0];
    final_signal.name = math_expression_string;

    // adding all terms
    for (int i = 1; i < math_terms.size(); ++i)
    {
        for (int j = 0; j < final_signal.data_points.size(); ++j)
        {
            final_signal.data_points[j].first += math_terms[i].data_points[j].first;
        }
    }

    // adding the signal
    if (!plot_view) { plot_view = make_unique<Plot_View>(); }
    final_signal.color = default_colors[color_index % default_colors.size()];
    color_index++;
    if (color_index == 15)
        color_index = 0;
    plot_view->add_signal(final_signal);
    plot_view->auto_zoom();
    if (current_analysis_mode == Analysis_Mode::Transient)
    {
        if (math_selected_signal_type == Signal_Type::Voltage)     plot_view->set_y_unit(Unit::V);
        else if (math_selected_signal_type == Signal_Type::Current)     plot_view->set_y_unit(Unit::A);
        else if (math_selected_signal_type == Signal_Type::Power)     plot_view->set_y_unit(Unit::W);
        plot_view->set_x_unit(Unit::s);
    }
    else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
    {
        if (math_selected_signal_type == Signal_Type::Mag)  plot_view->set_y_unit(Unit::dB);
        else if (math_selected_signal_type == Signal_Type::Phase)  plot_view->set_y_unit(Unit::deg);

        if (AC_sweep_type == AC_Sweep_Type::Linear)     plot_view->set_x_unit(Unit::Hz);
        else if (AC_sweep_type == AC_Sweep_Type::Octave)     plot_view->set_x_unit(Unit::Oct);
        else if (AC_sweep_type == AC_Sweep_Type::Decade)     plot_view->set_x_unit(Unit::Dec);
    }
    else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
    {
        if (math_selected_signal_type == Signal_Type::Mag)  plot_view->set_y_unit(Unit::dB);
        else if (math_selected_signal_type == Signal_Type::Phase)  plot_view->set_y_unit(Unit::deg);
        plot_view->set_x_unit(Unit::deg);
    }

    math_terms.clear();
    math_expression_string.clear();
    math_operation_mode = false;
}

void graphical_view::draw_save_menu(SDL_Renderer *renderer, TTF_Font *font, Controller *C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color TEXT_BOX_BG = {33, 37, 41, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color HIGHLIGHT_COLOR = {52, 152, 219, 255};

    int menu_width = 400;
    int menu_height = 420;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);

    vector<Editable_Property> props = {{"Name", current_file_name}, {"Address", current_file_address}};

    property_rects.clear();

    int start_y = menu_panel.y + 50;
    int row_height = 40;
    for (int i = 0; i < props.size(); ++i)
    {
        render_text(renderer, font, props[i].label, menu_panel.x + 20, start_y + (i * row_height), TEXT_COLOR);

        SDL_Rect textbox_rect = {menu_panel.x + 180, start_y + (i * row_height) - 5, 200, 30};
        property_rects.push_back(textbox_rect);

        SDL_SetRenderDrawColor(renderer, TEXT_BOX_BG.r, TEXT_BOX_BG.g, TEXT_BOX_BG.b, TEXT_BOX_BG.a);
        SDL_RenderFillRect(renderer, &textbox_rect);

        if (i == active_edit_box)
        {
            SDL_SetRenderDrawColor(renderer, HIGHLIGHT_COLOR.r, HIGHLIGHT_COLOR.g, HIGHLIGHT_COLOR.b, HIGHLIGHT_COLOR.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
        }
        SDL_RenderDrawRect(renderer, &textbox_rect);

        if (i < edit_buffers.size())
        {
            render_text(renderer, font, edit_buffers[i], textbox_rect.x + 5, textbox_rect.y + 5, TEXT_COLOR);
        }
    }

    ok_button_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 100, 30};
    cancel_button_rect = {menu_panel.x + menu_width - 110, menu_panel.y + menu_height - 50, 100, 30};

    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderFillRect(renderer, &ok_button_rect);
    SDL_RenderFillRect(renderer, &cancel_button_rect);

    render_text(renderer, font, "OK", ok_button_rect.x + 40, ok_button_rect.y + 5, TEXT_COLOR);
    render_text(renderer, font, "Cancel", cancel_button_rect.x + 25, cancel_button_rect.y + 5, TEXT_COLOR);
}

void graphical_view::draw_file_menu(SDL_Renderer *renderer, TTF_Font *font, Controller *C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color ITEM_BG = {40, 45, 53, 255};
    const SDL_Color HOVER_BG = {60, 68, 80, 255};
    const SDL_Color SELECTED_BG = {52, 152, 219, 255};
    const SDL_Color BUTTON_BG = {80, 88, 99, 255};

    int menu_width = 400;
    int menu_height = 500;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);

    // draw file list
    render_text(renderer, font, "Open File", menu_panel.x + 10, menu_panel.y + 10, TEXT_COLOR);
    file_button_rects.clear();
    vector<string> file_names = C->get_file_names();
//    for (string name : C->get_file_names())
//        cout << name << endl;
//    if (C->get_file_names().empty())
//        cout << "no files" << endl;

    int start_y = menu_panel.y + 50;
    int row_height = 35;
    for (int i = 0; i < file_names.size(); ++i)
    {
        SDL_Rect item_rect = {menu_panel.x + 10, start_y + (i * row_height), menu_width - 20, 30};
        file_button_rects.push_back(item_rect);

        // background color based on state
        if (i == selected_file_index)
        {
            SDL_SetRenderDrawColor(renderer, SELECTED_BG.r, SELECTED_BG.g, SELECTED_BG.b, SELECTED_BG.a);
        }
        else if (i == hovered_file_index)
        {
            SDL_SetRenderDrawColor(renderer, HOVER_BG.r, HOVER_BG.g, HOVER_BG.b, HOVER_BG.a);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, ITEM_BG.r, ITEM_BG.g, ITEM_BG.b, ITEM_BG.a);
        }
        SDL_RenderFillRect(renderer, &item_rect);

        render_text(renderer, font, file_names[i], item_rect.x + 10, item_rect.y + 5, TEXT_COLOR);
    }

    // OK cancel
    file_ok_button_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 100, 40};
    file_cancel_button_rect = {menu_panel.x + menu_width - 110, menu_panel.y + menu_height - 50, 100, 40};
    SDL_SetRenderDrawColor(renderer, BUTTON_BG.r, BUTTON_BG.g, BUTTON_BG.b, BUTTON_BG.a);
    SDL_RenderFillRect(renderer, &file_ok_button_rect);
    SDL_RenderFillRect(renderer, &file_cancel_button_rect);
    render_text(renderer, font, "OK", file_ok_button_rect.x + 35, file_ok_button_rect.y + 10, TEXT_COLOR);
    render_text(renderer, font, "Cancel", file_cancel_button_rect.x + 20, file_cancel_button_rect.y + 10, TEXT_COLOR);
}

void graphical_view::draw_SubC_menu(SDL_Renderer *renderer, TTF_Font *font, Controller* C)
{
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color SELECTED = {80, 88, 99, 255};
    const SDL_Color HOVERED = {60, 68, 80, 255};

    int menu_width = 800;
    int menu_height = 500;
    SDL_Rect menu_panel = {200, 100, menu_width, menu_height};
    SDL_Rect preview_panel = {menu_panel.x + 550, menu_panel.y + 50, 200, 200};

    // panels
    SDL_SetRenderDrawColor(renderer, 50, 58, 69, 255);
    SDL_RenderFillRect(renderer, &menu_panel);
    SDL_SetRenderDrawColor(renderer, 236, 239, 244, 255);
    SDL_RenderFillRect(renderer, &preview_panel);

    // items
    int start_y = menu_panel.y + 60;
    render_text(renderer, font, "Sub_Circuits:", menu_panel.x + 20, menu_panel.y + 30, TEXT_COLOR);
    for (int i = 0; i < SubC_menu_items.size(); ++i)
    {
        if (i == selected_SubC_menu_item_index)
        {
            SDL_SetRenderDrawColor(renderer, SELECTED.r, SELECTED.g, SELECTED.b, SELECTED.a);
            SDL_RenderFillRect(renderer, &SubC_menu_items[i].rect);
        }
        else if (i == hovered_SubC_item_index)
        {
            SDL_SetRenderDrawColor(renderer, HOVERED.r, HOVERED.g, HOVERED.b, HOVERED.a);
            SDL_RenderFillRect(renderer, &SubC_menu_items[i].rect);
        }
        SubC_menu_items[i].rect = {menu_panel.x + 20, start_y + (i * 30), 200, 25};
        render_text(renderer, font, SubC_menu_items[i].name, SubC_menu_items[i].rect.x, SubC_menu_items[i].rect.y, {200, 200, 200, 255});
    }

    // preview
    if (selected_SubC_menu_item_index != -1)
    {
        Graphical_SubCircuit preview(nullptr, "");
        preview.bounding_box = preview_panel;
        preview.draw(renderer, false);
    }

    create_new_SubC_button_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 160, 30};
    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderFillRect(renderer, &create_new_SubC_button_rect);
    render_text(renderer, font, "Create New", create_new_SubC_button_rect.x + 40, create_new_SubC_button_rect.y + 5, TEXT_COLOR);
}

void graphical_view::draw_text_input_menu(SDL_Renderer* renderer, TTF_Font* font)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color TEXT_BOX_BG = {33, 37, 41, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color HIGHLIGHT_COLOR = {52, 152, 219, 255};

    int menu_width = 400;
    int menu_height = 150;
    SDL_Rect menu_panel = {(m_window_width - menu_width) / 2, (m_window_height - menu_height) / 2, menu_width, menu_height};

    // background
    SDL_SetRenderDrawColor(renderer, PANEL_BG.r, PANEL_BG.g, PANEL_BG.b, PANEL_BG.a);
    SDL_RenderFillRect(renderer, &menu_panel);
    render_text(renderer, font, "Name of the Sub_Circuit:", menu_panel.x + 10, menu_panel.y + 10, TEXT_COLOR);

    // text box
    text_input_box_rect = {menu_panel.x + 10, menu_panel.y + 40, menu_width - 20, 40};
    SDL_SetRenderDrawColor(renderer, TEXT_BOX_BG.r, TEXT_BOX_BG.g, TEXT_BOX_BG.b, TEXT_BOX_BG.a);
    SDL_RenderFillRect(renderer, &text_input_box_rect);
    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderDrawRect(renderer, &text_input_box_rect);
    render_text(renderer, font, text_input_buffer, text_input_box_rect.x + 5, text_input_box_rect.y + 10, TEXT_COLOR);

    // OK cancel buttons
    text_input_ok_rect = {menu_panel.x + menu_width - 220, menu_panel.y + menu_height - 50, 100, 40};
    text_input_cancel_rect = {menu_panel.x + menu_width - 110, menu_panel.y + menu_height - 50, 100, 40};
    SDL_SetRenderDrawColor(renderer, BORDER_COLOR.r, BORDER_COLOR.g, BORDER_COLOR.b, BORDER_COLOR.a);
    SDL_RenderFillRect(renderer, &text_input_ok_rect);
    SDL_RenderFillRect(renderer, &text_input_cancel_rect);
    render_text(renderer, font, "OK", text_input_ok_rect.x + 35, text_input_ok_rect.y + 10, TEXT_COLOR);
    render_text(renderer, font, "Cancel", text_input_cancel_rect.x + 20, text_input_cancel_rect.y + 10, TEXT_COLOR);
}

void graphical_view::draw_status_bar(SDL_Renderer* renderer, TTF_Font* font)
{
    // background
    SDL_Rect bar_rect = {0, m_window_height - 25, m_window_width, 25};
    SDL_SetRenderDrawColor(renderer, 50, 58, 69, 255);
    SDL_RenderFillRect(renderer, &bar_rect);

    // text
    render_text(renderer, font, info_bar_text, 5, bar_rect.y + 3, {211, 211, 211, 255});
}

// main functions

bool graphical_view::run(Controller *C)
{
    cout << C->getFile_Handler().getMainFolderPath() << endl;

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
    Graphical_Element::set_font(font);

    initialize_menu();
    initialize_toolbar(font);
    initialize_SubC_menu(C);
    C->getFile_Handler().loadFiles();
    C->loadGraphicalSubCircuits();

    SDL_Window* window = SDL_CreateWindow(
            "SparkSense",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280,
            720,
            SDL_WINDOW_RESIZABLE
    );

    main_window_id = SDL_GetWindowID(window);

    if (!window)
    {
        SDL_Quit();
        throw runtime_error("Window could not be created! SDL_Error: " + string(SDL_GetError()));
    }

    SDL_Surface* icon_surface = IMG_Load(ICON);
    if (icon_surface) {
        SDL_SetWindowIcon(window, icon_surface);
        SDL_FreeSurface(icon_surface);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw runtime_error("Renderer could not be created! SDL_Error: " + string(SDL_GetError()));
    }

    SDL_GetWindowSize(window, &m_window_width, &m_window_height);

    default_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    crosshair_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    math_operation_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    dragging_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    not_valid_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
    loading_cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);

    SDL_Surface* probe_surface = IMG_Load(PROBE);
    if (probe_surface)
    {
        probe_cursor = SDL_CreateColorCursor(probe_surface, 4, 60);
        SDL_FreeSurface(probe_surface);
    }
    SDL_Surface* delete_surface = IMG_Load(DELETE);
    if (delete_surface)
    {
        deleting_cursor = SDL_CreateColorCursor(delete_surface, 4, 60);
        SDL_FreeSurface(delete_surface);
    }
    SDL_Surface* ground_surface = IMG_Load(GROUND);
    if (ground_surface)
    {
        grounding_cursor = SDL_CreateColorCursor(ground_surface, 16, 0);
        SDL_FreeSurface(ground_surface);
    }
    SDL_Surface* tick_surface = IMG_Load(TICK);
    if (tick_surface)
    {
        finished_action_cursor = SDL_CreateColorCursor(tick_surface, 0, 0);
        SDL_FreeSurface(tick_surface);
    }

    bool running = true;
    SDL_Event event;


    while (running)
    {
        SDL_GetWindowSize(window, &m_window_width, &m_window_height);

        auto& graphical_elements = C->get_graphical_elements();
        auto& graphical_wires = C->get_graphical_wires();

        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (feedback_cursor_state != Feedback_Cursor_State::None)
        {
            Uint32 elapsed_ms = SDL_GetTicks() - feedback_cursor_timer_start;

            if (feedback_cursor_state == Feedback_Cursor_State::Loading)
            {
                SDL_SetCursor(loading_cursor);
                if (elapsed_ms > 1500)
                {
                    feedback_cursor_state = Feedback_Cursor_State::Finished;
                }
            }
            else if (feedback_cursor_state == Feedback_Cursor_State::Finished)
            {
                SDL_SetCursor(finished_action_cursor);
                if (elapsed_ms > 3000)
                {
                    feedback_cursor_state = Feedback_Cursor_State::None;
                }
            }
        }
        else
        {
            // if the mouse is in the toolbar
            if (mouseY < 40)
            {
                SDL_ShowCursor(SDL_ENABLE);
                if (is_dragging)
                {
                    if (not_valid_cursor) SDL_SetCursor(not_valid_cursor);
                }
                else
                {
                    if (default_cursor) SDL_SetCursor(default_cursor);
                }
            }
            else
            {
                if (is_dragging)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (dragging_cursor) SDL_SetCursor(dragging_cursor);
                }
                else if (probe_mode)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (probe_cursor) SDL_SetCursor(probe_cursor);
                }
                else if (is_wiring)
                {
                    SDL_ShowCursor(SDL_DISABLE);
                }
                else if (math_operation_mode || is_file_menu_open || sub_circuit_menu || elements_menu)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (math_operation_cursor) SDL_SetCursor(math_operation_cursor);
                }
                else if (is_deleting)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (deleting_cursor) SDL_SetCursor(deleting_cursor);
                }
                else if (is_grounding)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (grounding_cursor) SDL_SetCursor(grounding_cursor);
                }
                else if (naming_SubC_menu_active)
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (default_cursor) SDL_SetCursor(default_cursor);
                }
                else if (create_SubC_mode)
                {
                    if (node1 == nullptr)
                    {
                        SDL_ShowCursor(SDL_ENABLE);
                        if (crosshair_cursor) SDL_SetCursor(crosshair_cursor);
                    }
                    else
                    {
                        SDL_ShowCursor(SDL_DISABLE);
                    }
                }
                else
                {
                    SDL_ShowCursor(SDL_ENABLE);
                    if (crosshair_cursor) SDL_SetCursor(crosshair_cursor);
                }
            }
        }

        Graphical_Element* hovered_element = find_element_at({mouseX, mouseY}, C);
        if (is_wiring)
        {
            info_bar_text = "Press and hold the left mouse button to start wiring.";
        }
        else if (is_deleting)
        {
            info_bar_text = "Click on a component to delete.";
        }
        else if (probe_mode)
        {
            info_bar_text = "Click on a Valid point to show the signal.";
        }
        else if (elements_menu)
        {
            info_bar_text = "Single Click to see preview, Double click to Select.";
        }
        else if (sub_circuit_menu)
        {
            info_bar_text = "Single Click to see preview, Double click to Select. Click on new to create a new one.";
        }
        else if (is_grounding)
        {
            info_bar_text = "Click on a Component Terminal or a Wire to make it Ground.";
        }
        else if (is_file_menu_open)
        {
            info_bar_text = "Select a File to Open.";
        }
        else if (is_labeling)
        {
            info_bar_text = "Select a Valid point to Label.";
        }
        else if (is_saving)
        {
            info_bar_text = "Enter Name and Address.";
        }
        else if (hovered_element)
        {
            info_bar_text = hovered_element->get_info_text();
        }
        else
        {
            info_bar_text = "";
        }

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                throw Quit_Event_Exception();
                continue;
            }

            // window ID from the event
            Uint32 event_window_id = 0;
            switch (event.type)
            {
                case SDL_MOUSEMOTION:       event_window_id = event.motion.windowID; break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:     event_window_id = event.button.windowID; break;
                case SDL_KEYDOWN:
                case SDL_KEYUP:             event_window_id = event.key.windowID;    break;
                case SDL_WINDOWEVENT:       event_window_id = event.window.windowID; break;
                case SDL_TEXTINPUT:         event_window_id = event.text.windowID;   break;
                case SDL_MOUSEWHEEL:        event_window_id = event.wheel.windowID;  break;
            }

            // handling the event
            if (plot_view && event_window_id == plot_view->get_window_id())
            {
                Plot_Event_Status status = plot_view->handle_event(event);
                if (status == Plot_Event_Status::Close_Request)
                {
                    plot_view.reset();
                }
            }

            else if (event_window_id == main_window_id)
            {
                if (event.type == SDL_MOUSEMOTION)
                {
                    SDL_Point mouse_pos = {event.motion.x, event.motion.y};

                    hovered_button_index = -1;

                    for (int i = 0; i < toolbar_buttons.size(); ++i)
                    {
                        if (SDL_PointInRect(&mouse_pos, &toolbar_buttons[i].rect))
                        {
                            hovered_button_index = i;
                            break;
                        }
                    }
                }

                bool event_was_handled = handle_toolbar_events(event, C);

                if (!event_was_handled)
                {
                    if (elements_menu)
                    {
                        running = handle_menu_events(event, C);
                    }
                    else if (editing)
                    {
                        running = handle_edit_properties_menu(event, C);
                    }
                    else if (is_wiring)
                    {
                        running = handle_wiring_events(event, C);
                    }
                    else if (is_grounding)
                    {
                        running = handle_grounding_events(event, C);
                    }
                    else if (is_labeling)
                    {
                        running = handle_net_labeling_events(event, C);
                    }
                    else if (is_configuring_analysis)
                    {
                        running = handle_configure_analysis_events(event, C);
                    }
                    else if (probe_mode)
                    {
                        running = handle_probing_events(event, C);
                    }
                    else if (math_operation_mode)
                    {
                        running = handle_math_operation_events(event, C);
                    }
                    else if (is_saving)
                    {
                        running = handle_saving_events(event, C);
                    }
                    else if (is_deleting)
                    {
                        running = handle_deleting_events(event, C);
                    }
                    else if (is_file_menu_open)
                    {
                        running = handle_file_menu_events(event, C);
                    }
                    else if (sub_circuit_menu)
                    {
                        running = handle_SubC_menu_events(event, C);
                    }
                    else if (create_SubC_mode)
                    {
                        running = handle_SubC_creation_events(event, C);
                    }
                    else if (naming_SubC_menu_active)
                    {
                        running = handle_text_input_menu_events(event);
                    }
                    else
                    {
                        running = handle_events(event, C);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (show_grids)
        {
            draw_grid(renderer);
        }

        draw_toolbar(renderer, font);
        draw_status_bar(renderer, font);

        if (is_wiring)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point snapped_mouse = snap_to_grid(mouseX, mouseY, GRID_SIZE);
            SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);

            if (mouseY > 40)
            {
                SDL_RenderDrawLine(renderer, snapped_mouse.x, 40, snapped_mouse.x, m_window_height);
                SDL_RenderDrawLine(renderer, 0, snapped_mouse.y, m_window_width, snapped_mouse.y);
            }
        }


        for (const auto& element : graphical_elements)
        {
            element->draw(renderer, show_grids);
        }

        for (const auto& wire : graphical_wires)
        {
            wire->draw(renderer);
        }

        map<SDL_Point, int, Point_Comparator> endpoint_counts;

        for (const auto& wire : C->get_graphical_wires())
        {
            if (wire->path.size() >= 2) {
                endpoint_counts[wire->path.front()]++;
                endpoint_counts[wire->path.back()]++;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for (const auto& pair : endpoint_counts)
        {
            const SDL_Point& point = pair.first;
            const int count = pair.second;

            if (count > 2)
            {
                int junction_size = 8;
                SDL_Rect junction_rect = {
                        point.x - junction_size / 2,
                        point.y - junction_size / 2,
                        junction_size,
                        junction_size
                };
                SDL_RenderFillRect(renderer, &junction_rect);
            }
        }

        if (is_wiring && !new_wire_points.empty())
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderDrawLine(renderer, mouseX, mouseY, new_wire_points.front().pos.x, new_wire_points.front().pos.y);
        }

        if (elements_menu)
        {
            draw_component_menu(renderer, font);
        }

        if (editing)
        {
            draw_properties_menu(renderer, font, C);
        }

        if (is_configuring_analysis)
        {
            draw_configure_analysis(renderer, font, C);
        }

        if (math_operation_mode)
        {
            draw_math_operation_menu(renderer, font, C);
        }

        if (is_saving)
        {
            draw_save_menu(renderer, font, C);
        }

        if (is_file_menu_open)
        {
            draw_file_menu(renderer, font, C);
        }

        if (sub_circuit_menu)
        {
            draw_SubC_menu(renderer, font, C);
        }

        if (naming_SubC_menu_active)
        {
            draw_text_input_menu(renderer, font);
        }

        if (create_SubC_mode && node1 != nullptr)
        {
            SDL_SetRenderDrawColor(renderer, 0, 128, 0, 255);
            SDL_RenderDrawLine(renderer, mouseX - 10, mouseY, mouseX + 10, mouseY);
            SDL_RenderDrawLine(renderer, mouseX, mouseY - 10, mouseX, mouseY + 10);
        }

        // showing the run
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            int x = 10; int y = 50;
            double t_step = 0; double start = 0; double stop = 0;
            C->get_tran_params(start, stop, t_step);
            string text = ".tran(" + format_with_suffix(start, " ") + format_with_suffix(stop - t_step, " ") + format_with_suffix(t_step, ")");
            render_text(renderer, font, text, x, y, {0, 0, 0, 255});
        }
        else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            int x = 10; int y = 50;
            double start_f, stop_f, num_of_points;
            AC_Sweep_Type current_controller_type;

            C->get_ac_params(start_f, stop_f, num_of_points, current_controller_type);

            string text = ".AC(" + format_with_suffix(start_f, " ") + format_with_suffix(stop_f, " ") + format_with_suffix(num_of_points, " ");

            if (current_controller_type == AC_Sweep_Type::Linear) { text += "Linear"; }
            else if (current_controller_type == AC_Sweep_Type::Octave) { text += "Octave"; }
            else if (current_controller_type == AC_Sweep_Type::Decade) { text += "Decade"; }
            text += ")";

            render_text(renderer, font, text, x, y, {0, 0, 0, 255});
        }

        else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            int x = 10; int y = 50;
            double base_ph = 0; double start_ph = 0; double stop_ph = 0; double num_of_points = 0;
            C->get_phase_params(start_ph, stop_ph, base_ph, num_of_points, phase_sweep_type);
            string text = ".Phase(" + format_with_suffix(base_ph, " ") + format_with_suffix(start_ph, " ") + format_with_suffix(stop_ph, " ") + format_with_suffix(num_of_points, ")");
            render_text(renderer, font, text, x, y, {0, 0, 0, 255});
        }


        SDL_RenderPresent(renderer);

        // plot window
        if (plot_view)
        {
            plot_view->render();
        }

    }

    SDL_FreeCursor(default_cursor);
    SDL_FreeCursor(probe_cursor);
    SDL_FreeCursor(crosshair_cursor);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return true;
}

bool graphical_view::handle_events(SDL_Event& event, Controller* C)
{
    // quit
    if (event.type == SDL_QUIT)
    {
        throw Quit_Event_Exception();
    }

    // window events
    if (event.type == SDL_WINDOWEVENT)
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            m_window_width = event.window.data1;
            m_window_height = event.window.data2;
            cout << "Window resized to " << m_window_width << "x" << m_window_height << endl;
        }
    }

    // keyboard events
    if (event.type == SDL_KEYDOWN)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Keymod modState = SDL_GetModState();

        switch (event.key.keysym.sym)
        {
            case SDLK_r:
            {
                if (modState & KMOD_CTRL)
                {
                    if (is_dragging)
                    {
                        auto& graphical_elements = C->get_graphical_elements();
                        auto& element_to_rotate = graphical_elements[dragged_element_index];
                        element_to_rotate->change_rotation();
                        if (element_to_rotate->get_model() != nullptr)
                        {
                            element_to_rotate->get_model()->set_rotation(element_to_rotate->get_rotation());
                        }
                        else if (dynamic_cast<Graphical_SubCircuit*>(element_to_rotate.get()))
                        {
                            dynamic_cast<Graphical_SubCircuit*>(element_to_rotate.get())->get_subcircuit_model()->set_rotation(element_to_rotate->get_rotation());
                        }
                    }
                }
                else
                {
                    SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                    C->add_Graphical_Resistor(snapped_pos.x, snapped_pos.y);
                }
                break;
            }

            case SDLK_c:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_Capacitor(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_l:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_Inductor(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_s:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_Current_Source(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_d:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_Real_Diode(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_z:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_Zener_Diode(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_v:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                C->add_Graphical_DC_Source(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_p:
            {
                elements_menu = !elements_menu;
                initialize_menu();
                initialize_SubC_menu(C);
                break;
            }

            case SDLK_g:
            {
                is_grounding = !is_grounding;
                break;
            }

            case SDLK_w:
            {
                is_wiring = !is_wiring;
                new_wire_points.clear();
                break;
            }

            case SDLK_n:
            {
                is_labeling = !is_labeling;
                break;
            }

            case SDLK_BACKSPACE:
            {
                is_deleting = !is_deleting;
                break;
            }

            case SDLK_DELETE:
            {
                is_deleting = !is_deleting;
                break;
            }

            case SDLK_RETURN:
                cout << "Enter key was pressed." << endl;
                break;

            case SDLK_ESCAPE:
                cout << "Escape key was pressed." << endl;
                break;

            default:
                break;
        }
    }

    // mouse events
    auto& graphical_elements = C->get_graphical_elements();

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        if (event.button.button == SDL_BUTTON_LEFT)
        {
            for (int i = graphical_elements.size() - 1; i >= 0; --i)
            {
                if (SDL_PointInRect(&mouse_pos, &graphical_elements[i]->bounding_box))
                {
                    if (dynamic_cast<Graphical_Ground*>(graphical_elements[i].get()) || dynamic_cast<Graphical_Net_Label*>(graphical_elements[i].get()))
                    {
                    }
                    else
                    {
                        is_dragging = true;
                        dragged_element_index = i;
                        drag_offset.x = mouse_pos.x - graphical_elements[i]->bounding_box.x;
                        drag_offset.y = mouse_pos.y - graphical_elements[i]->bounding_box.y;
                    }
                    break;
                }
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            for (int i = graphical_elements.size() - 1; i >= 0; --i)
            {
                if (SDL_PointInRect(&mouse_pos, &graphical_elements[i]->bounding_box)/* && graphical_elements[i]->get_model() != nullptr*/)
                {
                    editing = true;
                    edited_element_index = i;

                    auto props = graphical_elements[i]->get_editable_properties();

                    edit_buffers.clear();
                    for (const auto& prop : props)
                    {
                        edit_buffers.push_back(prop.value_as_string);
                    }
                    active_edit_box = -1;

                    break;
                }
            }

        }
    }

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        is_dragging = false;
        dragged_element_index = -1;
    }

    if (event.type == SDL_MOUSEMOTION && is_dragging)
    {
        SDL_Point mouse_pos = {event.motion.x, event.motion.y};

        int new_x = mouse_pos.x - drag_offset.x;
        int new_y = mouse_pos.y - drag_offset.y;

        SDL_Point snapped_pos = snap_to_grid(new_x, new_y, GRID_SIZE);

        graphical_elements[dragged_element_index]->bounding_box.x = snapped_pos.x;
        graphical_elements[dragged_element_index]->bounding_box.y = snapped_pos.y;
        if (graphical_elements[dragged_element_index]->get_model() != nullptr)
        {
            graphical_elements[dragged_element_index]->get_model()->set_coordinates(snapped_pos.x, snapped_pos.y);
        }
        else if (dynamic_cast<Graphical_SubCircuit*>(graphical_elements[dragged_element_index].get()))
        {
            dynamic_cast<Graphical_SubCircuit*>(graphical_elements[dragged_element_index].get())->get_subcircuit_model()->set_coordinates(snapped_pos.x, snapped_pos.y);
        }
    }

    return true;
}

bool graphical_view::handle_menu_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE)
        {
            elements_menu = false;
        }
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        SDL_Point mouse_pos = {event.motion.x, event.motion.y};
        hovered_menu_item_index = -1;
        hovered_SubC_item_index = -1;
        bool hover_found = false;

        for (int i = 0; i < menu_items.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &menu_items[i].rect))
            {
                hovered_menu_item_index = i;
                hover_found = true;
                break;
            }
        }
        if (!hover_found)
        {
            for (int i = 0; i < SubC_menu_items.size(); ++i)
            {
                if (SDL_PointInRect(&mouse_pos, &SubC_menu_items[i].rect))
                {
                    hovered_SubC_item_index = i;
                    break;
                }
            }
        }
    }


    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        bool clicked_on_item = false;
        for (int i = 0; i < menu_items.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &menu_items[i].rect))
            {
                clicked_on_item = true;

                // click
                if (event.button.clicks == 1)
                {
                    selected_menu_item_index = i;
                    selected_SubC_menu_item_index = -1;
                }
                // double click
                else if (event.button.clicks == 2)
                {
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    // adding the component
                    switch (menu_items[i].type)
                    {
                        case Element_Type::Resistor: C->add_Graphical_Resistor(mouseX, mouseY); break;
                        case Element_Type::Capacitor: C->add_Graphical_Capacitor(mouseX, mouseY); break;
                        case Element_Type::Inductor: C->add_Graphical_Inductor(mouseX, mouseY); break;
                        case Element_Type::Current_Source: C->add_Graphical_Current_Source(mouseX, mouseY); break;
                        case Element_Type::Real_Diode: C->add_Graphical_Real_Diode(mouseX, mouseY); break;
                        case Element_Type::Zener_Diode: C->add_Graphical_Zener_Diode(mouseX, mouseY); break;
                        case Element_Type::Voltage_Source:
                        {
                            if (menu_items[i].subtype_tag == "DC")
                                C->add_Graphical_DC_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "SIN")
                                C->add_Graphical_Sin_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "Pulse")
                                C->add_Graphical_Pulse_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "Delta")
                                C->add_Graphical_Dirac_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "Square")
                                C->add_Graphical_Square_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "Triangular")
                                C->add_Graphical_Triangular_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "AC")
                                C->add_Graphical_AC_Phase_Source(mouseX, mouseY);
                            else if (menu_items[i].subtype_tag == "Waveform")
                                C->add_Graphical_WF_Source(mouseX, mouseY);
                            break;
                        }
                        case Element_Type::VC_Voltage_Source: C->add_Graphical_VCVS(mouseX, mouseY); break;
                        case Element_Type::VC_Current_Source: C->add_Graphical_VCCS(mouseX, mouseY); break;
                        case Element_Type::CC_Voltage_Source: C->add_Graphical_CCVS(mouseX, mouseY); break;
                        case Element_Type::CC_Current_source: C->add_Graphical_CCCS(mouseX, mouseY); break;

                    }

                    elements_menu = false;
                    selected_menu_item_index = -1;
                    selected_SubC_menu_item_index = -1;
                }
                break;
            }
        }
        if (!clicked_on_item)
        {
            for (int i = 0; i < SubC_menu_items.size(); ++i)
            {
                if (SDL_PointInRect(&mouse_pos, &SubC_menu_items[i].rect))
                {
                    selected_SubC_menu_item_index = i;
                    selected_menu_item_index = -1;

                    if (event.button.clicks == 2)
                    {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        C->add_Graphical_Sub_Circuit(mouseX, mouseY, SubC_menu_items[i].name);
                        elements_menu = false;
                        selected_menu_item_index = -1;
                        selected_SubC_menu_item_index = -1;
                    }
                    break;
                }
            }
        }
    }
    return true;
}

bool graphical_view::handle_edit_properties_menu(SDL_Event &event, Controller *C)
{
    // helper
    auto save_and_exit = [&]() {
        auto& element = C->get_graphical_elements()[edited_element_index];

        if (auto net_label = dynamic_cast<Graphical_Net_Label*>(element.get()))
        {
            Node* node_to_name = net_label->get_node();
            string new_name = edit_buffers[0];
            C->assign_net_name(node_to_name, new_name);
            net_label->set_label(new_name);
        }
        else
        {
            C->update_element_properties(edited_element_index, edit_buffers);
        }
        editing = false;
        SDL_StopTextInput();
    };

    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        // OK or Cancel
        if (SDL_PointInRect(&mouse_pos, &ok_button_rect))
        {
            // OK
            save_and_exit();
        }
        else if (SDL_PointInRect(&mouse_pos, &cancel_button_rect))
        {
            // Cancel
            editing = false;
            SDL_StopTextInput();
        }
        else
        {
            // text box
            bool an_edit_box_was_clicked = false;
            for (int i = 0; i < property_rects.size(); ++i)
            {
                if (SDL_PointInRect(&mouse_pos, &property_rects[i]))
                {
                    active_edit_box = i;
                    SDL_StartTextInput();
                    an_edit_box_was_clicked = true;
                    break;
                }
            }
            if (!an_edit_box_was_clicked)
            {
                active_edit_box = -1;
                SDL_StopTextInput();
            }
        }
    }

    if (event.type == SDL_TEXTINPUT && active_edit_box != -1)
    {
        edit_buffers[active_edit_box] += event.text.text;
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                editing = false;
                SDL_StopTextInput();
                break;

            case SDLK_BACKSPACE:
                if (active_edit_box != -1 && !edit_buffers[active_edit_box].empty())
                {
                    edit_buffers[active_edit_box].pop_back();
                }
                break;

            case SDLK_RETURN:
                save_and_exit();
                break;
        }
    }

    return true;
}

bool graphical_view::handle_wiring_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_ESCAPE)
        {
            is_wiring = false;
            new_wire_points.clear();
        }
        if (event.key.keysym.sym == SDLK_g)
        {
            is_wiring = false;
            is_grounding = true;
            new_wire_points.clear();
        }
        if (event.key.keysym.sym == SDLK_BACKSPACE || event.key.keysym.sym == SDLK_DELETE)
        {
            is_wiring = false;
            is_deleting = true;
            new_wire_points.clear();
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        is_wiring = false;
        new_wire_points.clear();
    }

    // start wiring
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (new_wire_points.empty())
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point click_pos = {mouseX, mouseY};
            SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

            // if the click is on a components connection point
            bool started_on_component = false;
            for (auto& element : C->get_graphical_elements())
            {
                auto connection_points = element->get_connection_points();
                for (auto& point : connection_points)
                {
                    if (abs(snapped_pos.x - point.pos.x) < 20 && abs(snapped_pos.y - point.pos.y) < 20)
                    {
                        new_wire_points.push_back(point);
                        started_on_component = true;
                        break;
                    }
                }
                if (started_on_component) break;
            }
            if(started_on_component) return true;


            // see if the click is on a wire
            const float CLICK_TOLERANCE = 10.0f;
            auto& wires = C->get_graphical_wires();

            int index_to_delete = -1;
            SDL_Point junction_pos;
            size_t segment_index = 0;

            // finding the wire
            for (size_t k = 0; k < wires.size(); ++k)
            {
                auto& wire = wires[k];
                for (size_t i = 0; i < wire->path.size() - 1; ++i)
                {
                    SDL_Point p1 = wire->path[i];
                    SDL_Point p2 = wire->path[i+1];

                    if (dist_to_segment(click_pos, p1, p2) < CLICK_TOLERANCE)
                    {
                        index_to_delete = k;
                        segment_index = i;

                        // junction position
                        // vertical segment
                        if (p1.x == p2.x)
                        {
                            junction_pos = { p1.x, snapped_pos.y };
                        }
                        // horizantal segment
                        else
                        {
                            junction_pos = { snapped_pos.x, p1.y };
                        }

                        break;
                    }
                }
                if (index_to_delete != -1)
                {
                    break;
                }
            }

            if (index_to_delete != -1)
            {
                Node* existing_node = wires[index_to_delete]->start_node;
                vector<SDL_Point> original_path = wires[index_to_delete]->path;

                // wire from start to junction
                vector<Connection_Point> path_a;
                for(size_t j=0; j <= segment_index; ++j) path_a.push_back({original_path[j]});
                path_a.push_back({junction_pos});
                C->add_Graphical_Wire(path_a, existing_node, existing_node);

                // wire from junction to end
                vector<Connection_Point> path_b;
                path_b.push_back({junction_pos});
                for(size_t j = segment_index + 1; j < original_path.size(); ++j) path_b.push_back({original_path[j]});
                C->add_Graphical_Wire(path_b, existing_node, existing_node);

                // deleting the old wire
                wires.erase(wires.begin() + index_to_delete);

                // starting the new wire
                SDL_Point p1 = original_path[segment_index];
                SDL_Point p2 = original_path[segment_index + 1];
                Rotation new_wire_rot = (p1.x == p2.x) ? Rotation::Right : Rotation::Down;
                new_wire_points.push_back({junction_pos, existing_node, new_wire_rot});

                cout << "Started wiring from existing net." << endl;
                return true;
            }
        }
    }

    // finishing wiring
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        if (!new_wire_points.empty())
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);
            auto& elements = C->get_graphical_elements();

            Graphical_Element* target_element = nullptr;
            int target_point_index = -1;

            for (auto& element : elements) {
                auto connection_points = element->get_connection_points();
                for (int i = 0; i < connection_points.size(); ++i) {
                    if (abs(snapped_pos.x - connection_points[i].pos.x) < 20 && abs(snapped_pos.y - connection_points[i].pos.y) < 20) {
                        target_element = element.get();
                        target_point_index = i;
                        break;
                    }
                }
                if (target_element) break;
            }

            if (target_element) {
                Connection_Point target_point = target_element->get_connection_points()[target_point_index];

                Node* start_node = new_wire_points.front().node;
                Node* end_node = target_point.node;

                Node* node_to_keep = start_node;
                Node* node_to_merge = end_node;
                if (end_node->is_the_node_ground() || !end_node->net_name.empty()) {
                    node_to_keep = end_node;
                    node_to_merge = start_node;
                }

                Node* final_node = C->connect_nodes(node_to_keep, node_to_merge);

                SDL_Point start_pos = new_wire_points.front().pos;
                SDL_Point end_pos = target_point.pos;
                Rotation start_rot = new_wire_points.front().rotation;
                Rotation end_rot = target_point.rotation;
                vector<SDL_Point> path_points;
                path_points.push_back(start_pos);
                SDL_Point corner;

                if (start_rot == Rotation::Right)
                {
                    if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                    else if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                    {
                        if (start_pos.x < end_pos.x)
                        {
                            corner = { end_pos.x, start_pos.y };
                        }
                        else
                        {
                            corner = { start_pos.x, end_pos.y };
                        }
                    }
                }

                else if (start_rot == Rotation::Left)
                {
                    if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                    else if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                    {
                        if (start_pos.x > end_pos.x)
                        {
                            corner = { end_pos.x, start_pos.y };
                        }
                        else
                        {
                            corner = { start_pos.x, end_pos.y };
                        }
                    }
                }

                else if (start_rot == Rotation::Up)
                {
                    if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                    else if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                    {
                        if (start_pos.y < end_pos.y)
                        {
                            corner = { start_pos.x, end_pos.y };
                        }
                        else
                        {
                            corner = { end_pos.x, start_pos.y };
                        }
                    }
                }

                else if (start_rot == Rotation::Down)
                {
                    if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                    else if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                    {
                        if (start_pos.y > end_pos.y)
                        {
                            corner = { start_pos.x, end_pos.y };
                        }
                        else
                        {
                            corner = { end_pos.x, start_pos.y };
                        }
                    }
                }
                path_points.push_back(corner);

                path_points.push_back(end_pos);

                // creating wire (This part remains the same)
                vector<Connection_Point> final_wire_points;
                for(const auto& p : path_points) final_wire_points.push_back({ p, nullptr, {} });

                final_wire_points.front().node = start_node;
                final_wire_points.front().rotation = start_rot;
                final_wire_points.back().node = end_node;
                final_wire_points.back().rotation = end_rot;

                C->connect_nodes(start_node, end_node);
                C->add_Graphical_Wire(final_wire_points, final_node, final_node);
            }
            // if the click is on an existing wire
            else
            {
                const float CLICK_TOLERANCE = 10.0f;
                auto& wires = C->get_graphical_wires();

                int index_to_delete = -1;
                SDL_Point junction_pos;
                size_t segment_index = 0;

                // find the wire and the click place
                for (size_t k = 0; k < wires.size(); ++k)
                {
                    auto& wire = wires[k];
                    for (size_t i = 0; i < wire->path.size() - 1; ++i)
                    {
                        SDL_Point p1 = wire->path[i];
                        SDL_Point p2 = wire->path[i+1];

                        if (dist_to_segment({mouseX, mouseY}, p1, p2) < CLICK_TOLERANCE)
                        {
                            index_to_delete = k;
                            segment_index = i;
                            junction_pos = (p1.x == p2.x) ? SDL_Point{ p1.x, snapped_pos.y } : SDL_Point{ snapped_pos.x, p1.y };
                            break;
                        }
                    }
                    if (index_to_delete != -1) break;
                }

                if (index_to_delete != -1)
                {
                    Node* source_node = new_wire_points.front().node;
                    Node* target_wire_node = wires[index_to_delete]->start_node;
                    vector<SDL_Point> original_path = wires[index_to_delete]->path;

                    // splitting the wire at the junction
                    vector<Connection_Point> path_a;
                    for(size_t j = 0; j <= segment_index; ++j) path_a.push_back({original_path[j]});
                    path_a.push_back({junction_pos});
                    C->add_Graphical_Wire(path_a, target_wire_node, target_wire_node);

                    vector<Connection_Point> path_b;
                    path_b.push_back({junction_pos});
                    for(size_t j = segment_index + 1; j < original_path.size(); ++j) path_b.push_back({original_path[j]});
                    C->add_Graphical_Wire(path_b, target_wire_node, target_wire_node);

                    wires.erase(wires.begin() + index_to_delete);

                    C->connect_nodes(source_node, target_wire_node);

                    SDL_Point start_pos = new_wire_points.front().pos;
                    SDL_Point end_pos = junction_pos;
                    Rotation start_rot = new_wire_points.front().rotation;

                    SDL_Point seg_p1 = original_path[segment_index];
                    SDL_Point seg_p2 = original_path[segment_index + 1];
                    Rotation end_rot = (seg_p1.x == seg_p2.x) ? Rotation::Left : Rotation::Up;

                    vector<SDL_Point> path_points;

                    path_points.push_back(start_pos);
                    SDL_Point corner_pos;
                    bool is_start_horizontal = (start_rot == Rotation::Left || start_rot == Rotation::Right);

                    if (is_start_horizontal)
                    {
                        corner_pos = { end_pos.x, start_pos.y };
                    }
                    else
                    {
                        corner_pos = { start_pos.x, end_pos.y };
                    }

                    if ((corner_pos.x != start_pos.x || corner_pos.y != start_pos.y) &&
                        (corner_pos.x != end_pos.x || corner_pos.y != end_pos.y))
                    {
                        path_points.push_back(corner_pos);
                    }

                    path_points.push_back(end_pos);


                    // creating wire
                    vector<Connection_Point> final_wire_points;
                    for(const auto& p : path_points) final_wire_points.push_back({ p, nullptr, {} });

                    final_wire_points.front().node = source_node;
                    final_wire_points.front().rotation = start_rot;
                    final_wire_points.back().node = source_node;
                    final_wire_points.back().rotation = end_rot;

                    C->add_Graphical_Wire(final_wire_points, source_node, source_node);
                }
            }

            new_wire_points.clear();
        }
    }
    return true;
}

bool graphical_view::handle_grounding_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_g || event.key.keysym.sym == SDLK_ESCAPE)
        {
            is_grounding = false;
            cout << "Exiting Grounding mode" << endl;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point click_pos = {mouseX, mouseY};
        SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

        const float CLICK_TOLERANCE = 5.0f;
        auto& wires = C->get_graphical_wires();

        int index_to_ground = -1;

        // finding the wire and the node
        for (size_t k = 0; k < wires.size(); ++k)
        {
            auto& wire = wires[k];
            for (size_t i = 0; i < wire->path.size() - 1; ++i)
            {
                SDL_Point p1 = wire->path[i];
                SDL_Point p2 = wire->path[i+1];

                if (dist_to_segment(click_pos, p1, p2) < CLICK_TOLERANCE)
                {
                    index_to_ground = k;
                    break;
                }
            }
            if (index_to_ground != -1)
            {
                break;
            }
        }

        if (index_to_ground != -1)
        {
            Node* node = wires[index_to_ground]->start_node;

            node->make_ground();

            C->add_Graphical_Ground(snapped_pos, node);

            cout << "made the node ground: " << node->get_name() << endl;
            is_grounding = false;
            return true;
        }

        bool clicked_on_component = false;
        for (auto& element : C->get_graphical_elements())
        {
            auto connection_points = element->get_connection_points();
            for (auto& point : connection_points)
            {
                if (abs(snapped_pos.x - point.pos.x) < 10 && abs(snapped_pos.y - point.pos.y) < 10)
                {
                    clicked_on_component = true;

                    Node* node = point.node;

                    node->make_ground();

                    C->add_Graphical_Ground(snapped_pos, node);

                    cout << "made the node ground: " << node->get_name() << endl;
                    is_grounding = false;
                    return true;
                    break;
                }
            }
            if (clicked_on_component) break;
        }
    }

    return true;
}

bool graphical_view::handle_net_labeling_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_n || event.key.keysym.sym == SDLK_ESCAPE)
        {
            is_labeling = false;
            cout << "Exiting Labeling mode" << endl;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        SDL_Point click_pos = {mouseX, mouseY};
        SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

        const float CLICK_TOLERANCE = 10.0f;
        auto& wires = C->get_graphical_wires();
        int index_of_clicked_wire = -1;
        SDL_Point label_pos;

        for (size_t k = 0; k < wires.size(); ++k)
        {
            auto& wire = wires[k];
            for (size_t i = 0; i < wire->path.size() - 1; ++i)
            {
                SDL_Point p1 = wire->path[i];
                SDL_Point p2 = wire->path[i+1];

                if (dist_to_segment(click_pos, p1, p2) < CLICK_TOLERANCE)
                {
                    index_of_clicked_wire = k;
                    if (p1.x == p2.x)
                    {
                        label_pos = { p1.x, snapped_pos.y };
                    }
                    else
                    {
                        label_pos = { snapped_pos.x, p1.y };
                    }
                    break;
                }
            }
            if (index_of_clicked_wire != -1) break;
        }

        if (index_of_clicked_wire != -1)
        {
            Node* target_node = wires[index_of_clicked_wire]->start_node;

            C->add_Graphical_Net_Label(label_pos, target_node);

            is_labeling = false;
            editing = true;

            edited_element_index = C->get_graphical_elements().size() - 1;

            edit_buffers.assign(1, "");
            active_edit_box = 0;
            SDL_StartTextInput();

            cout << "Label placed. Enter name." << endl;
        }

        bool clicked_on_component = false;
        for (auto& element : C->get_graphical_elements())
        {
            auto connection_points = element->get_connection_points();
            for (auto& point : connection_points)
            {
                if (abs(snapped_pos.x - point.pos.x) < 10 && abs(snapped_pos.y - point.pos.y) < 10)
                {
                    clicked_on_component = true;

                    Node* target_node = point.node;

                    label_pos = point.pos;

                    C->add_Graphical_Net_Label(label_pos, target_node);

                    is_labeling = false;
                    editing = true;

                    edited_element_index = C->get_graphical_elements().size() - 1;

                    cout << edited_element_index << endl;

                    edit_buffers.assign(1, "");
                    active_edit_box = 0;
                    SDL_StartTextInput();

                    cout << "Label placed. Enter name." << endl;
                    break;
                }
            }
            if (clicked_on_component) break;
        }
    }

    return true;
}

bool graphical_view::handle_toolbar_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        if (mouse_pos.y < 40) 
        { 
            for (const auto& button : toolbar_buttons)
            {
                if (SDL_PointInRect(&mouse_pos, &button.rect))
                {
                    switch (button.action) 
                    {
                        case Tool_Bar_Action::New_File:
                            new_file = true;
                            is_labeling = false;
                            is_wiring = false;
                            elements_menu = false;
                            is_grounding = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;

                            if (current_file_name != "" && current_file_address != "")
                            {
                                C->circuit->change_name(current_file_name);
                                C->saveCircuit(C->circuit, current_file_address);
                            }
                            C->New_File();
                            break;
                        case Tool_Bar_Action::Wire:
                            is_wiring = !is_wiring;
                            is_labeling = false;
                            elements_menu = false;
                            is_grounding = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Net_Label:
                            is_labeling = !is_labeling;
                            is_wiring = false;
                            elements_menu = false;
                            is_grounding = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Grid:
                            show_grids = !show_grids;
                            break;
                        case Tool_Bar_Action::Components_Menu:
                            elements_menu = !elements_menu;
                            is_wiring = false;
                            is_labeling = false;
                            is_grounding = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;

                            if (elements_menu)
                            {
                                initialize_menu();
                                initialize_SubC_menu(C);
                            }
                            break;
                        case Tool_Bar_Action::File:
                            is_file_menu_open = !is_file_menu_open;
                            is_configuring_analysis = false;
                            elements_menu = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            is_grounding = false;
                            is_saving = false;
                            sub_circuit_menu = false;
                            is_deleting = false;
                            break;
                        case Tool_Bar_Action::Configure_Analysis:
                            is_configuring_analysis = !is_configuring_analysis;

                            if (is_configuring_analysis)
                            {
                                initialize_configure_analysis_menu(C);
                            }
                            else
                            {
                                SDL_StopTextInput();
                            }
                            elements_menu = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            is_grounding = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Run:
                            if (current_analysis_mode == Analysis_Mode::Transient)
                            {
                                C->do_transient();
                            }
                            else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
                            {
                                double s, e, n;
                                C->get_ac_params(s, e, n, AC_sweep_type);
                                cout << s << " " << e << n << (int)AC_sweep_type;
                                C->performACSweep(C->circuit);
                            }
                            else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
                            {
                                double s, e, b, num;
                                C->get_phase_params(s, e, b, num, phase_sweep_type);
                                C->performPhaseSweep(C->circuit);
                            }
                            feedback_cursor_state = Feedback_Cursor_State::Loading;
                            feedback_cursor_timer_start = SDL_GetTicks();

                            if (plot_view)
                            {
                                plot_view->delete_all_signals();
                            }
                            break;
                        case Tool_Bar_Action::Probe:
                            probe_mode = !probe_mode;
                            math_operation_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            elements_menu = false;
                            is_grounding = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Math_Operation:
                            math_operation_mode = !math_operation_mode;
                            probe_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            elements_menu = false;
                            is_grounding = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Save:
                            is_saving = !is_saving;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            elements_menu = false;
                            is_grounding = false;
                            is_file_menu_open = false;
                            is_deleting = false;

                            edit_buffers.clear();
                            edit_buffers.resize(2);
                            edit_buffers[0] = current_file_name;
                            edit_buffers[1] = current_file_address;
                            break;
                        case Tool_Bar_Action::Delete:
                            is_deleting = !is_deleting;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_wiring = false;
                            is_labeling = false;
                            elements_menu = false;
                            is_grounding = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            sub_circuit_menu = false;
                            break;
                        case Tool_Bar_Action::Sub_Circuit:
                            sub_circuit_menu = !sub_circuit_menu;
                            is_labeling = false;
                            elements_menu = false;
                            is_wiring = false;
                            is_grounding = false;
                            math_operation_mode = false;
                            probe_mode = false;
                            is_saving = false;
                            is_file_menu_open = false;
                            is_deleting = false;

                            initialize_SubC_menu(C);
                            break;
                    }
                    return true;
                }
            }
        }
    }
    return false;
}

bool graphical_view::handle_configure_analysis_events(SDL_Event &event, Controller *C)
{
    auto save_and_exit = [&]() {
        try {
            switch (current_analysis_mode) {
                case Analysis_Mode::Transient:
                    if (edit_buffers.size() >= 3)
                    {
                        C->set_transient_values(toValue(edit_buffers[2]), toValue(edit_buffers[1]) + toValue(edit_buffers[2]), toValue(edit_buffers[0]), toValue(edit_buffers[2]));
                    }
                    break;
                case Analysis_Mode::AC_Sweep:
                    if (edit_buffers.size() >= 3)
                    {
                        double start_freq = toValue(edit_buffers[0]);
                        double stop_freq = toValue(edit_buffers[1]);
                        int num_points = static_cast<int>(toValue(edit_buffers[2]));
                        C->set_AC_sweep_variables(start_freq, stop_freq, num_points, AC_sweep_type);
                    }
                    break;
                case Analysis_Mode::Phase_Sweep:
                    if (edit_buffers.size() >= 4)
                    {
                        double base_phase = toValue(edit_buffers[0]);
                        double start_phase = toValue(edit_buffers[1]);
                        double stop_phase = toValue(edit_buffers[2]);
                        int num_points = static_cast<int>(toValue(edit_buffers[3]));
                        C->set_phase_sweep_variables(start_phase, stop_phase, base_phase, num_points, phase_sweep_type);
                    }
                    break;
            }
        }
        catch (const exception& e)
        {
            cerr << "Error" << e.what() << endl;
        }
        is_configuring_analysis = false;
        SDL_StopTextInput();
    };

    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        // handling tab clicks
        auto switch_tab = [&](Analysis_Mode new_mode, int buffer_size) {
            if (current_analysis_mode != new_mode)
            {
                current_analysis_mode = new_mode;
                active_edit_box = -1;
                edit_buffers.assign(buffer_size, "");
            }
        };

        if (SDL_PointInRect(&mouse_pos, &transient_tab_rect))
        {
            switch_tab(Analysis_Mode::Transient, 3);
            return true;
        }
        if (SDL_PointInRect(&mouse_pos, &ac_sweep_tab_rect))
        {
            switch_tab(Analysis_Mode::AC_Sweep, 3);
            return true;
        }
        if (SDL_PointInRect(&mouse_pos, &phase_sweep_tab_rect))
        {
            switch_tab(Analysis_Mode::Phase_Sweep, 4);
            return true;
        }

        // handling AC sweep type clicks
        if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            if (SDL_PointInRect(&mouse_pos, &octave_button_rect))
            {
                AC_sweep_type = AC_Sweep_Type::Octave;
                cout << "CLICKED Octave: AC_sweep_type is now " << (int)AC_sweep_type << endl;
                return true;
            }
            if (SDL_PointInRect(&mouse_pos, &decade_button_rect))
            {
                AC_sweep_type = AC_Sweep_Type::Decade;
                cout << "CLICKED Decade: AC_sweep_type is now " << (int)AC_sweep_type << endl;
                return true;
            }
            if (SDL_PointInRect(&mouse_pos, &linear_button_rect))
            {
                AC_sweep_type = AC_Sweep_Type::Linear;
                cout << "CLICKED Linear: AC_sweep_type is now " << (int)AC_sweep_type << endl;
                return true;
            }
        }

        // textbox OK cancel
        if (SDL_PointInRect(&mouse_pos, &ok_button_rect))
        {
            save_and_exit();
        }
        else if (SDL_PointInRect(&mouse_pos, &cancel_button_rect))
        {
            is_configuring_analysis = false;
            SDL_StopTextInput();
        }
        else
        {
            bool box_clicked = false;
            for (int i = 0; i < property_rects.size(); ++i)
            {
                if (SDL_PointInRect(&mouse_pos, &property_rects[i]))
                {
                    active_edit_box = i;
                    SDL_StartTextInput();
                    box_clicked = true;
                    break;
                }
            }
            if (!box_clicked)
            {
                active_edit_box = -1;
                SDL_StopTextInput();
            }
        }
    }

    // keyboard input
    if (event.type == SDL_TEXTINPUT && active_edit_box != -1)
    {
        edit_buffers[active_edit_box] += event.text.text;
    }
    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                is_configuring_analysis = false;
                SDL_StopTextInput();
                break;
            case SDLK_BACKSPACE:
                if (active_edit_box != -1 && !edit_buffers[active_edit_box].empty())
                {
                    edit_buffers[active_edit_box].pop_back();
                }
                break;
            case SDLK_RETURN:
                save_and_exit();
                break;
        }
    }
    return true;
}

bool graphical_view::handle_probing_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
        probe_mode = false;
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    bool ctrl_is_pressed = keystates[SDL_SCANCODE_LCTRL] || keystates[SDL_SCANCODE_RCTRL];


    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && !ctrl_is_pressed)
    {
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            Node* target_node = find_node_at({event.button.x, event.button.y}, C);
            if (target_node)
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal node_signal;
                node_signal.name = "V(" + target_node->get_name() + ")";

                for (int i = 0; i < target_node->get_all_voltages().size(); i++)
                    node_signal.data_points.push_back({-1 * target_node->get_all_voltages()[i].first, target_node->get_all_voltages()[i].second});

                node_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(node_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::V);
                plot_view->set_x_unit(Unit::s);
                probe_mode = false;
            }
            Graphical_Element* target_element = find_element_at({event.button.x, event.button.y}, C);
            if (target_element)
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal element_signal;
                element_signal.name = "V(" + target_element->get_model()->get_name() + ")";

                if (current_analysis_mode == Analysis_Mode::Transient)
                {
                    double start_time, stop_time, time_step;
                    C->get_tran_params(start_time, stop_time, time_step);


                    if (time_step > 0)
                    {
                        for (double time = start_time; time < stop_time; time += time_step)
                        {
                            double voltage = target_element->get_model()->get_voltage_at_time(time);
                            element_signal.data_points.push_back({voltage, time});
                        }
                    }
                }

                element_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(element_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::V);
                plot_view->set_x_unit(Unit::s);
                probe_mode = false;
            }
        }
        else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            Node* target_node = find_node_at({event.button.x, event.button.y}, C);
            if (target_node && !target_node->is_the_node_ground())
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal node_signal;
                node_signal.name = "Mag(" + target_node->get_name() + ")";

                vector<double> freq, mag;

                for (const auto& data : C->circuit->getAcVoltage())
                {
                    if (target_node == data.first)
                    {
                        freq = get<0>(data.second);
                        mag = get<1>(data.second);
                        break;
                    }
                }

                for (int i = 0; i < min(freq.size(), mag.size()); i++)
                {
                    node_signal.data_points.push_back({mag[i], freq[i]});
                }

                node_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(node_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::dB);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Linear) plot_view->set_x_unit(Unit::Hz);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Decade) plot_view->set_x_unit(Unit::Dec);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Octave) plot_view->set_x_unit(Unit::Oct);
                probe_mode = false;
            }
        }
        else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            Node* target_node = find_node_at({event.button.x, event.button.y}, C);
            if (target_node && !target_node->is_the_node_ground())
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal node_signal;
                node_signal.name = "Mag(" + target_node->get_name() + ")";

                vector<double> p, mag;

                for (const auto& data : C->circuit->getPhaseVoltage())
                {
                    if (target_node == data.first)
                    {
                        p = get<0>(data.second);
                        mag = get<1>(data.second);
                        break;
                    }
                }

                for (int i = 0; i < min(p.size(), mag.size()); i++)
                {
                    node_signal.data_points.push_back({mag[i], p[i]});
                }

                node_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(node_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::dB);
                plot_view->set_x_unit(Unit::Hz);
                probe_mode = false;
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT)
    {
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            Graphical_Element* target_element = find_element_at({event.button.x, event.button.y}, C);
            if (target_element)
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal element_signal;
                element_signal.name = "I(" + target_element->get_model()->get_name() + ")";

                if (current_analysis_mode == Analysis_Mode::Transient)
                {
                    double start_time, stop_time, time_step;
                    C->get_tran_params(start_time, stop_time, time_step);


                    if (time_step > 0)
                    {
                        for (double time = start_time; time < stop_time; time += time_step)
                        {
                            double current = target_element->get_model()->get_current(time, time_step);
                            element_signal.data_points.push_back({current, time});
                        }
                    }
                }

                element_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(element_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::A);
                plot_view->set_x_unit(Unit::s);
                probe_mode = false;
            }
        }
        else if (current_analysis_mode == Analysis_Mode::AC_Sweep)
        {
            Node* target_node = find_node_at({event.button.x, event.button.y}, C);
            if (target_node && !target_node->is_the_node_ground())
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal node_signal;
                node_signal.name = "Phase(" + target_node->get_name() + ")";


                vector<double> freq, phase;

                for (const auto& data : C->circuit->getAcVoltage())
                {
                    if (target_node == data.first)
                    {
                        freq = get<0>(data.second);
                        phase = get<2>(data.second);
                        break;
                    }
                }

                for (int i = 0; i < min(freq.size(), phase.size()); i++)
                {
                    node_signal.data_points.push_back({phase[i], freq[i]});
                }


                node_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(node_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::deg);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Linear) plot_view->set_x_unit(Unit::Hz);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Decade) plot_view->set_x_unit(Unit::Dec);
                if (C->get_AC_sweep_t() == AC_Sweep_Type::Octave) plot_view->set_x_unit(Unit::Oct);
                probe_mode = false;

            }
        }
        else if (current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            Node* target_node = find_node_at({event.button.x, event.button.y}, C);
            if (target_node && !target_node->is_the_node_ground())
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal node_signal;
                node_signal.name = "Phase(" + target_node->get_name() + ")";


                vector<double> p, phase;

                for (const auto& data : C->circuit->getPhaseVoltage())
                {
                    if (target_node == data.first)
                    {
                        p = get<0>(data.second);
                        phase = get<2>(data.second);
                        break;
                    }
                }

                for (int i = 0; i < min(p.size(), phase.size()); i++)
                {
                    node_signal.data_points.push_back({phase[i], p[i]});
                }


                node_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(node_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::deg);
                plot_view->set_x_unit(Unit::deg);
                probe_mode = false;

            }
        }


    }

    // show power
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && ctrl_is_pressed)
    {
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            Graphical_Element* target_element = find_element_at({event.button.x, event.button.y}, C);
            if (target_element)
            {
                if (!plot_view)
                {
                    plot_view = make_unique<Plot_View>();
                }

                // create and add the signal
                Signal element_signal;
                element_signal.name = "Pow(" + target_element->get_model()->get_name() + ")";

                if (current_analysis_mode == Analysis_Mode::Transient)
                {
                    double start_time, stop_time, time_step;
                    C->get_tran_params(start_time, stop_time, time_step);


                    if (time_step > 0)
                    {
                        for (double time = start_time; time < stop_time; time += time_step)
                        {
                            double voltage = target_element->get_model()->get_voltage_at_time(time);
                            double current = target_element->get_model()->get_current(time, time_step);
                            double power = voltage * current;
                            element_signal.data_points.push_back({power, time});
                        }
                    }
                }

                element_signal.color = default_colors[color_index % default_colors.size()];
                color_index++;
                if (color_index == 15)
                    color_index = 0;

                plot_view->add_signal(element_signal);
                plot_view->auto_zoom();
                plot_view->set_y_unit(Unit::W);
                plot_view->set_x_unit(Unit::s);
                probe_mode = false;
            }
        }
    }

    return true;
}

bool graphical_view::handle_math_operation_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
        math_operation_mode = false;
    }

    if (is_editing_constant)
    {
        if (event.type == SDL_TEXTINPUT && (isdigit(event.text.text[0]) || event.text.text[0] == '-'))
        {
            math_constant_buffer += event.text.text;
        }
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && !math_constant_buffer.empty())
        {
            math_constant_buffer.pop_back();
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};
        bool control_was_clicked = false;

        // click on constant box
        if (SDL_PointInRect(&mouse_pos, &constant_textbox_rect))
        {
            is_editing_constant = true;
            SDL_StartTextInput();
            control_was_clicked = true;
        }

        // click on element list
        for (int i = 0; i < math_element_buttons.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &math_element_buttons[i]))
            {
                math_selected_element_index = i;
                math_selected_node_index = -1;
                control_was_clicked = true;
                break;
            }
        }

        // click on node list
        for (int i = 0; i < math_node_buttons.size(); ++i) {
            if (SDL_PointInRect(&mouse_pos, &math_node_buttons[i])) {
                math_selected_node_index = i;
                math_selected_element_index = -1;
                control_was_clicked = true;
                break;
            }
        }

        // click on signal type
        if (current_analysis_mode == Analysis_Mode::Transient)
        {
            if (SDL_PointInRect(&mouse_pos, &v_button_rect))
            {
                math_selected_signal_type = Signal_Type::Voltage;
                return true;
            }
            if (SDL_PointInRect(&mouse_pos, &i_button_rect))
            {
                math_selected_signal_type = Signal_Type::Current;
                return true;
            }
            if (SDL_PointInRect(&mouse_pos, &p_button_rect))
            {
                math_selected_signal_type = Signal_Type::Power;
                return true;
            }
        }
        else if (current_analysis_mode == Analysis_Mode::AC_Sweep || current_analysis_mode == Analysis_Mode::Phase_Sweep)
        {
            if (SDL_PointInRect(&mouse_pos, &mag_button_rect))
            {
                math_selected_signal_type = Signal_Type::Mag;
                return true;
            }
            if (SDL_PointInRect(&mouse_pos, &phase_button_rect))
            {
                math_selected_signal_type = Signal_Type::Phase;
                return true;
            }
        }

        // click on add or subtract
        bool term_ready = (math_selected_element_index != -1 || math_selected_node_index != -1);
        if (term_ready) {
            if (SDL_PointInRect(&mouse_pos, &op_plus_button))
            {
                add_math_term(false, C);
                control_was_clicked = true;
            } else if (SDL_PointInRect(&mouse_pos, &op_minus_button))
            {
                add_math_term(true, C);
                control_was_clicked = true;
            }
        }

        // clear
        if (SDL_PointInRect(&mouse_pos, &op_clear_button))
        {
            math_terms.clear();
            math_expression_string.clear();
            math_constant_buffer = "1.0";
            math_selected_element_index = -1;
            control_was_clicked = true;
        }

        // execute
        if (SDL_PointInRect(&mouse_pos, &op_execute_button) && !math_terms.empty())
        {
            execute_math_operation();
            control_was_clicked = true;
        }

        // click out of active control box
        if (!control_was_clicked)
        {
            is_editing_constant = false;
            SDL_StopTextInput();
        }
    }

    return true;
}

bool graphical_view::handle_saving_events(SDL_Event &event, Controller *C)
{
    // helper
    auto save_and_exit = [&]() {
        current_file_name = edit_buffers[0];
        current_file_address = edit_buffers[1];

        C->circuit->change_name(current_file_name);
        C->saveGraphicalCircuit(C->circuit, current_file_address);


        is_saving = false;
        SDL_StopTextInput();
        C->getFile_Handler().saveFiles();
    };

    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        // OK or Cancel
        if (SDL_PointInRect(&mouse_pos, &ok_button_rect))
        {
            // OK
            save_and_exit();
        }
        else if (SDL_PointInRect(&mouse_pos, &cancel_button_rect))
        {
            // Cancel
            is_saving = false;
            SDL_StopTextInput();
        }
        else
        {
            // text box
            bool an_edit_box_was_clicked = false;
            for (int i = 0; i < property_rects.size(); ++i)
            {
                if (SDL_PointInRect(&mouse_pos, &property_rects[i]))
                {
                    active_edit_box = i;
                    SDL_StartTextInput();
                    an_edit_box_was_clicked = true;
                    break;
                }
            }
            if (!an_edit_box_was_clicked)
            {
                active_edit_box = -1;
                SDL_StopTextInput();
            }
        }
    }

    if (event.type == SDL_TEXTINPUT && active_edit_box != -1)
    {
        edit_buffers[active_edit_box] += event.text.text;
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                is_saving = false;
                SDL_StopTextInput();
                break;

            case SDLK_BACKSPACE:
                if (active_edit_box != -1 && !edit_buffers[active_edit_box].empty())
                {
                    edit_buffers[active_edit_box].pop_back();
                }
                break;

            case SDLK_RETURN:
                save_and_exit();
                break;

            case SDLK_v:
            {
                SDL_Keymod modState = SDL_GetModState();
                if (modState & KMOD_CTRL)
                {
                    if (active_edit_box != -1)
                    {
                        char* clipboard_text = SDL_GetClipboardText();
                        if (clipboard_text)
                        {
                            edit_buffers[active_edit_box] += clipboard_text;
                            SDL_free(clipboard_text);
                        }
                    }
                }
                break;
            }
        }
    }

    return true;
}

bool graphical_view::handle_deleting_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
    {
        is_deleting = false;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point pos = {event.button.x, event.button.y};
        Graphical_Element* element = find_element_at(pos, C);
        if (element)
        {
            if (dynamic_cast<Graphical_Net_Label*>(element)) return true;
            if (dynamic_cast<Graphical_Ground*>(element)) dynamic_cast<Graphical_Ground*>(element)->get_node()->return_to_normal();
            C->delete_element(element);
            is_deleting = false;
        }
        Graphical_Wire* wire = find_wire_at(pos, C);
        if (wire)
        {
            C->delete_wire(wire);
            is_deleting = false;
        }
    }
    return true;
}

bool graphical_view::handle_file_menu_events(SDL_Event &event, Controller *C)
{
    // helper
    auto execute_and_exit = [&]() {
        if (selected_file_index != -1)
            C->load_file(C->get_file_names()[selected_file_index]);
        is_file_menu_open = false;
    };

    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN && (event.key.keysym.sym == SDLK_ESCAPE))
    {
        return true;
    }

    // mouse hover effect
    if (event.type == SDL_MOUSEMOTION)
    {
        SDL_Point mouse_pos = {event.motion.x, event.motion.y};
        hovered_file_index = -1;
        for (int i = 0; i < file_button_rects.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &file_button_rects[i]))
            {
                hovered_file_index = i;
                break;
            }
        }
    }

    // handle clicks
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        // click on file
        for (int i = 0; i < file_button_rects.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &file_button_rects[i]))
            {
                selected_file_index = i;
                return true;
            }
        }

        if (SDL_PointInRect(&mouse_pos, &file_ok_button_rect))
        {
            if (selected_file_index != -1)
            {
                vector<string> file_names = C->get_file_names();
                string selected_file = file_names[selected_file_index];

                execute_and_exit();

                cout << "OK clicked! Opening file: " << selected_file << endl;

                is_file_menu_open = false;
            }
            return true;
        }

        if (SDL_PointInRect(&mouse_pos, &file_cancel_button_rect))
        {
            is_file_menu_open = false;
            return true;
        }
    }

    return true;
}

bool graphical_view::handle_SubC_menu_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE)
        {
            sub_circuit_menu = false;
        }
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        SDL_Point mouse_pos = {event.motion.x, event.motion.y};
        hovered_SubC_item_index = -1;

        for (int i = 0; i < SubC_menu_items.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &SubC_menu_items[i].rect))
            {
                hovered_SubC_item_index = i;
                break;
            }
        }
    }


    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        if (SDL_PointInRect(&mouse_pos, &create_new_SubC_button_rect))
        {
            naming_SubC_menu_active = true;
            sub_circuit_menu = false;
        }

        bool clicked_on_item = false;
        for (int i = 0; i < SubC_menu_items.size(); ++i)
        {
            if (SDL_PointInRect(&mouse_pos, &SubC_menu_items[i].rect))
            {
                cout << "detected click on item" << endl;
                clicked_on_item = true;

                // click
                if (event.button.clicks == 1)
                {
                    selected_SubC_menu_item_index = i;
                }
                // double click
                else if (event.button.clicks == 2)
                {
                    selected_SubC_menu_item_index = i;
                    cout << "detected double click" << endl;
                    int mouseX, mouseY;
                    SDL_GetMouseState(&mouseX, &mouseY);

                    // adding the component

                    C->add_Graphical_Sub_Circuit(mouseX, mouseY, SubC_menu_items[i].name);

                    sub_circuit_menu = false;
                    selected_SubC_menu_item_index = -1;
                }
                break;
            }
        }
    }
    return true;
}

bool graphical_view::handle_SubC_creation_events(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE)
        {
            sub_circuit_menu = false;
            node1 = nullptr;
            node2 = nullptr;
        }
    }

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point pos = {event.button.x, event.button.y};

        Graphical_Element* element = find_element_at(pos, C);
        if (element)
        {
            int CP_index = find_connection_point_at(element, pos);
            if (node1 == nullptr) node1 = element->get_connection_points()[CP_index].node;
            else node2 = element->get_connection_points()[CP_index].node;
        }
        else
        {
            Graphical_Wire* wire = find_wire_at(pos, C);
            if (wire)
            {
                if (node1 == nullptr) node1 = wire->start_node;
                else node2 = wire->start_node;
            }
        }
    }

    if (node1 == node2)
        node2 = nullptr;

    if (node1 != nullptr && node2 != nullptr)
    {
        cout << "selected 2 points" << endl;

        //cout << node1->get_name() << " " << node2->get_name() << " pkfjpigjrpfirejfpijfpe" << endl;
        C->addSubCircuit(new_SubC_name,C->circuit, node1, node2);
        //C->saveGraphicalSubCircuits();
        C->saveGraphicalSubCircuit(C->subCircuits.back(), "D://Project_Files//subcircuits/");

        new_SubC_name = "";
        node1 = nullptr;
        node2 = nullptr;
        create_SubC_mode = false;
    }

    return true;
}

bool graphical_view::handle_text_input_menu_events(SDL_Event& event)
{
    auto process_and_exit = [&]() {
        cout << "Text entered: " << text_input_buffer << endl;
        new_SubC_name = text_input_buffer;
        naming_SubC_menu_active = false;
        SDL_StopTextInput();
    };

    if (event.type == SDL_QUIT)
        throw Quit_Event_Exception();

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        if (SDL_PointInRect(&mouse_pos, &text_input_box_rect))
        {
            SDL_StartTextInput();
        }
        else if (SDL_PointInRect(&mouse_pos, &text_input_ok_rect))
        {
            create_SubC_mode = true;
            process_and_exit();
        }
        else if (SDL_PointInRect(&mouse_pos, &text_input_cancel_rect))
        {
            naming_SubC_menu_active = false;
            SDL_StopTextInput();
        }
        else
        {
            SDL_StopTextInput();
        }
    }

    // handle text entry
    if (event.type == SDL_TEXTINPUT)
    {
        text_input_buffer += event.text.text;
    }

    if (event.type == SDL_KEYDOWN)
    {
        switch (event.key.keysym.sym)
        {
            case SDLK_ESCAPE:
                naming_SubC_menu_active = false;
                SDL_StopTextInput();
                break;
            case SDLK_BACKSPACE:
                if (!text_input_buffer.empty())
                {
                    text_input_buffer.pop_back();
                }
                break;
            case SDLK_RETURN:
                create_SubC_mode = true;
                process_and_exit();
                break;
        }
    }
    return true;
}