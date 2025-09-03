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
    Voltage_Source() : Element("default", Element_Type::Voltage_Source, nullptr, nullptr, 0.0) {}
    Voltage_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value) : Element(_name, Element_Type::Voltage_Source, _node1, _node2, 0.0) {}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void set_aux_index(int i);
    virtual double get_value_at(double time, double time_step) const = 0;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    double get_current(double time, double time_step) override;
    void set_current(double current, double time);
    int get_aux_index() const;
    double getAmplitude() { return amplitude; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Element>(this));
    }
};
CEREAL_REGISTER_TYPE(Voltage_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Voltage_Source);


class Waveform_Voltage_Source : public Voltage_Source {
private:
    vector<pair<double, double>> segments; // (voltage, duration)
    double totalDuration = 0.0;

public:
    Waveform_Voltage_Source() : Voltage_Source("default_waveform", nullptr, nullptr, 0.0) {}
    // Constructor
    Waveform_Voltage_Source(std::string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, const std::vector<std::pair<double, double>>& _segments)
            : Voltage_Source(_name, _node1, _node2, 0.0), segments(_segments) {
        totalDuration = 0.0;
        for (const auto& seg : segments) {
            if (seg.second <= 0) {
                throw std::invalid_argument("Duration must be positive.");
            }
            totalDuration += seg.second;
        }
    }
    Waveform_Voltage_Source(std::string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2)
            : Voltage_Source(_name, _node1, _node2, 0.0) {}

    // Add a voltage segment (voltage in volts, duration in seconds)
    void add_segment(double voltage, double duration) {
        if (duration <= 0) {
            throw std::invalid_argument("Duration must be positive.");
        }
        segments.emplace_back(voltage, duration);
        totalDuration += duration;
    }
    void set_segments(const std::vector<std::pair<double, double>>& newSegments) {
        double total = 0.0;
        for (const auto& seg : newSegments) {
            if (seg.second <= 0) {
                throw std::invalid_argument("Duration must be positive for all segments.");
            }
            total += seg.second;
        }
        segments = newSegments;
        totalDuration = total;
    }

    vector<pair<double, double>> get_data_points() { return segments; }

    // Get the value of the voltage at a given time
    double get_value_at(double time, double /*time_step*/) const override {
        if (time < 0.0 || segments.empty()) {
            return 0.0;
        }

        double elapsed = 0.0;
        for (const auto& seg : segments) {
            double voltage = seg.first;
            double dur = seg.second;
            if (time >= elapsed && time < elapsed + dur) {
                return voltage;
            }
            elapsed += dur;
        }
        return 0.0; // Time beyond the end of waveform
    }
    // Interactive method to read waveform from user
    void read_from_user() {
        int count;
        std::cout << "Enter number of voltage segments: ";
        std::cin >> count;

        for (int i = 0; i < count; ++i) {
            double voltage, duration;
            std::cout << "Segment " << (i + 1) << " - Voltage (V): ";
            std::cin >> voltage;
            std::cout << "Segment " << (i + 1) << " - Duration (s): ";
            std::cin >> duration;
            add_segment(voltage, duration);
        }
    }
    // Optional: Get total duration
    double get_total_duration() const {
        return totalDuration;
    }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }

};
CEREAL_REGISTER_TYPE(Waveform_Voltage_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Waveform_Voltage_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Waveform_Voltage_Source);


class DC_Source : public Voltage_Source
{
private:
    double dc_value;
public:
    DC_Source() : Voltage_Source("default_dc", nullptr, nullptr, 0.0), dc_value(0.0) {}
    DC_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value)
            : Voltage_Source(_name, _node1, _node2, 0.0), dc_value(_value) {}

    double get_value_at(double time, double time_step) const override;
    void set_value(double val) { dc_value = val; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }

};
CEREAL_REGISTER_TYPE(DC_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, DC_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, DC_Source);


const double PI = 3.14159;

class AC_Voltage_Source : public Voltage_Source
{
private:
    double amplitude;
    double frequency;
    double phase_degrees; // فاز در درجه

public:
    AC_Voltage_Source() : Voltage_Source("default_ac", nullptr, nullptr, 0.0), amplitude(0.0), frequency(0.0), phase_degrees(0.0) {}
    AC_Voltage_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double amp)
            : Voltage_Source(_name, _node1, _node2, 0.0), amplitude(amp), frequency(0.0), phase_degrees(0.0) {}
    AC_Voltage_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double amp, double freq, double phase = 0.0)
            : Voltage_Source(_name, _node1, _node2, 0.0), amplitude(amp), frequency(freq), phase_degrees(phase) {}

    void get_parameters(double& out_amplitude, double& out_frequency, double& out_phase) const {
        out_amplitude = amplitude;
        out_frequency = frequency;
        out_phase = phase_degrees;
    }

    double getAmplitude() const override { return amplitude; }
    void set_amplitude(double val) { amplitude = val; }
    void set_frequency(double val) { frequency = val; }
    void set_phase_degrees(double val) { phase_degrees = val; }
    double getPhase() const { return phase_degrees; }
    void setPhase(double val) { phase_degrees = val; } // برای هماهنگی با performPhaseSweep
    // dummy function
    double get_value_at(double time, double time_step) const override { return 0.0; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }
};
CEREAL_REGISTER_TYPE(AC_Voltage_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, AC_Voltage_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, AC_Voltage_Source);

class Sine_Source : public Voltage_Source
{
private:
    double offset;
    double amplitude;
    double frequency;
    double phase_degrees;
    double phase = 0;
public:
    Sine_Source() : Voltage_Source("default_sine", nullptr, nullptr, 0.0), offset(0.0), amplitude(0.0), frequency(0.0), phase_degrees(0.0) {}
    Sine_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double off, double amp, double freq, double phase = 0.0)
            : Voltage_Source(_name, _node1, _node2, 0.0), offset(off), amplitude(amp), frequency(freq), phase_degrees(phase) {}

    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& out_offset, double& out_amplitude, double& out_frequency, double& out_phase) const;
    void set_offset(double val) { offset = val; }
    double getOffset () {return offset;}
    virtual double getAmplitude() const override { return amplitude; }
    void set_amplitude(double val) { amplitude = val; }
    void set_frequency(double val) { frequency = val; }
    void set_phase_degrees(double val) { phase_degrees = val; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }
};
CEREAL_REGISTER_TYPE(Sine_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Sine_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Sine_Source);


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
    Pulse_Source() : Voltage_Source("default_pulse", nullptr, nullptr, 0.0), v_initial(0.0), v_pulsed(0.0),
                    time_delay(0.0), time_rise(0.0), time_fall(0.0), pulse_width(0.0), period(0.0) {}
    Pulse_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double v1, double v2, double td, double tr, double tf, double pw, double per)
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
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }
};
CEREAL_REGISTER_TYPE(Pulse_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Pulse_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Pulse_Source);

class Delta_Dirac : public Voltage_Source
{
private:
    double delta_value;
    double not_delta_value;
    double time_of_delta;
public:
    Delta_Dirac(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double time, double delta_val, double not_delta_val)
    : Voltage_Source(_name, _node1, _node2, 0.0), delta_value(delta_val), not_delta_value(not_delta_val), time_of_delta(time) {}
    Delta_Dirac() : Voltage_Source("default_delta", nullptr, nullptr, 0.0), delta_value(10000000000), not_delta_value(0.0), time_of_delta(0.0) {}
    Delta_Dirac(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double time)
            : Voltage_Source(_name, _node1, _node2, 0.0), delta_value(10000000000), not_delta_value(0.0), time_of_delta(time) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& delta_value, double& not_delta_value, double& time_of_delta) const;
    void set_time_of_delta(double val) { time_of_delta = val; }
};
CEREAL_REGISTER_TYPE(Delta_Dirac);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Delta_Dirac);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Delta_Dirac);

class Square_Source : public Voltage_Source
{
private:
    double v_down;
    double v_up;
    double time_delay;
    double square_width;
    double period;
public:
    Square_Source() : Voltage_Source("default_square", nullptr, nullptr, 0.0), v_down(0.0), v_up(0.0),
                     time_delay(0.0), square_width(0.0), period(0.0) {}
    Square_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double v1, double v2, double td, double pw, double per)
            : Voltage_Source(_name, _node1, _node2, 0.0), v_down(v1), v_up(v2), time_delay(td),
              square_width(pw), period(per) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& v_down, double& v_up, double& time_delay, double& square_width, double& period) const;
    void set_v_down(double val) { v_down = val; }
    void set_v_up(double val) { v_up = val; }
    void set_time_delay(double val) { time_delay = val; }
    void set_square_width(double val) { square_width = val; }
    void set_period(double val) { period = val; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }
};
CEREAL_REGISTER_TYPE(Square_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Square_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Square_Source);


class Triangular_Source : public Voltage_Source
{
private:
    double v_initial;
    double v_peak;
    double time_delay;
    double period;
public:
    Triangular_Source() : Voltage_Source("default_triangular", nullptr, nullptr, 0.0), v_initial(0.0), v_peak(0.0),
                         time_delay(0.0), period(0.0) {}
    Triangular_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double v1, double v2, double td, double per)
            : Voltage_Source(_name, _node1, _node2, 0.0), v_initial(v1), v_peak(v2), time_delay(td), period(per) {}
    double get_value_at(double time, double time_step) const override;
    void get_parameters(double& v_initial, double& v_peak, double& time_delay, double& period) const;
    void set_v_initial(double val) { v_initial = val; }
    void set_v_peak(double val) { v_peak = val; }
    void set_time_delay(double val) { time_delay = val; }
    void set_period(double val) { period = val; }
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Voltage_Source>(this));
    }
};
CEREAL_REGISTER_TYPE(Triangular_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Triangular_Source);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, Triangular_Source);



#endif //PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H