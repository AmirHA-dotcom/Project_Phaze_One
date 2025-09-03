//
// Created by amirh on 2025-05-23.
//

#include "Element.h"

Element::Element(string _name, Element_Type _type, shared_ptr<Node> n1, shared_ptr<Node> n2, double _value)
{
    name = _name;
    type = _type;
    node1 = n1;
    node2 = n2;
    value = _value;
    rotation = Rotation::Right;
}

string Element::get_name() const
{
    return name;
}

int Element::get_rotation_as_int (){
    switch (rotation) {
        case Rotation::Right:
            return 0;
        case Rotation::Left:
            return 1;
        case Rotation::Up:
            return 2;
        case Rotation::Down:
            return 3;
    }
    return -1; // Should never reach here if rotation is set correctly
}

void Element::set_rotation_by_int (int r){
    switch (r) {
        case 0:
            rotation = Rotation::Right;
            break;
        case 1:
            rotation = Rotation::Left;
            break;
        case 2:
            rotation = Rotation::Up;
            break;
        case 3:
            rotation = Rotation::Down;
            break;
        default:
            throw invalid_argument("Invalid rotation value");
    }
}

Element_Type Element::get_element_type()
{
    return type;
}

pair<shared_ptr<Node>, shared_ptr<Node>> Element::get_nodes()
{
    return {node1, node2};
}

double Element::get_voltage_at_time(double time)
{
    return node1->get_voltage_in_time(time) - node2->get_voltage_in_time(time);
}

void Element::replace_node(shared_ptr<Node> node_to_merge, shared_ptr<Node> node_to_keep)
{
    if (node1 == node_to_merge)
        node1 = node_to_keep;
    if (node2 == node_to_merge)
        node2 = node_to_keep;
}
