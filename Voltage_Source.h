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
    virtual double get_value_at(double time, double time_step) const = 0;
};

class DC_Source : public Voltage_Source
{
private:
    double dc_value;
public:
    DC_Source(string _name, Node* _node1, Node* _node2, double _value)
            : Voltage_Source(_name, _node1, _node2, 0.0), dc_value(_value) {}

    double get_value_at(double time, double time_step) const override;
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

    double get_value_at(double time, double time_step) const override;
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

    double get_value_at(double time, double time_step) const override;
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
    double get_value_at(double time, double time_step) const override;
};

class Square_Source : Voltage_Source
{
private:
    double v_initial;
    double v_up;
    double v_down;
    double time_delay;
    double time_rise;
    double time_fall;
    double square_width;
    double period;
public:
    Square_Source(string _name, Node* _node1, Node* _node2, double v1, double v2, double v3, double td, double tr, double tf, double pw, double per)
    : Voltage_Source(_name, _node1, _node2, 0.0), v_initial(v1), v_up(v2), v_down(v3), time_delay(td),
    time_rise(tr), time_fall(tf), square_width(pw), period(per) {}
    double get_value_at(double time, double time_step) const override;
};

class Triangular_Source : public Voltage_Source {
private:
    double v_initial;
    double v_peak;
    double time_delay;
    double period;

public:
//    Triangular_Source(string _name, Node* _node1, Node* _node2, double v1, double v2, double td, double per)
//            : Voltage_Source(_name, _node1, _node2), v_initial(v1), v_peak(v2), time_delay(td), period(per) {}

    // Override the virtual function to provide the triangle wave logic
    double get_value_at(double time, double time_step) const override;
};

#endif //PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H
