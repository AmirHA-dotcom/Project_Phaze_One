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

void Capacitor::stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i, j;
    i = node1->get_index();
    j = node2->get_index();
    double g = value/time_step;
    G_triplets.emplace_back(i, j, g);
    G_triplets.emplace_back(i, j, -g);
    G_triplets.emplace_back(j, i, -g);
    G_triplets.emplace_back(j, j, g);
    double I = g * (x_previous[i] - x_previous[j]);
    b[i] +=  I;
    b[j] += -I;
}