//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_CCCS_H
#define PROJECT_PHAZE_ONE_CCCS_H

#include "Element.h"


class CCCS : public Element
{
private:
    shared_ptr<Node> ctrl_node1;
    shared_ptr<Node> ctrl_node2;
public:
    CCCS(string _name, shared_ptr<Node> _node1, shared_ptr<Node> _node2, double _value, shared_ptr<Node> ctrl1, shared_ptr<Node> ctrl2) :
    Element(_name, Element_Type::CC_Current_source, _node1, _node2, _value) {ctrl_node1 = ctrl1; ctrl_node2 = ctrl2;}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    double get_current(double time, double time_step) override;
};


#endif //PROJECT_PHAZE_ONE_CCCS_H
