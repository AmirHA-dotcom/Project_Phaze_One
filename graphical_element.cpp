//
// Created by amirh on 2025-07-21.
//

#include "graphical_element.h"


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
    // Get the dimensions from the bounding box
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_y = y + h / 2;

    // Set draw color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Draw the bounding box outline (optional, but good for debugging)
    SDL_RenderDrawRect(renderer, &bounding_box);

    // Draw lead-in line
    SDL_RenderDrawLine(renderer, x, center_y, x + w / 4, center_y);

    // Draw three loops (approximated with lines)
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

    // Draw lead-out line
    SDL_RenderDrawLine(renderer, x + 3 * w / 4, center_y, x + w, center_y);
}

// Helper function to draw a circle (Midpoint circle algorithm)
void draw_circle(SDL_Renderer* renderer, int center_x, int center_y, int radius)
{
    int x = radius - 1;
    int y = 0;
    int tx = 1;
    int ty = 1;
    int err = tx - (radius << 1); // radius * 2

    while (x >= y) {
        // Each of the following renders an octant of the circle
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

void Graphical_Current_Source::draw(SDL_Renderer* renderer)
{
    // Get the dimensions from the bounding box
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;

    // Calculate center and radius for the circle
    int center_x = x + w / 2;
    int center_y = y + h / 2;
    int radius = h / 2 - 5; // A little smaller than the box height

    // Set draw color to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    // Draw the circle
    draw_circle(renderer, center_x, center_y, radius);

    // Draw the arrow inside the circle (pointing up)
    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x, center_y + radius - 5);
    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x - 5, center_y - radius + 15);
    SDL_RenderDrawLine(renderer, center_x, center_y - radius + 5, center_x + 5, center_y - radius + 15);
}