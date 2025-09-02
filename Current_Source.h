//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_CURRENT_SOURCE_H
#define PROJECT_PHAZE_ONE_CURRENT_SOURCE_H

#include "Element.h"

class Current_Source : public Element
{
public:
    Current_Source(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value) : Element(_name, Element_Type::Current_Source, _node1, _node2, _value) {}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    double get_current(double time, double time_step) override;
    template <class Archive>
    void serialize(Archive& ar) {
        ar(cereal::base_class<Element>(this));
    }
};


#endif //PROJECT_PHAZE_ONE_CURRENT_SOURCE_H
