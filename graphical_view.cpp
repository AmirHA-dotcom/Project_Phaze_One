//
// Created by amirh on 2025-07-20.
//

#include "graphical_view.h"

// helper functions

const char* FONT = "D:/Fonts/Roboto/static/Roboto-Regular.ttf";

string rotation_to_string(Rotation r) 
{
    switch (r) {
        case Rotation::Right: return "Right";
        case Rotation::Left:  return "Left";
        case Rotation::Up:    return "Up";
        case Rotation::Down:  return "Down";
        default:              return "Unknown/Error";
    }
}

inline SDL_Point snap_to_grid(int x, int y, int grid_size)
{
    int snapped_x = round((float)x / grid_size) * grid_size;
    int snapped_y = round((float)y / grid_size) * grid_size;
    return {snapped_x, snapped_y};
}

void graphical_view::draw_grid(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    for (int x = 0; x < m_window_width; x += GRID_SIZE) {
        SDL_RenderDrawLine(renderer, x, 0, x, m_window_height);
    }
    for (int y = 0; y < m_window_height; y += GRID_SIZE) {
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
}

void graphical_view::draw_component_menu(SDL_Renderer* renderer, TTF_Font* font) {
    // fade
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 180);
//    SDL_RenderFillRect(renderer, NULL);

    SDL_Rect menu_panel = {200, 100, 800, 500};
    SDL_Rect preview_panel = {menu_panel.x + 550, menu_panel.y + 50, 200, 200};

    // panels
    SDL_SetRenderDrawColor(renderer, 50, 58, 69, 255);
    SDL_RenderFillRect(renderer, &menu_panel);
    SDL_SetRenderDrawColor(renderer, 236, 239, 244, 255);
    SDL_RenderFillRect(renderer, &preview_panel);

    // items
    int start_y = menu_panel.y + 60;
    for (int i = 0; i < menu_items.size(); ++i)
    {
        menu_items[i].rect = {menu_panel.x + 20, start_y + (i * 30), 200, 25};
        render_text(renderer, font, menu_items[i].name, menu_items[i].rect.x, menu_items[i].rect.y, {200, 200, 200, 255});
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
                preview.draw(renderer);
                break;
            }
            case Element_Type::Capacitor:
            {
                Graphical_Capacitor preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer);
                break;
            }
            case Element_Type::Inductor:
            {
                Graphical_Inductor preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer);
                break;
            }
            case Element_Type::Current_Source:
            {
                Graphical_Current_Source preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer);
                break;
            }
            case Element_Type::Real_Diode:
            {
                Graphical_Real_Diode preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer);
                break;
            }
            case Element_Type::Zener_Diode:
            {
                Graphical_Zener_Diode preview(nullptr);
                preview.bounding_box = preview_panel;
                preview.draw(renderer);
                break;
            }
        }
    }
}

void graphical_view::draw_properties_menu(SDL_Renderer* renderer, TTF_Font* font, Controller* C)
{
    const SDL_Color PANEL_BG = {50, 58, 69, 255};
    const SDL_Color TEXT_COLOR = {211, 211, 211, 255};
    const SDL_Color TEXT_BOX_BG = {33, 37, 41, 255};
    const SDL_Color BORDER_COLOR = {80, 88, 99, 255};
    const SDL_Color HIGHLIGHT_COLOR = {52, 152, 219, 255};

//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
//    SDL_SetRenderDrawColor(renderer, 20, 20, 20, 180);
//    SDL_RenderFillRect(renderer, NULL);
//    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

    int menu_width = 400;
    int menu_height = 300;
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

// main functions

bool graphical_view::run(Controller *C)
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

    TTF_Font* font = TTF_OpenFont(FONT , 16);
    if (!font)
    {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
    }
    Graphical_Element::set_font(font);

    initialize_menu();

    SDL_Window* window = SDL_CreateWindow(
            "AHA & AS",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            1280,
            720,
            SDL_WINDOW_RESIZABLE
    );

    if (!window)
    {
        SDL_Quit();
        throw runtime_error("Window could not be created! SDL_Error: " + string(SDL_GetError()));
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw runtime_error("Renderer could not be created! SDL_Error: " + string(SDL_GetError()));
    }

    SDL_GetWindowSize(window, &m_window_width, &m_window_height);

    bool running = true;
    SDL_Event event;


    while (running)
    {
        auto& graphical_elements = C->get_graphical_elements();
        auto& graphical_wires = C->get_graphical_wires();

        while (SDL_PollEvent(&event) != 0)
        {
            if (elements_menu)
            {
                 running = handle_menu_events(event, C);
            }
            else if (editing)
            {
                running = handle_edit_properties_menu(event, C);
            }
            else if (m_is_wiring)
            {
                running = handle_wiring_events(event, C);
            }
            else
            {
                running = handle_events(event, C);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        draw_grid(renderer);

        for (const auto& element : graphical_elements)
        {
            element->draw(renderer);
        }

        for (const auto& wire : graphical_wires)
        {
            wire->draw(renderer);
        }

//        if (new_wire_points.empty() && m_is_wiring)
//            cout << "no wires to draw" << endl;

        if (m_is_wiring && !new_wire_points.empty())
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point snapped_mouse = snap_to_grid(mouseX, mouseY, GRID_SIZE);

            SDL_Point last_point = new_wire_points.back().pos;

            //SDL_Point corner_point = {snapped_mouse.x, last_point.y};

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

            SDL_RenderDrawLine(renderer, mouseX, mouseY, new_wire_points.front().pos.x - 5, new_wire_points.front().pos.y - 5);

//            SDL_RenderDrawLine(renderer, last_point.x, last_point.y, corner_point.x, corner_point.y);
//            SDL_RenderDrawLine(renderer, corner_point.x, corner_point.y, snapped_mouse.x, snapped_mouse.y);
        }

        if (elements_menu)
        {
            draw_component_menu(renderer, font);
        }

        if (editing)
        {
            draw_properties_menu(renderer, font, C);
        }
        SDL_RenderPresent(renderer);
    }

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
        return false;
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
                    if (m_is_dragging)
                    {
                        cout << "Ctrl+R was pressed while dragging!" << endl;
                        auto& graphical_elements = C->get_graphical_elements();
                        auto& element_to_rotate = graphical_elements[m_dragged_element_index];
                        element_to_rotate->change_rotation();
                    }
                }
                else
                {
                    SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                    cout << "R key was pressed." << endl;
                    C->add_Graphical_Resistor(snapped_pos.x, snapped_pos.y);
                }
                break;
            }

            case SDLK_c:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                cout << "C key was pressed." << endl;
                C->add_Graphical_Capacitor(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_l:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                cout << "L key was pressed." << endl;
                C->add_Graphical_Inductor(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_s:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                cout << "S key was pressed." << endl;
                C->add_Graphical_Current_Source(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_d:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                cout << "D key was pressed." << endl;
                C->add_Graphical_Real_Diode(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_z:
            {
                SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);

                cout << "Z key was pressed." << endl;
                C->add_Graphical_Zener_Diode(snapped_pos.x, snapped_pos.y);
                break;
            }

            case SDLK_p:
            {
                cout << "P key was pressed." << endl;
                elements_menu = !elements_menu;
                break;
            }

            case SDLK_w:
            {
                m_is_wiring = !m_is_wiring;
                new_wire_points.clear();
                if (m_is_wiring)
                {
                    cout << "Entered wiring Mode." << endl;
                }
                else
                {
                    cout << "Exited wiring Mode." << endl;
                }
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
                    m_is_dragging = true;
                    m_dragged_element_index = i;
                    m_drag_offset.x = mouse_pos.x - graphical_elements[i]->bounding_box.x;
                    m_drag_offset.y = mouse_pos.y - graphical_elements[i]->bounding_box.y;
                    break;
                }
            }
        }
        else if (event.button.button == SDL_BUTTON_RIGHT)
        {
            for (int i = graphical_elements.size() - 1; i >= 0; --i)
            {
                if (SDL_PointInRect(&mouse_pos, &graphical_elements[i]->bounding_box))
                {
                    cout << "Right-clicked on: " << graphical_elements[i]->get_model()->get_name() << endl;

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
        m_is_dragging = false;
        m_dragged_element_index = -1;
    }

    if (event.type == SDL_MOUSEMOTION && m_is_dragging)
    {
//        SDL_Point mouse_pos = {event.motion.x, event.motion.y};
//        graphical_elements[m_dragged_element_index]->bounding_box.x = mouse_pos.x - m_drag_offset.x;
//        graphical_elements[m_dragged_element_index]->bounding_box.y = mouse_pos.y - m_drag_offset.y;

        SDL_Point mouse_pos = {event.motion.x, event.motion.y};

        int new_x = mouse_pos.x - m_drag_offset.x;
        int new_y = mouse_pos.y - m_drag_offset.y;

        SDL_Point snapped_pos = snap_to_grid(new_x, new_y, GRID_SIZE);

        graphical_elements[m_dragged_element_index]->bounding_box.x = snapped_pos.x;
        graphical_elements[m_dragged_element_index]->bounding_box.y = snapped_pos.y;
    }

    return true;
}

bool graphical_view::handle_menu_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_QUIT) return false;

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_ESCAPE)
        {
            elements_menu = false;
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
                    }

                    elements_menu = false;
                    selected_menu_item_index = -1;
                }
                break;
            }
        }
    }
    return true;
}

bool graphical_view::handle_edit_properties_menu(SDL_Event &event, Controller *C)
{
    if (event.type == SDL_QUIT) return false;

    if (event.type == SDL_MOUSEBUTTONDOWN) {
        SDL_Point mouse_pos = {event.button.x, event.button.y};

        // OK or Cancel
        if (SDL_PointInRect(&mouse_pos, &ok_button_rect))
        {
            // OK
            C->update_element_properties(edited_element_index, edit_buffers);
            editing = false;
            SDL_StopTextInput();
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
            // (Note: the text box rects are defined in your draw_properties_menu function)
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

    // Handle text input, but only if a text box is active
    if (event.type == SDL_TEXTINPUT && active_edit_box != -1)
    {
        edit_buffers[active_edit_box] += event.text.text;
    }

    // Handle special keys
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
                C->update_element_properties(edited_element_index, edit_buffers);
                editing = false;
                SDL_StopTextInput();
                break;
        }
    }

    return true;
}

bool graphical_view::handle_wiring_events(SDL_Event& event, Controller* C)
{
    if (event.type == SDL_QUIT) return false;

    if (event.type == SDL_KEYDOWN)
    {
        if (event.key.keysym.sym == SDLK_w || event.key.keysym.sym == SDLK_ESCAPE)
        {
            m_is_wiring = false;
            cout << "Exiting wiring mode" << endl;
            new_wire_points.clear();
        }
    }

    // start wiring
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        if (new_wire_points.empty())
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point snapped_pos = snap_to_grid(mouseX, mouseY, GRID_SIZE);
            auto& elements = C->get_graphical_elements();

            // finding connection point
            for (auto& element : elements)
            {
                auto connection_points = element->get_connection_points();
                for (auto& point : connection_points)
                {
                    if (abs(snapped_pos.x - point.pos.x) < 20 && abs(snapped_pos.y - point.pos.y) < 20)
                    {
                        cout << element.get()->get_model()->get_name() << " is selected" << endl;
                        new_wire_points.push_back(point);
                        return true;
                    }
                }
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

            // see if its a valid point
            Connection_Point* target_point = nullptr;
            for (auto& element : elements)
            {
                auto connection_points = element->get_connection_points();
                for (auto& point : connection_points)
                {
                    if (abs(snapped_pos.x - point.pos.x) < 20 && abs(snapped_pos.y - point.pos.y) < 20)
                    {
                        target_point = &point;
                        cout << element.get()->get_model()->get_name() << " is selected" << endl;
                        break;
                    }
                }
                if (target_point) break;
            }

            // creating wire
//            if (target_point)
//            {
//                Node* start_node = new_wire_points.front().node;
//                Node* end_node = target_point->node;
//
//                // position and rotation
//                SDL_Point start_pos = new_wire_points.front().pos;
//                SDL_Point end_pos = target_point->pos;
//                Rotation start_orientation = new_wire_points.front().rotation;
//                Rotation end_orientation = target_point->rotation;
//
//                if (start_orientation == end_orientation)
//                {
//                    // h to h
//                    if (start_orientation == Rotation::Left || start_orientation == Rotation::Right)
//                    {
//                        int mid_x = start_pos.x + (end_pos.x - start_pos.x) / 2;
//                        SDL_Point corner1 = { mid_x, start_pos.y };
//                        SDL_Point corner2 = { mid_x, end_pos.y };
//                        new_wire_points.push_back({ corner1, nullptr, {} });
//                        new_wire_points.push_back({ corner2, nullptr, {} });
//                    }
//                    else
//                    {
//                        // V to V
//                        int mid_y = start_pos.y + (end_pos.y - start_pos.y) / 2;
//                        SDL_Point corner1 = { start_pos.x, mid_y };
//                        SDL_Point corner2 = { end_pos.x, mid_y };
//                        new_wire_points.push_back({ corner1, nullptr, {} });
//                        new_wire_points.push_back({ corner2, nullptr, {} });
//                    }
//                }
//                else
//                {
//                    if (start_orientation == Rotation::Left || start_orientation == Rotation::Right)
//                    {
//                        // h to v
//                        SDL_Point corner = { end_pos.x, start_pos.y };
//                        new_wire_points.push_back({ corner, nullptr, {} });
//                    }
//                    else
//                    {
//                        // v to h
//                        SDL_Point corner = { start_pos.x, end_pos.y };
//                        new_wire_points.push_back({ corner, nullptr, {} });
//                    }
//                }
//                new_wire_points.push_back(*target_point);
//
//                C->connect_nodes(start_node, end_node);
//                C->add_Graphical_Wire(new_wire_points);
//            }

            if (target_point)
            {
                Node* start_node = new_wire_points.front().node;
                Node* end_node = target_point->node;

                SDL_Point start_pos = new_wire_points.front().pos;
                SDL_Point end_pos = target_point->pos;
                Rotation start_rot = new_wire_points.front().rotation;
                Rotation end_rot = target_point->rotation;

                vector<SDL_Point> path_points;
                path_points.push_back(start_pos);

                SDL_Point c1, c2;

                bool is_start_horizontal = (start_rot == Rotation::Left || start_rot == Rotation::Right);

                if (is_start_horizontal)
                {
                    int mid_x = start_pos.x + (end_pos.x - start_pos.x) / 2;
                    c1 = { mid_x, start_pos.y };
                    c2 = { mid_x, end_pos.y };

                    if (start_rot == Rotation::Left && mid_x > start_pos.x) {
                        c1.x = c2.x = min(start_pos.x, end_pos.x) - 20;
                    } else if (start_rot == Rotation::Right && mid_x < start_pos.x) {
                        c1.x = c2.x = max(start_pos.x, end_pos.x) + 20;
                    }
                }
                else
                {
                    int mid_y = start_pos.y + (end_pos.y - start_pos.y) / 2;
                    c1 = { start_pos.x, mid_y };
                    c2 = { end_pos.x, mid_y };

                    if (start_rot == Rotation::Up && mid_y > start_pos.y) {
                        c1.y = c2.y = min(start_pos.y, end_pos.y) - 20;
                    } else if (start_rot == Rotation::Down && mid_y < start_pos.y) {
                        c1.y = c2.y = max(start_pos.y, end_pos.y) + 20;
                    }
                }

                // add corner1 if its not the same as the start
                if (c1.x != start_pos.x || c1.y != start_pos.y) {
                    path_points.push_back(c1);
                }
                // add corner2 if its not the same as the last point
                if (c2.x != path_points.back().x || c2.y != path_points.back().y) {
                    path_points.push_back(c2);
                }
                // add the end point if its not the same as the last point
                if (end_pos.x != path_points.back().x || end_pos.y != path_points.back().y) {
                    path_points.push_back(end_pos);
                }

                new_wire_points.clear();
                for(const auto& p : path_points) {
                    new_wire_points.push_back({ p, nullptr, {} });
                }

                new_wire_points.front().node = start_node;
                new_wire_points.front().rotation = start_rot;
                new_wire_points.back().node = end_node;
                new_wire_points.back().rotation = end_rot;


                C->connect_nodes(start_node, end_node);
                C->add_Graphical_Wire(new_wire_points);
            }


//            if (target_point)
//            {
//                Node* start_node = new_wire_points.front().node;
//                Node* end_node = target_point->node;
//
//                SDL_Point start_pos = new_wire_points.front().pos;
//                SDL_Point end_pos = target_point->pos;
//                Rotation start_rot = new_wire_points.front().rotation;
//                Rotation end_rot = target_point->rotation;
//
//                cout << "-------------------------\n";
//                cout << "CONNECTING WIRE:\n";
//                cout << "  > Start Port Orientation: " << rotation_to_string(start_rot) << "\n";
//                cout << "  > End Port Orientation:   " << rotation_to_string(end_rot) << "\n";
//                cout << "-------------------------\n";
//
//                vector<SDL_Point> path_points;
//                path_points.push_back(start_pos);
//
//                int stub_length = 20;
//
//                SDL_Point p_start_stub = start_pos;
//                switch (start_rot) {
//                    case Rotation::Right: p_start_stub.x += stub_length; break;
//                    case Rotation::Left:  p_start_stub.x -= stub_length; break;
//                    case Rotation::Down:  p_start_stub.y += stub_length; break;
//                    case Rotation::Up:    p_start_stub.y -= stub_length; break;
//                }
//
//                SDL_Point p_end_stub = end_pos;
//                switch (end_rot) {
//                    case Rotation::Right: p_end_stub.x -= stub_length; break;
//                    case Rotation::Left:  p_end_stub.x += stub_length; break;
//                    case Rotation::Down:  p_end_stub.y -= stub_length; break;
//                    case Rotation::Up:    p_end_stub.y += stub_length; break;
//                }
//
//                SDL_Point corner;
//                bool is_start_horizontal = (start_rot == Rotation::Left || start_rot == Rotation::Right);
//
//                SDL_Point corner_A = {p_end_stub.x, p_start_stub.y};
//                SDL_Point corner_B = {p_start_stub.x, p_end_stub.y};
//
//                if (is_start_horizontal) {
//                    corner = corner_A;
//                    if ((start_rot == Rotation::Left && corner.x > p_start_stub.x) || (start_rot == Rotation::Right && corner.x < p_start_stub.x)) {
//                        corner = corner_B;
//                    }
//                } else {
//                    corner = corner_B;
//                    if ((start_rot == Rotation::Up && corner.y > p_start_stub.y) || (start_rot == Rotation::Down && corner.y < p_start_stub.y)) {
//                        corner = corner_A;
//                    }
//                }
//
//                path_points.push_back(p_start_stub);
//                path_points.push_back(corner);
//                path_points.push_back(p_end_stub);
//                path_points.push_back(end_pos);
//
//                vector<SDL_Point> final_path;
//                if (!path_points.empty()) {
//                    final_path.push_back(path_points[0]);
//                    for (size_t i = 1; i < path_points.size(); ++i) {
//                        if (path_points[i].x == final_path.back().x && path_points[i].y == final_path.back().y) continue;
//                        if (final_path.size() > 1) {
//                            SDL_Point& p1 = final_path[final_path.size() - 2];
//                            SDL_Point& p2 = final_path.back();
//                            SDL_Point& p3 = path_points[i];
//                            if ((p1.x == p2.x && p2.x == p3.x) || (p1.y == p2.y && p2.y == p3.y)) {
//                                final_path.back() = p3;
//                                continue;
//                            }
//                        }
//                        final_path.push_back(path_points[i]);
//                    }
//                }
//
//                new_wire_points.clear();
//                for(const auto& p : final_path) {
//                    new_wire_points.push_back({ p, nullptr, {} });
//                }
//                new_wire_points.front().node = start_node;
//                new_wire_points.front().rotation = start_rot;
//                new_wire_points.back().node = end_node;
//                new_wire_points.back().rotation = end_rot;
//
//                C->connect_nodes(start_node, end_node);
//                C->add_Graphical_Wire(new_wire_points);
//            }

            new_wire_points.clear();
        }
    }

    return true;
}