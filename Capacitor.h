//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_CAPACITOR_H
#define PROJECT_PHAZE_ONE_CAPACITOR_H

#include "Element.h"

class Capacitor : public Element
{
public:
    Capacitor() : Element("default", Element_Type::Capacitor, nullptr, nullptr, 0.0) {}
    Capacitor(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value) : Element(_name, Element_Type::Capacitor, _node1, _node2, _value) {}
    double get_current(double time1, double time_step) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;

    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Element>(this));
    }

};
CEREAL_REGISTER_TYPE(Capacitor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(Element, Capacitor);

#endif //PROJECT_PHAZE_ONE_CAPACITOR_H
