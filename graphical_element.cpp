//
// Created by amirh on 2025-07-21.
//

#include "graphical_element.h"

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

void Graphical_Resistor::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &bounding_box);
}

void Graphical_Capacitor::draw(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;

    SDL_RenderDrawLine(renderer, x + w/2 - 5, y + 10, x + w/2 - 5, y + h - 10);
    SDL_RenderDrawLine(renderer, x + w/2 + 5, y + 10, x + w/2 + 5, y + h - 10);
}

void Graphical_Inductor::draw(SDL_Renderer* renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_y = y + h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    SDL_RenderDrawLine(renderer, x, center_y, x + w / 4, center_y);

    SDL_Point points[] = {
            {x + w / 4, center_y},
            {x + w / 4 + 5, center_y - 10},
            {x + w / 2 - 5, center_y - 10},
            {x + w / 2, center_y},
            {x + w / 2 + 5, center_y + 10},
            {x + 3 * w / 4 - 5, center_y + 10},
            {x + 3 * w / 4, center_y}
    };
    SDL_RenderDrawLines(renderer, points, 7);

    SDL_RenderDrawLine(renderer, x + 3 * w / 4, center_y, x + w, center_y);
}

void Graphical_Current_Source::draw(SDL_Renderer* renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;

    int center_x = x + w / 2;
    int center_y = y + h / 2;
    int radius = h / 2 - 5;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    draw_circle(renderer, center_x, center_y, radius);

    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x, center_y + radius - 5);
    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x - 5, center_y - radius + 15);
    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x + 5, center_y - radius + 15);
}