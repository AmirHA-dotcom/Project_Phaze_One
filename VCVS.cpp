//
// Created by amirh on 2025-06-07.
//

#include "VCVS.h"

void VCVS::set_aux_index(int i)
{
    aux_index = i;
}

void VCVS::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{

    int i = node1->get_index();
    int j = node2->get_index();

    int ictrl = ctrl_node1->get_index();
    int jctrl = ctrl_node2->get_index();

    int a = aux_index;

    double gain = value;

    if (i != -1) {
        G_triplets.emplace_back(i, a, 1.0);
    }
    if (j != -1) {
        G_triplets.emplace_back(j, a, -1.0);
    }

    if (i != -1) {
        G_triplets.emplace_back(a, i, 1.0);
    }
    if (j != -1) {
        G_triplets.emplace_back(a, j, -1.0);
    }
    if (ictrl != -1) {
        G_triplets.emplace_back(a, ictrl, -gain);
    }
    if (jctrl != -1) {
        G_triplets.emplace_back(a, jctrl, gain);
    }

}
void VCVS::change_name(std::string new_name)
{
    name = new_name;
}

void VCVS::change_value(double new_value)
{
    value = new_value;
}

void VCVS::display_info()
{
    cout << "No Info" << endl;
}

double VCVS::get_current(double time, double time_step)
{
    for (int i = 0; i < currents.size(); i++)
    {
        if (abs(time - currents[i].second) < time_step)
            return currents[i].first;
    }
    return 0.0;
}

void VCVS::set_current(double current, double time)
{
    currents.emplace_back(current, time);
}

int VCVS::get_aux_index() const
{
    return aux_index;
}