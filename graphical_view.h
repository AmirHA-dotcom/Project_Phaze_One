//
// Created by amirh on 2025-07-20.
//

#ifndef PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
#define PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H

#include "Libraries.h"
#include "Errors.h"
#include "Controller.h"
#include <SDL.h>
#include "elements_graphics.h"

class graphical_view
{
private:
    bool mainMenu = true;
    bool circuitMenu = false;
    bool fileMenu = false;
    bool analysisMenu = false;

    int m_window_width;
    int m_window_height;

    bool m_is_dragging = false;
    int m_dragged_element_index = -1;
    SDL_Point m_drag_offset = {0, 0};

public:
    bool run (Controller* C);
};

#endif //PROJECT_PHAZE_ONE_GRAPHICAL_VIEW_H
