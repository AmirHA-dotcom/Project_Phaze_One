//
// Created by amirh on 2025-07-22.
//

#include "Graphical_Wire.h"

void Graphical_Wire::draw(SDL_Renderer* renderer)
{
    if (path.size() < 2) return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (size_t i = 0; i < path.size() - 1; ++i)
    {
        SDL_RenderDrawLine(renderer, path[i].x, path[i].y, path[i+1].x, path[i+1].y);
    }
}