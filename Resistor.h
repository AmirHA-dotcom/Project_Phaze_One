//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_RESISTOR_H
#define PROJECT_PHAZE_ONE_RESISTOR_H

#include "Element.h"

class Resistor : public Element
{
public:
    Resistor() : Element("default", Element_Type::Resistor, nullptr, nullptr, 0.0) {}
    Resistor(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value) : Element(_name, Element_Type::Resistor, _node1, _node2, _value) {}
    double get_current(double time, double time_step) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Element>(this));
    }
};

CEREAL_REGISTER_TYPE(Resistor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Resistor);

#endif //PROJECT_PHAZE_ONE_RESISTOR_H
