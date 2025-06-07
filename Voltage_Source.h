//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H
#define PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H

#include "Element.h"

class Voltage_Source : public Element
{
private:
    int aux_index;
public:
    Voltage_Source(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Voltage_Source, _node1, _node2, _value) {}
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous);
};


#endif //PROJECT_PHAZE_ONE_VOLTAGE_SOURCE_H
