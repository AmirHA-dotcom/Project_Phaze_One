//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_NODE_H
#define PROJECT_PHAZE_ONE_NODE_H

#include "Libraries.h"

class Node
{
private:
    string name;
    double voltage;
    bool is_ground;
public:
    Node(string _name);
    string get_name() const;
    double get_voltage();
    void make_ground();
    bool is_the_node_ground();
};


#endif //PROJECT_PHAZE_ONE_NODE_H
