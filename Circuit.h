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
#include "Algorithems.h"
#include "VCCS.h"
#include "VCVS.h"
#include "CCCS.h"
#include "CCVS.h"
#include "Real_Diode.h"
#include "Zener_Diode.h"
#include "Current_Source.h"

class Circuit
{
private:
    vector<Element*> Elements;
    vector<Node*> Nodes;
    vector<Node*> Active_Nodes;
    int total_unknowns;
    double time_step;
    double t_start;
    double t_end;
    Algorithems algorithems;
    string name;
    bool haveGround;
    bool is_diode_added = false;
public:
    bool isGround();
    void ground(bool b);
    Circuit(string _name) : name(_name) {total_unknowns = 0;}
    void change_name(string new_name);
    string get_name() const;
    vector<pair<double, double>> get_node_voltages(string name);
    void change_value_of_element(string name ,double value);
    void change_name_of_element(string old_name ,string new_name);
    void change_name_of_node(string old_name, string new_name);
    void set_time_step(double ts);
    void set_time_start(double ts);
    void set_time_end(double ts);
    void make_node_ground(string name);
    void make_node_NOT_ground(string name);
    void create_new_resistor(string name, string node1_name, string node2_name, double resistance);
    void create_new_capacitor(string name, string node1_name, string node2_name, double capacitance);
    void create_new_inductor(string name, string node1_name, string node2_name, double inductor);
    void create_new_current_source(string name, string node1_name, string node2_name, double current);
    void create_new_VCVS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain);
    void create_new_CCVS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain);
    void create_new_VCCS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain);
    void create_new_CCCS(string name, string node1_name, string node2_name, string ctrl1, string ctrl2, double gain);
    void create_new_real_diode(string name, string node1_name, string node2_name, double dummy_number);
    void create_new_zener_diode(string name, string node1_name, string node2_name, double dummy_number);
    void create_new_DC_voltage_source(string name, string node1_name, string node2_name, double voltage);
    void create_new_Sin_voltage_source(string name, string node1_name, string node2_name, double offset, double amplitude, double frequency);
    void create_new_Pulse_voltage_source(string name, string node1_name, string node2_name, double period, double value);
    void create_new_Square_voltage_source(string name, string node1_name, string node2_name, double period, double value);
    void create_new_Triangle_voltage_source(string name, string node1_name, string node2_name, double period, double value);
    void create_new_Delta_voltage_source(string name, string node1_name, string node2_name, double time);
    void analyse_data();
    const vector<Element*> get_Elements();
    const vector<Node*> get_Nodes();
    const vector<Element*> get_Elements_of_type(Element_Type type);
    void transient();
    void transient_linear();
    void transient_NR();
    void delete_element(string name);
    void delete_node(Node* node_to_delete);
    ~Circuit();

    Node* create_new_node(const string& name);
    void addElement(Element* new_element);

private:
    int node_index_finder_by_name(const string& name); // returns index, if not found, returns -1.
    int element_index_finder_by_name(const string& name); // returns index, if not found, returns -1.
};

class SubCircuit : public Circuit
{
private:
    Node* input;
    Node* output;
public:
    SubCircuit(string _name, Node* _input, Node* _output) : Circuit(_name), input(_input), output(_output) {}
    Node* getInput() const;
    Node* getOutput() const;
    void setInput(Node* newInput);
    void setOutput(Node* newOutput);

};

#endif //PROJECT_PHAZE_ONE_CIRCUIT_H
