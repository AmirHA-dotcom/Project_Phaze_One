//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_VCVS_H
#define PROJECT_PHAZE_ONE_VCVS_H

#include "Element.h"

class VCVS : public Element
{
private:
    Node* ctrl_node1;
    Node* ctrl_node2;
    int aux_index;
    vector<pair<double, double>> currents;  // current time
public:
    VCVS(string _name, Node* _node1, Node* _node2, double _value, Node* ctrl1, Node* ctrl2) :
    Element(_name, Element_Type::VC_Voltage_Source, _node1, _node2, _value) {ctrl_node1 = ctrl1; ctrl_node2 = ctrl2;}
    void set_aux_index(int i);
    void stamp(double current_time, double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
    void display_info() override;
    void change_value(double new_value) override;
    void change_name(string new_name) override;
    void set_current(double current, double time);
    double get_current(double time, double time_step) override;
    int get_aux_index() const;
};


#endif //PROJECT_PHAZE_ONE_VCVS_H
