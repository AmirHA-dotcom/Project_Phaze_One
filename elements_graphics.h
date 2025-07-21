//
// Created by amirh on 2025-07-20.
//

#ifndef PROJECT_PHAZE_ONE_ELEMENTS_GRAPHICS_H
#define PROJECT_PHAZE_ONE_ELEMENTS_GRAPHICS_H

#include "Libraries.h"
#include <SDL.h>

class elements_graphics
{
private:
    std::vector<SDL_Rect> m_blocks;
public:
    void add_block(int x, int y);

    // Draws all blocks to the screen
    void draw(SDL_Renderer* renderer);

    // Give graphical_view access to the blocks for collision detection
    std::vector<SDL_Rect>& get_blocks();
};


#endif //PROJECT_PHAZE_ONE_ELEMENTS_GRAPHICS_H
