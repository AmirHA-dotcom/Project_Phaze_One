//
// Created by amirh on 2025-07-20.
//

#include "graphical_view.h"

bool graphical_view::run(Controller *C)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw runtime_error("SDL could not initialize! SDL_Error: " + string(SDL_GetError()));
    }

    SDL_Window* window = SDL_CreateWindow(
            "My Application",
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

    elements_graphics* EG = new elements_graphics();

    while (running) 
    {
        while (SDL_PollEvent(&event) != 0) 
        {
            if (event.type == SDL_QUIT) 
            {
                running = false;
            }

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    m_window_width = event.window.data1;
                    m_window_height = event.window.data2;
                    cout << "Window resized to " << m_window_width << "x" << m_window_height << endl;
                }
            }

            if (event.type == SDL_KEYDOWN)
            {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                switch (event.key.keysym.sym)
                {
                    case SDLK_r:
                        cout << "R key was pressed." << endl;
                        EG->add_block(mouseX, mouseY);
                        break;

                    case SDLK_s:
                        cout << "S key was pressed." << endl;
                        break;

                    case SDLK_a:
                        cout << "A key was pressed." << endl;
                        break;

                    case SDLK_d:
                        cout << "D key was pressed." << endl;
                        break;

                    case SDLK_RETURN: 
                        cout << "Enter key was pressed." << endl;
                        break;

                    case SDLK_ESCAPE:
                        cout << "Escape key was pressed." << endl;
                        break;

                    default:
                        // Optional: handle other keys
                        break;
                }
            }

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                SDL_Point mouse_pos = {event.button.x, event.button.y};

                auto& blocks = EG->get_blocks();
                for (int i = 0; i < blocks.size(); ++i) {
                    if (SDL_PointInRect(&mouse_pos, &blocks[i]))
                    {
                        m_is_dragging = true;
                        m_dragged_block_index = i;
                        m_drag_offset.x = mouse_pos.x - blocks[i].x;
                        m_drag_offset.y = mouse_pos.y - blocks[i].y;
                        break;
                    }
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT)
            {
                m_is_dragging = false;
                m_dragged_block_index = -1;
            }

            if (event.type == SDL_MOUSEMOTION && m_is_dragging)
            {
                SDL_Point mouse_pos = {event.motion.x, event.motion.y};
                auto& blocks = EG->get_blocks();
                blocks[m_dragged_block_index].x = mouse_pos.x - m_drag_offset.x;
                blocks[m_dragged_block_index].y = mouse_pos.y - m_drag_offset.y;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        EG->draw(renderer);

        SDL_RenderPresent(renderer);
    }

    delete EG;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}