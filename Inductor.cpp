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

void Inductor::set_aux_index(int i)
{
    aux_index = i;
}

int Inductor::get_aux_index()
{
    return aux_index;
}

void Inductor::stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous)
{
    int i, j, a;
    i = node1->get_index();
    j = node2->get_index();
    a = aux_index;
    double g = value/time_step;
    G_triplets.emplace_back(i, j, g);
    G_triplets.emplace_back(i, j, -g);
    G_triplets.emplace_back(j, i, -g);
    G_triplets.emplace_back(j, j, g);
    b[a] -= g * x_previous[a];
}