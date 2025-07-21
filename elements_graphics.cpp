#include "elements_graphics.h"

void elements_graphics::add_block(int x, int y) {
    SDL_Rect new_block = {x, y, 50, 50};
    m_blocks.push_back(new_block);
}

void elements_graphics::draw(SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0x66, 0xCC, 0xFF);

    for (const auto& block : m_blocks) 
    {
        SDL_RenderFillRect(renderer, &block);
    }
}

// Getter to access the blocks from the main loop
vector<SDL_Rect>& elements_graphics::get_blocks() 
{
    return m_blocks;
}