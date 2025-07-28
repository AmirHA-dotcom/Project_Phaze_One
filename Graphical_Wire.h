//
// Created by amirh on 2025-07-22.
//

#ifndef PROJECT_PHAZE_ONE_GRAPHICAL_WIRE_H
#define PROJECT_PHAZE_ONE_GRAPHICAL_WIRE_H

#include "Libraries.h"
#include "Node.h"

class Graphical_Wire
{
public:
    vector<SDL_Point> path;

    Node* start_node;
    Node* end_node;

    void draw(SDL_Renderer* renderer);
};


#endif //PROJECT_PHAZE_ONE_GRAPHICAL_WIRE_H
