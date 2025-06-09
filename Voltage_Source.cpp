//
// Created by amirh on 2025-05-23.
//

#include "Voltage_Source.h"

void Voltage_Source::set_aux_index(int i)
{
    aux_index = i;
}

void Voltage_Source::stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous)
{
    int i = node1->get_index();
    int j = node2->get_index();
    int a = aux_index;

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
    b[a] += get_value_at(current_time, time_step);
}

void Voltage_Source::set_current(double current, double time)
{
    currents.emplace_back(current, time);
}

double Voltage_Source::get_current(double time, double time_step)
{
    for (int i = 0; i < currents.size(); i++)
    {
        if (abs(time - currents[i].second) < time_step)
            return currents[i].first;
    }
    return 0.0;
}

int Voltage_Source::get_aux_index() const
{
    return aux_index;
}

void Voltage_Source::display_info()
{
    cout << "Element: " << name << endl;
    cout << "Type: Voltage Source" << endl;
    cout << "Node 1: " << node1->get_name() << endl;
    cout << "Node 2: " << node2->get_name() << endl;
}

void Voltage_Source::change_value(double new_value)
{
    this->value = new_value;
}

void Voltage_Source::change_name(std::string new_name)
{
    this->name = new_name;
}

double DC_Source::get_value_at(double time, double time_step) const
{
        return dc_value;
}

double Sine_Source::get_value_at(double time, double time_step) const
{
    double phase_radians = phase_degrees * PI / 180.0;
    return offset + amplitude * sin(2 * PI * frequency * time + phase_radians);
}

double Pulse_Source::get_value_at(double time, double time_step) const
{
    if (time <= time_delay)
    {
        return v_initial;
    }

    // find the time within the current period
    double t = fmod(time - time_delay, period);

    // rise
    if (t <= time_rise)
    {
        return v_initial + (v_pulsed - v_initial) * (t / time_rise);
    }
    // pulse
    if (t <= time_rise + pulse_width)
    {
        return v_pulsed;
    }
    // fall
    if (t <= time_rise + pulse_width + time_fall)
    {
        return v_pulsed + (v_initial - v_pulsed) * ((t - (time_rise + pulse_width)) / time_fall);
    }
    // not pulse
    return v_initial;
}

double Delta_Dirac::get_value_at(double time, double time_step) const
{
    if (abs(time - time_of_delta) < time_step)
    {
        return delta_value;
    }
    return not_delta_value;
}

double Square_Source::get_value_at(double time, double time_step) const
{
    // before delay
    if (time <= time_delay)
    {
        return 0.0;
    }

    // find the time within the current period
    double time_in_period = fmod(time - time_delay, period);

    // upper part
    if (time_in_period <= square_width)
    {
        return v_up;
    }
    // lower part
    else
    {
        return v_down;
    }
}

double Triangular_Source::get_value_at(double time, double time_step) const
{
    // before delay
    if (time <= time_delay)
    {
        return v_initial;
    }

    // finding the period
    double time_in_period = fmod(time - time_delay, period);
    double half_period = period / 2.0;

    // rising half (+m)
    if (time_in_period <= half_period)
    {
        return v_initial + (v_peak - v_initial) * (time_in_period / half_period);
    }
    // falling half (-m)
    else
    {
        return v_peak + (v_initial - v_peak) * ((time_in_period - half_period) / half_period);
    }
}