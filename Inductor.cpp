//
// Created by amirh on 2025-05-23.
//

#include "Inductor.h"

double Inductor::get_current(double time1, double time2)
{
    cout << "Still Not Coded!!!" << endl;
    return 0;
}

void Inductor::display_info()
{
    cout << "Inductor Name : " << name << endl;
    cout << "L = " << value << endl;
    cout << "From Node " << node1->get_name() << " to Node " << node2->get_name() << endl;
}
void Inductor::change_value(double new_value)
{
    value = new_value;
}
void Inductor::change_name(std::string new_name)
{
    name = new_name;
}