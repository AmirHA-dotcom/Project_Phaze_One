//
// Created by amirh on 2025-07-20.
//

#include "graphical_view.h"

// helper functions



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
    SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
    SDL_RenderFillRect(renderer, &menu_panel);
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
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

    TTF_Font* font = TTF_OpenFont("D:/Fonts/Roboto/static/Roboto-Regular.ttf", 16);
    if (!font)
    {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
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

        while (SDL_PollEvent(&event) != 0)
        {
            if (elements_menu) {
                 running = handle_menu_events(event, C);
            } else {
                running = handle_events(event, C);
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (const auto& element : graphical_elements)
        {
            element->draw(renderer);
        }

        if (elements_menu) {
            draw_component_menu(renderer, font);
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
        switch (event.key.keysym.sym)
        {
            case SDLK_r:
            {
                cout << "R key was pressed." << endl;
                C->add_Graphical_Resistor(mouseX, mouseY);
                break;
            }

            case SDLK_c:
            {
                cout << "C key was pressed." << endl;
                C->add_Graphical_Capacitor(mouseX, mouseY);
                break;
            }

            case SDLK_l:
            {
                cout << "L key was pressed." << endl;
                C->add_Graphical_Inductor(mouseX, mouseY);
                break;
            }

            case SDLK_s:
            {
                cout << "S key was pressed." << endl;
                C->add_Graphical_Current_Source(mouseX, mouseY);
                break;
            }

            case SDLK_d:
            {
                cout << "D key was pressed." << endl;
                C->add_Graphical_Real_Diode(mouseX, mouseY);
                break;
            }

            case SDLK_z:
            {
                cout << "Z key was pressed." << endl;
                C->add_Graphical_Zener_Diode(mouseX, mouseY);
                break;
            }

            case SDLK_p:
            {
                cout << "P key was pressed." << endl;
                elements_menu = !elements_menu;
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

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mouse_pos = {event.button.x, event.button.y};
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

    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
    {
        m_is_dragging = false;
        m_dragged_element_index = -1;
    }

    if (event.type == SDL_MOUSEMOTION && m_is_dragging)
    {
        SDL_Point mouse_pos = {event.motion.x, event.motion.y};
        graphical_elements[m_dragged_element_index]->bounding_box.x = mouse_pos.x - m_drag_offset.x;
        graphical_elements[m_dragged_element_index]->bounding_box.y = mouse_pos.y - m_drag_offset.y;
    }

    return true;
}

// graphical_view.cpp

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
