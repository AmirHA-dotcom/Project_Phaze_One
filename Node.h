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
public:
    Node(string _name);
    string get_name() const;
    double get_voltage();
};


#endif //PROJECT_PHAZE_ONE_NODE_H
