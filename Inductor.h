//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_INDUCTOR_H
#define PROJECT_PHAZE_ONE_INDUCTOR_H

#include "Element.h"

class Inductor : public Element
{
    int aux_index; // this is added so it can be used in the solving matrix
public:
    Inductor(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Inductor, _node1, _node2, _value) {}
    double get_current(double time1, double time2);
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    void set_aux_index(int i);
    int get_aux_index();
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous) override;
};


#endif //PROJECT_PHAZE_ONE_INDUCTOR_H
