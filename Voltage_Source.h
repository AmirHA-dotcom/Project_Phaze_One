//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H
#define PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H

#include "Element.h"

class Voltage_Source : public Element
{
protected:
    int aux_index;
public:
    Voltage_Source(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Voltage_Source, _node1, _node2, 0.0) {}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void set_aux_index(int i);
    virtual double get_value_at(double time) const = 0;
};

class DC_Source : public Voltage_Source
{
private:
    double dc_value;
public:
    DC_Source(string _name, Node* _node1, Node* _node2, double _value)
            : Voltage_Source(_name, _node1, _node2, 0.0), dc_value(_value) {}

    double get_value_at(double time) const override
    {
        return dc_value;
    }
};

const double PI = 3.14159;

class Sine_Source : public Voltage_Source
{
private:
    double offset;
    double amplitude;
    double frequency;
    double phase_degrees;
public:
    Sine_Source(string _name, Node* _node1, Node* _node2, double off, double amp, double freq, double phase = 0.0)
            : Voltage_Source(_name, _node1, _node2, 0.0), offset(off), amplitude(amp), frequency(freq), phase_degrees(phase) {}

    double get_value_at(double time) const override
    {
        double phase_radians = phase_degrees * PI / 180.0;
        return offset + amplitude * sin(2 * PI * frequency * time + phase_radians);
    }
};

class Pulse_Source : public Voltage_Source
{
private:
    double v_initial;
    double v_pulsed;
    double time_delay;
    double time_rise;
    double time_fall;
    double pulse_width;
    double period;
public:
    Pulse_Source(string _name, Node* _node1, Node* _node2, double v1, double v2, double td, double tr, double tf, double pw, double per)
            : Voltage_Source(_name, _node1, _node2, 0.0), v_initial(v1), v_pulsed(v2), time_delay(td),
              time_rise(tr), time_fall(tf), pulse_width(pw), period(per) {}

    double get_value_at(double time) const override
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
        if (t <= time_rise + pulse_width) {
            return v_pulsed;
        }
        // fall
        if (t <= time_rise + pulse_width + time_fall) {
            return v_pulsed + (v_initial - v_pulsed) * ((t - (time_rise + pulse_width)) / time_fall);
        }
        // not pulse
        return v_initial;
    }
};

class Delta_Dirac : Voltage_Source
{
private:
    double delta_value;
    double not_delta_value;
    double time_of_delta;
public:
    Delta_Dirac(string _name, Node* _node1, Node* _node2, double time)
    : Voltage_Source(_name, _node1, _node2, 0.0), delta_value(10000000000), not_delta_value(0.0), time_of_delta(time) {}
    double get_value_at(double time) const override
    {

    }
};

#endif //PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H
