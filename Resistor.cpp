//
// Created by amirh on 2025-05-23.
//

#include "Resistor.h"

double Resistor::get_current(double time, double time_step)
{
    return (node1->get_voltage_in_time(time) - node2->get_voltage_in_time(time))/value;
}

void Resistor::display_info()
{
    cout << "Resistor Name : " << name << endl;
    cout << "R = " << value << endl;
    cout << "From Node " << node1->get_name() << " to Node " << node2->get_name() << endl;
}
void Resistor::change_value(double new_value)
{
    value = new_value;
}
void Resistor::change_name(std::string new_name)
{
    name = new_name;
}

void Resistor::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i, j;
    i = node1->get_index();
    j = node2->get_index();
    double g = 1/value;
    G_triplets.emplace_back(i, i, g);
    G_triplets.emplace_back(i, j, -g);
    G_triplets.emplace_back(j, i, -g);
    G_triplets.emplace_back(j, j, g);
}