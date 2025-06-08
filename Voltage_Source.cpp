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
    int i, j, a;
    i = node1->get_index();
    j = node2->get_index();
    a = aux_index;
    // stamping in G
    G_triplets.emplace_back(i, a, 1.0);
    G_triplets.emplace_back(j, a, -1.0);
    G_triplets.emplace_back(a, i, 1.0);
    G_triplets.emplace_back(a, j, -1.0);
    // stamping in b
    b[a] = get_value_at(current_time, time_step);
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
    if (t <= time_rise) {
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
    if (time <= time_delay)
    {
        return v_initial;
    }

    // find the time within the current period
    double t = fmod(time - time_delay, period);

    // rise
    if (t <= time_rise)
    {
        return v_initial + (v_up - v_initial) * (t / time_rise);
    }
    // up
    if (t <= time_rise + square_width)
    {
        return v_up;
    }
    // fall
    if (t <= time_rise + square_width + time_fall)
    {
        return v_up + (v_initial - v_up) * ((t - (time_rise + square_width)) / time_fall);
    }
    // not up
    return v_down;
}