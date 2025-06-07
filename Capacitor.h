//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_CAPACITOR_H
#define PROJECT_PHAZE_ONE_CAPACITOR_H

#include "Element.h"

class Capacitor : public Element
{
public:
    Capacitor(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Capacitor, _node1, _node2, _value) {}
    double get_current(double time1, double time2);
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous) override;
};

#endif //PROJECT_PHAZE_ONE_CAPACITOR_H
