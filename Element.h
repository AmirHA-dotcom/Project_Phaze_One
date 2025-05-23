//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_ELEMENT_H
#define PROJECT_PHAZE_ONE_ELEMENT_H

#include <iostream>
#include "string"
#include "Node.h"

using namespace std;

enum class Element_Type{Resistor, Capacitor, Inductor, Diode, Voltage_Source, Current_Source, Ground};

class Element
{
protected:
    string name;
    Element_Type type;
    Node* node1;
    Node* node2;
    double value;
public:
    Element(string _name, Element_Type _type, Node* n1, Node* n2);
    string get_name() const;
    Element_Type get_element_type();
    pair<Node*, Node*> get_nodes();
    double get_voltage();
    virtual double get_current() = 0;
    double get_value() {return value;}
    virtual void display_info() = 0;
};


#endif //PROJECT_PHAZE_ONE_ELEMENT_H
