//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_ELEMENT_H
#define PROJECT_PHAZE_ONE_ELEMENT_H

#include "Libraries.h"
#include "Node.h"

enum class Element_Type{Resistor, Capacitor, Inductor, Diode, Voltage_Source, Current_Source, VC_Voltage_Source, VC_Current_Source, CC_Voltage_Source, CC_Current_source};

class Element
{
protected:
    string name;
    Element_Type type;
    Node* node1;
    Node* node2;
    double value;
public:
    Element(string _name, Element_Type _type, Node* n1, Node* n2, double _value);
    string get_name() const;
    Element_Type get_element_type();
    pair<Node*, Node*> get_nodes();
    double get_voltage();
    double get_value() const {return value;}
    virtual void display_info() = 0;
    virtual void change_value(double new_value) = 0;
    virtual void change_name(string new_name) = 0;
    virtual void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous) = 0;
};


#endif //PROJECT_PHAZE_ONE_ELEMENT_H
