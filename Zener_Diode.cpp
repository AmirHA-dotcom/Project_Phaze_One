#include "Zener_Diode.h"


void Zener_Diode::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> &x_k, const vector<double> &x_previous)
{
    const double Is = 1e-14;
    const double Vt = 0.026;
    const double n = 1.0;
    const double Vz = 0.7;

    int i = node1->get_index();
    int j = node2->get_index();
    double Vd = x_k[i] - x_k[j];

    double Id, g;

    // forward bias or reverse breakdown
    if (Vd >= -Vz)
    {
        // regular diode (FB)
        Id = Is * (exp(Vd / (n * Vt)) - 1.0);
        g = (Is / (n * Vt)) * exp(Vd / (n * Vt));
    }
    else
    {
        // opposing voltage source (RB)
        // we model this using a large conductance and the reverse breakdown current
        Id = -Is * (exp(-(Vd + Vz) / Vt) -1);
        g = (Is / Vt) * exp(-(Vd + Vz) / Vt);
    }

    // the equivalent current source (changes according to the FB or RB)
    double I_eq = Id - g * Vd;

    // stamping g in the G
    G_triplets.emplace_back(i, i, g);
    G_triplets.emplace_back(i, j, -g);
    G_triplets.emplace_back(j, i, -g);
    G_triplets.emplace_back(j, j, g);

    // stamping current source I_eq in b
    b[i] -= I_eq;
    b[j] += I_eq;
}

void Zener_Diode::change_name(std::string new_name)
{
    name = new_name;
}

void Zener_Diode::change_value(double new_value)
{
    value = new_value;
}

void Zener_Diode::display_info()
{
    cout << "No Info" << endl;
}

double Zener_Diode::get_current(double time, double time_step)
{
    return 0.0;
}