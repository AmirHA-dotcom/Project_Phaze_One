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
    vector<pair<double, double>> currents;  // current time
    double amplitude = 0;
public:
    Voltage_Source(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Voltage_Source, _node1, _node2, 0.0) {}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void set_aux_index(int i);
    virtual double get_value_at(double time, double time_step) const = 0;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    double get_current(double time, double time_step) override;
    void set_current(double current, double time);
    int get_aux_index() const;
    virtual double getAmplitude() const { return amplitude; }
};

class DC_Source : public Voltage_Source
{
private:
    double dc_value;
public:
    DC_Source(string _name, Node* _node1, Node* _node2, double _value)
            : Voltage_Source(_name, _node1, _node2, 0.0), dc_value(_value) {}

    double get_value_at(double time, double time_step) const override;
    void set_value(double val) { dc_value = val; }
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
    void get_parameters(double& out_offset, double& out_amplitude, double& out_frequency, double& out_phase) const;
    void set_offset(double val) { offset = val; }
    double getOffset () {return offset;}
    virtual double getAmplitude() const override { return amplitude; }
    void set_amplitude(double val) { amplitude = val; }
    void set_frequency(double val) { frequency = val; }
    void set_phase_degrees(double val) { phase_degrees = val; }
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
    void get_parameters(double& v_initial, double& v_pulsed, double& time_delay, double& time_rise, double& time_fall, double& pulse_width, double& period) const;
    void set_v_initial(double val) { v_initial = val; }
    void set_v_pulsed(double val) { v_pulsed = val; }
    void set_time_delay(double val) { time_delay = val; }
    void set_time_rise(double val) { time_rise = val; }
    void set_time_fall(double val) { time_fall = val; }
    void set_pulse_width(double val) { pulse_width = val; }
    void set_period(double val) { period = val; }
};

class Delta_Dirac : public Voltage_Source
{
private:
    double delta_value;
    double not_delta_value;
    double time_of_delta;
public:
    Delta_Dirac(string _name, Node* _node1, Node* _node2, double time)
            : Voltage_Source(_name, _node1, _node2, 0.0), delta_value(10000000000), not_delta_value(0.0), time_of_delta(time) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& delta_value, double& not_delta_value, double& time_of_delta) const;
    void set_time_of_delta(double val) { time_of_delta = val; }
};

class Square_Source : public Voltage_Source
{
private:
    double v_down;
    double v_up;
    double time_delay;
    double square_width;
    double period;
public:
    Square_Source(string _name, Node* _node1, Node* _node2, double v1, double v2, double td, double pw, double per)
            : Voltage_Source(_name, _node1, _node2, 0.0), v_down(v1), v_up(v2), time_delay(td),
              square_width(pw), period(per) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& v_down, double& v_up, double& time_delay, double& square_width, double& period) const;
    void set_v_down(double val) { v_down = val; }
    void set_v_up(double val) { v_up = val; }
    void set_time_delay(double val) { time_delay = val; }
    void set_square_width(double val) { square_width = val; }
    void set_period(double val) { period = val; }
};

class Triangular_Source : public Voltage_Source
{
private:
    double v_initial;
    double v_peak;
    double time_delay;
    double period;
public:
    Triangular_Source(string _name, Node* _node1, Node* _node2, double v1, double v2, double td, double per)
            : Voltage_Source(_name, _node1, _node2, 0.0), v_initial(v1), v_peak(v2), time_delay(td), period(per) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& v_initial, double& v_peak, double& time_delay, double& period) const;
    void set_v_initial(double val) { v_initial = val; }
    void set_v_peak(double val) { v_peak = val; }
    void set_time_delay(double val) { time_delay = val; }
    void set_period(double val) { period = val; }
};


#endif //PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H