//
// Created by amirh on 2025-05-23.
//

#include "Element.h"

Element::Element(string _name, Element_Type _type, Node *n1, Node *n2, double _value)
{
    name = _name;
    type = _type;
    node1 = n1;
    node2 = n2;
    value = _value;
}

string Element::get_name() const
{
    return name;
}

Element_Type Element::get_element_type()
{
    return type;
}

pair<Node *, Node *> Element::get_nodes()
{
    return {node1, node2};
}

double Element::get_voltage()
{
    return node1->get_voltage() - node2->get_voltage();
}