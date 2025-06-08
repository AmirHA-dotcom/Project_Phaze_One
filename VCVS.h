//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_VCVS_H
#define PROJECT_PHAZE_ONE_VCVS_H

#include "Element.h"

class VCVS : Element
{
private:
    Node* ctrl_node1;
    Node* ctrl_node2;
    int aux_index;
public:
    VCVS(string _name, Element_Type _type, Node* _node1, Node* _node2, double _value, Node* ctrl1, Node* ctrl2) :
    Element(_name, Element_Type::VC_Voltage_Source, _node1, _node2, _value) {ctrl_node1 = ctrl1; ctrl_node2 = ctrl2;}
    void set_aux_index(int i);
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
};


#endif //PROJECT_PHAZE_ONE_VCVS_H
