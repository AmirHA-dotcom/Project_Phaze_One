//
// Created by amirh on 2025-05-23.
//

#include "Inductor.h"

double Inductor::get_current(double time1, double time_step)
{
    double time2 = time1 + time_step;
    double dV1 = node1->get_voltage_in_time(time1) - node2->get_voltage_in_time(time1);
    double dV2 = node1->get_voltage_in_time(time2) - node2->get_voltage_in_time(time2);
    double dT = time2 - time1;
    // I = 1/L * integral(Vdt) = 1/2L * dT * (dV2 + dV1)
    return (dV2 + dV1) * dT/(2 * value);
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

void Inductor::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i = node1->get_index();
    int j = node2->get_index();
    int a = aux_index;

    double R_eq = value / time_step;

    if (i != -1)
        G_triplets.emplace_back(i, a, 1.0);
    if (j != -1)
        G_triplets.emplace_back(j, a, -1.0);
    if (i != -1)
        G_triplets.emplace_back(a, i, 1.0);
    if (j != -1)
        G_triplets.emplace_back(a, j, -1.0);
    G_triplets.emplace_back(a, a, -R_eq);

    double I_previous = x_previous[a];
    b[a] -= R_eq * I_previous;
}