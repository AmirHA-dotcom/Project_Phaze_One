#include "Real_Diode.h"

void Real_Diode::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> &x_k, const vector<double> &x_previous)
{
    const double Is = 1e-14;
    const double Vt = 0.026;
    const double n = 1.0;

    int i = node1->get_index();
    int j = node2->get_index();
    double Vd = x_k[i] - x_k[j];

    // shockley diode equation i = Is * (exp(Vd / (n*Vt)) - 1)
    double Id = Is * (exp(Vd / (n * Vt)) - 1.0);

    // taking a derivative of the diode equation to get the conductance
    double g = (Is / (n * Vt)) * exp(Vd / (n * Vt));

    // adding a current source
    double I_eq = Id - g * Vd;

    // stamping in G
    G_triplets.emplace_back(i, i, g);
    G_triplets.emplace_back(i, j, -g);
    G_triplets.emplace_back(j, i, -g);
    G_triplets.emplace_back(j, j, g);

    // stamping the current source in b (because of the error)
    b[i] -= I_eq;
    b[j] += I_eq;
}

void Real_Diode::change_name(std::string new_name)
{
    name = new_name;
}

void Real_Diode::change_value(double new_value)
{
    value = new_value;
}

void Real_Diode::display_info()
{
    cout << "No Info" << endl;
}