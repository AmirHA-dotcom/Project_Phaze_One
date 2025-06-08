#ifndef PROJECT_PHAZE_ONE_ZENER_DIODE_H
#define PROJECT_PHAZE_ONE_ZENER_DIODE_H

#include "Element.h"

class Zener_Diode : public Element
{
public:
    Zener_Diode(string _name, Node* _node1, Node* _node2, double _value) : Element(_name, Element_Type::Zener_Diode, _node1, _node2, _value) {}
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double>& x_k, const vector<double>& x_previous) override;
};





#endif