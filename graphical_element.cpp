//
// Created by amirh on 2025-07-21.
//

#include "graphical_element.h"

// helper functions

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

// main functions

void Graphical_Resistor::draw(SDL_Renderer *renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_y = y + h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    int lead_length = w / 5;

    SDL_RenderDrawLine(renderer, x, center_y, x + lead_length, center_y);

    SDL_Point points[] = {
            {x + lead_length, center_y},
            {x + lead_length + (w - 2 * lead_length) * 1 / 6, center_y + 10},
            {x + lead_length + (w - 2 * lead_length) * 3 / 6, center_y - 10},
            {x + lead_length + (w - 2 * lead_length) * 5 / 6, center_y + 10},
            {x + w - lead_length, center_y}
    };
    SDL_RenderDrawLines(renderer, points, 5);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Capacitor::draw(SDL_Renderer *renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_x = x + w / 2;
    int center_y = y + h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    int plate_height = h;
    int gap = 8;

    SDL_RenderDrawLine(renderer, x, center_y, center_x - gap/2, center_y);
    SDL_RenderDrawLine(renderer, center_x - gap/2, center_y - plate_height/2, center_x - gap/2, center_y + plate_height/2);
    SDL_RenderDrawLine(renderer, center_x + gap/2, center_y - plate_height/2, center_x + gap/2, center_y + plate_height/2);
    SDL_RenderDrawLine(renderer, center_x + gap/2, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
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

    int num_loops = 3;
    int lead_length = w / 6;
    int loops_total_width = w - (2 * lead_length);
    int loop_width = loops_total_width / num_loops;
    int radius = h / 3;

    SDL_RenderDrawLine(renderer, x, center_y, x + lead_length, center_y);

    for (int i = 0; i < num_loops; ++i) {
        int loop_center_x = x + lead_length + (i * loop_width) + (loop_width / 2);
        draw_arc(renderer, loop_center_x, center_y, radius, 180, 360);
    }

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Current_Source::draw(SDL_Renderer* renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;

    int center_x = x + w / 2;
    int center_y = y + h / 2;
    int radius = h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    draw_circle(renderer, center_x, center_y, radius);

    SDL_RenderDrawLine(renderer, center_x - radius + 5, center_y, center_x + radius - 5, center_y);
    SDL_RenderDrawLine(renderer, center_x + radius - 5, center_y, center_x + radius - 15, center_y - 5);
    SDL_RenderDrawLine(renderer, center_x + radius - 5, center_y, center_x + radius - 15, center_y + 5);

    SDL_RenderDrawLine(renderer, x, center_y, center_x - radius, center_y);
    SDL_RenderDrawLine(renderer, center_x + radius, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Real_Diode::draw(SDL_Renderer* renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_y = y + h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    int lead_length = w / 4;

    SDL_RenderDrawLine(renderer, x, center_y, x + lead_length, center_y);

    SDL_Point triangle_points[] = {
            {x + lead_length, center_y - 15},
            {x + w - lead_length, center_y},
            {x + lead_length, center_y + 15},
            {x + lead_length, center_y - 15}
    };
    SDL_RenderDrawLines(renderer, triangle_points, 4);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y - 15, x + w - lead_length, center_y + 15);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}

void Graphical_Zener_Diode::draw(SDL_Renderer* renderer)
{
    int x = bounding_box.x;
    int y = bounding_box.y;
    int w = bounding_box.w;
    int h = bounding_box.h;
    int center_y = y + h / 2;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderDrawRect(renderer, &bounding_box);

    int lead_length = w / 4;

    SDL_RenderDrawLine(renderer, x, center_y, x + lead_length, center_y);

    SDL_Point triangle_points[] = {
            {x + lead_length, center_y - 15},
            {x + w - lead_length, center_y},
            {x + lead_length, center_y + 15},
            {x + lead_length, center_y - 15}
    };
    SDL_RenderDrawLines(renderer, triangle_points, 4);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y - 15, x + w - lead_length, center_y + 15);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y - 15, x + w - lead_length - 8, center_y - 10);
    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y + 15, x + w - lead_length + 8, center_y + 10);

    SDL_RenderDrawLine(renderer, x + w - lead_length, center_y, x + w, center_y);

    if (model_element != nullptr)
    {
        render_text(renderer, font, model_element->get_name(), bounding_box.x, bounding_box.y - 20);
    }
}