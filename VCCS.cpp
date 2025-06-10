//
// Created by amirh on 2025-06-07.
//

#include "VCCS.h"

void VCCS::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i, j, ictrl, jctrl, a;
    i = node1->get_index();
    j = node2->get_index();
    ictrl = ctrl_node1->get_index();
    jctrl = ctrl_node1->get_index();
    // stamping
    if (i != -1 && ictrl != -1)
        G_triplets.emplace_back(i, ictrl, value);
    if (i != -1 && jctrl != -1)
        G_triplets.emplace_back(i, jctrl, -value);
    if (j != -1 && ictrl != -1)
        G_triplets.emplace_back(j, ictrl, -value);
    if (j != -1 && jctrl != -1)
        G_triplets.emplace_back(j, jctrl, value);
}

void VCCS::change_name(std::string new_name)
{
    name = new_name;
}

void VCCS::change_value(double new_value)
{
    value = new_value;
}

void VCCS::display_info()
{
    cout << "No Info" << endl;
}

double VCCS::get_current(double time, double time_step)
{
    return 0.0;
}