#include "Zener_Diode.h"


void Zener_Diode::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> &x_k, const vector<double> &x_previous)
{
    const double Is = 1e-14;
    const double Vt = 0.026;
    const double n = 1.0;
    const double Vz = 0.7;

    int i = node1->get_index();
    int j = node2->get_index();
    double Vd = 0;
    if (i != -1 && j != -1)
        Vd = x_k[i] - x_k[j];
    else if (i != -1)
        Vd = x_k[i];
    else
        Vd = -x_k[j];
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
    if (i != -1)
        G_triplets.emplace_back(i, i, g);
    if (j != -1)
        G_triplets.emplace_back(j, j, g);
    if (i != -1 && j != -1) {
        G_triplets.emplace_back(i, j, -g);
        G_triplets.emplace_back(j, i, -g);
    }

    // stamping current source I_eq in b
    if (i != -1)
        b[i] -= I_eq;
    if (j != -1)
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
    for (int i = 0; i < currents.size(); i++)
    {
        if (abs(time - currents[i].second) < time_step)
            return currents[i].first;
    }
    return 0.0;}

void Zener_Diode::set_current(double current, double time)
{
    currents.emplace_back(current, time);
}

double Zener_Diode::calculate_current(const std::vector<double>& x) const
{
    const double Is = 1e-14;
    const double Vt = 0.026;
    const double n = 1.0;
    const double Vz = 0.7; // Zener breakdown voltage
    int i = node1->get_index();
    int j = node2->get_index();
    double Vd = x[i] - x[j];
    // forward bias or reverse breakdown
    if (Vd >= -Vz)
    {
        // forward bias
        return Is * (exp(Vd / (n * Vt)) - 1.0);
    }
    else
    {
        // reverse breakdown
        return -Is * (exp(-(Vd + Vz) / Vt) - 1.0);
    }
}