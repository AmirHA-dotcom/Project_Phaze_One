//
// Created by amirh on 2025-05-23.
//

#include "Node.h"

Node::Node(std::string _name)
{
    name = _name;
    is_ground = false;
    elements_connected_count = 0;
}

string Node::get_name() const
{
    return name;
}

void Node::change_name(string new_name)
{
    name = new_name;
}

double Node::get_voltage_in_time(double time)
{
    if (voltage.size() < 2)
    {
        if (!voltage.empty()) {
            return voltage[0].first;
        }
        return 0.0;
    }
    for (size_t i = 0; i < voltage.size() - 1; ++i)
    {
        if (time >= voltage[i].second && time < voltage[i+1].second)
        {
            if ((time - voltage[i].second) < (voltage[i+1].second - time))
                return voltage[i].first;
            else
                return voltage[i+1].first;
        }
    }
    return voltage.back().first;
}

vector<pair<double, double>> Node::get_all_voltages()
{
//    for (int i = 0; i < voltage.size(); i++)
//        cout << voltage[i].second << ": " << voltage[i].first << endl;
    return voltage;
}


void Node::make_ground()
{
    is_ground = true;
}

void Node::return_to_normal()
{
    is_ground = false;
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

void Node::connect_element()
{
    elements_connected_count++;
}

void Node::disconnect_element()
{
    elements_connected_count--;
}

int Node::connected_elements_count() const
{
    return elements_connected_count;
}

void Node::reset_voltages()
{
    voltage.clear();
}

void Node::set_net_label_coordinates(int x_, int y_)
{
//    net_label_coordinates.first = x_;
//    net_label_coordinates.second = y_;

    net_label_coordinates_vector.push_back({x_, y_});

//    for (auto co: net_label_coordinates_vector)
//        cout << "X = " << co.first << "\tY = " << co.second << endl;
}
void Node::set_ground_coordinates(int x_, int y_)
{
    ground_coordinates.first = x_;
    ground_coordinates.second = y_;
}