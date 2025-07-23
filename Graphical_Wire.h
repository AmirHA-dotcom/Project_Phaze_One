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
    // A list of all the corner points of the wire
    std::vector<SDL_Point> path;

    Node* start_node;
    Node* end_node;

    // The function to draw the wire
    void draw(SDL_Renderer* renderer);
};


#endif //PROJECT_PHAZE_ONE_GRAPHICAL_WIRE_H
