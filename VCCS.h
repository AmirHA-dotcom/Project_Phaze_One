//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_VCCS_H
#define PROJECT_PHAZE_ONE_VCCS_H

#include "Element.h"


class VCCS : public Element
{
private:
    Node* ctrl_node1;
    Node* ctrl_node2;
public:
    VCCS(string _name, Node* _node1, Node* _node2, double _value, Node* ctrl1, Node* ctrl2) :
    Element(_name, Element_Type::VC_Current_Source, _node1, _node2, _value) {ctrl_node1 = ctrl1; ctrl_node2 = ctrl2;}
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
};


#endif //PROJECT_PHAZE_ONE_VCCS_H
