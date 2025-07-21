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
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        // menus(not coded yet)

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return true;
}