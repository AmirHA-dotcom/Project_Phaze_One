//
// Created by amirh on 2025-05-23.
//

#include "Node.h"

Node::Node(std::string _name)
{
    name = _name;
    is_ground = false;
}

string Node::get_name() const
{
    return name;
}

double Node::get_voltage_in_time(double time)
{
    for (int i = 0; i < voltage.size() - 1; i++)
    {
        if (time < voltage[i + 1].second && time >= voltage[i].second)
        {
            if (abs(voltage[i + 1].second) - time > abs(voltage[i].second - time))
                return voltage[i].first;
            return voltage[i + 1].first;
        }
    }
}


void Node::make_ground()
{
    is_ground = true;
}

bool Node::is_the_node_ground()
{
    return is_ground;
}

void Node::set_index(int i)
{
    index = i;
}

int Node::get_index()
{
    return index;
}

void Node::set_voltage(double volt, double time)
{
    voltage.emplace_back(volt, time);
}