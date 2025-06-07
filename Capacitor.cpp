//
// Created by amirh on 2025-05-23.
//

#include "Capacitor.h"

double Capacitor::get_current(double time1, double time2)
{
    cout << "Still Not Coded!!!" << endl;
    return 0;
}

void Capacitor::display_info()
{
    cout << "Capacitor Name : " << name << endl;
    cout << "C = " << value << endl;
    cout << "From Node " << node1->get_name() << " to Node " << node2->get_name() << endl;
}
void Capacitor::change_value(double new_value)
{
    value = new_value;
}
void Capacitor::change_name(std::string new_name)
{
    name = new_name;
}