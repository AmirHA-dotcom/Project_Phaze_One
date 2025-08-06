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
    vector<tuple<string,int, Rotation,pair<int,int>>> subs;
    int total_unknowns;
    double time_step;
    double t_start;
    double t_end;
    Algorithems algorithems;
    string name;
    bool haveGround;
    bool is_diode_added = false;
    vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>> acVoltage; // Node and pair of indices for AC voltage {magnitude, phase}
    vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>> phaseVoltage; // Node and pair of indices for AC voltage {magnitude, phase}
public:
    void setAcVoltage (const vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>>& acVoltageList) {
        acVoltage = acVoltageList;
    }
    vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>> getAcVoltage() { return acVoltage; }

    void setPhaseVoltage (const vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>>& phaseVoltageList) {
            phaseVoltage = phaseVoltageList;
    }
    vector<pair<Node*, tuple<vector<double>, vector<double>, vector<double>>>> getPhaseVoltage() { return phaseVoltage; }

    vector<tuple<string,int, Rotation,pair<int,int>>> getSubs() const { return subs; }
    bool isGround() const;
    void ground(bool b);

    Circuit(string _name)
            : name(_name), total_unknowns(0), haveGround(false)
    {}

    Circuit()
            : name(""), total_unknowns(0), haveGround(false)
    {}

    void change_name(const string& new_name);
    string get_name() const;
    vector<pair<double, double>> get_node_voltages(const string& name) const;
    void change_value_of_element(const string& name, double value);
    void change_name_of_element(const string& old_name, const string& new_name);
    void change_name_of_node(const string& old_name, const string& new_name);
    void set_time_step(double ts);
    void set_time_start(double ts);
    void set_time_end(double ts);
    void make_node_ground(const string& name);
    void make_node_NOT_ground(const string& name);
    void create_new_resistor(const string& name, const string& node1_name, const string& node2_name, double resistance);
    void create_new_capacitor(const string& name, const string& node1_name, const string& node2_name, double capacitance);
    void create_new_inductor(const string& name, const string& node1_name, const string& node2_name, double inductor);
    void create_new_current_source(const string& name, const string& node1_name, const string& node2_name, double current);
    void create_new_VCVS(const string& name, const string& node1_name, const string& node2_name, const string& ctrl1, const string& ctrl2, double gain);
    void create_new_CCVS(const string& name, const string& node1_name, const string& node2_name, const string& ctrl1, const string& ctrl2, double gain);
    void create_new_VCCS(const string& name, const string& node1_name, const string& node2_name, const string& ctrl1, const string& ctrl2, double gain);
    void create_new_CCCS(const string& name, const string& node1_name, const string& node2_name, const string& ctrl1, const string& ctrl2, double gain);
    void create_new_real_diode(const string& name, const string& node1_name, const string& node2_name, double dummy_number);
    void create_new_zener_diode(const string& name, const string& node1_name, const string& node2_name, double dummy_number);
    void create_new_DC_voltage_source(const string& name, const string& node1_name, const string& node2_name, double voltage);
    void create_new_Sin_voltage_source(const string& name, const string& node1_name, const string& node2_name, double offset, double amplitude, double frequency);
    void create_new_Pulse_voltage_source(const string& name, const string& node1_name, const string& node2_name, double period, double value);
    void create_new_Square_voltage_source(const string& name, const string& node1_name, const string& node2_name, double period, double value);
    void create_new_Triangle_voltage_source(const string& name, const string& node1_name, const string& node2_name, double period, double value);
    void create_new_Delta_voltage_source(const string& name, const string& node1_name, const string& node2_name, double time);
    void analyse_data();

    const vector<Element*> get_Elements() const;
    const vector<Node*> get_Nodes() const;

    void setNodes(const vector<Node*>& new_nodes) { Nodes = new_nodes; }
    void setElements(const vector<Element*>& new_elements) { Elements = new_elements; }

    const vector<Element*> get_Elements_of_type(Element_Type type) const;
    void transient();
    void transient_linear();
    void transient_NR();
    void delete_element(const string& name);
    void delete_node(Node* node_to_delete);
    ~Circuit();

    Element* findElement(const string& name) const;
    Node* findNode(const string& name) const;
    void checkHaveGround();
    void addNode(Node* node);
    Node* create_new_node(const string& name);
    void addElement(Element* new_element);
    int node_index_finder_by_name(const string& name) const;
    int element_index_finder_by_name(const string& name) const;
};

class SubCircuit : public Circuit
{
private:
    Node* input;
    Node* output;
    int x;
    int y;
    Rotation rotation;
public:
    SubCircuit()
            : Circuit(), input(nullptr), output(nullptr), x(0), y(0), rotation(Rotation::Right)
    {}

    SubCircuit(const string& _name, Node* _input, Node* _output)
            : Circuit(_name), input(_input), output(_output), x(0), y(0), rotation(Rotation::Right)
    {}

    SubCircuit(string _name, Circuit* circuit, Node* _input, Node* _output)
            : Circuit(_name), input(_input), output(_output)
    {
        rotation = Rotation::Right;
        this->setNodes(circuit->get_Nodes());
        this->setElements(circuit->get_Elements());
    }
    Node* getInput();
    Node* getOutput();
    void setInput(Node* newInput);
    void setOutput(Node* newOutput);

    void set_coordinates(int x_, int y_) { x = x_; y = y_; }
    int get_rotation_as_int();
    void set_rotation_by_int(int r);
    void set_rotation(Rotation r) { rotation = r; }
};

#endif //PROJECT_PHAZE_ONE_CIRCUIT_H
