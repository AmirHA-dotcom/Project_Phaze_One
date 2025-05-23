//
// Created by amirh on 2025-05-23.
//

#include "Node.h"

Node::Node(std::string _name)
{
    name = _name;
}

string Node::get_name() const
{
    return name;
}

double Node::get_voltage()
{
    return voltage;
}