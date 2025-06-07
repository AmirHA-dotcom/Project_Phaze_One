//
// Created by amirh on 2025-06-03.
//

#ifndef PROJECT_PHAZE_ONE_CIRCUIT_H
#define PROJECT_PHAZE_ONE_CIRCUIT_H

#include "Libraries.h"
#include "Node.h"
#include "Element.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Voltage_Source.h"

class Circuit
{
private:
vector<Element*> Elements;
vector<Node*> Nodes;
public:
    void create_new_resistor(string name, string node1_name, string node2_name, double resistance);
    void create_new_capacitor(string name, string node1_name, string node2_name, double capacitance);
    void create_new_inductor(string name, string node1_name, string node2_name, double inductor);
    void create_new_voltage_sourse(string name, string node1_name, string node2_name, double voltage);
    void create_new_node();
private:
    int node_index_finder_by_name(const string& name); // returns index, if not found, returns -1.
};

#endif //PROJECT_PHAZE_ONE_CIRCUIT_H
