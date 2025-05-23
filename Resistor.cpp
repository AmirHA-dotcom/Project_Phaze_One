//
// Created by amirh on 2025-05-23.
//

#include "Resistor.h"

double Resistor::get_current()
{
    return (node1->get_voltage() - node2->get_voltage())/value;
}

void Resistor::display_info()
{
    cout << "Resistor Name : " << name << endl;
    cout << "R = " << value << endl;
    cout << "From Node " << node1->get_name() << " to Node " << node2->get_name() << endl;
}