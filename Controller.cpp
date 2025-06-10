//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

void Controller::addCircuit(string name){
    auto* c = new Circuit(name);
    circuits.push_back(c);
}
Circuit* Controller::findCircuit(string name){
    for (auto i : circuits) {
        if(i->get_name() == name)
            return i;
    }
    return nullptr;
}
void Controller::renameCircuit(Circuit* circuit,string name){
    circuit->change_name(name);
}
void Controller::deleteCircuit(Circuit* circuit){
    circuits.erase(
            std::remove(circuits.begin(), circuits.end(), circuit),
            circuits.end()
    );
}
Element* Controller::findElement (string name){
    for (auto& e : circuit->get_Elements())
    {
        if (e->get_name() == name)
            return e;
    }
    return nullptr;
}

Node* Controller::findNode (string name){
    for (auto& n : circuit->get_Nodes())
    {
        if (n->get_name() == name)
            return n;
    }
    return nullptr;
}

void Controller::addR (string name, string Node1, string Node2, double value){
    //cout << "called controller to add reisitor" << endl;
    circuit->create_new_resistor(name, Node1, Node2, value);
}

void Controller::addC (string name, string Node1, string Node2, double value){
    circuit->create_new_capacitor(name, Node1, Node2, value);
}

void Controller::addI (string name, string Node1, string Node2, double value){
    circuit->create_new_inductor(name, Node1, Node2, value);
}

void Controller::addD (string name, string Node1, string Node2, string model){
    if (model == "D")
        circuit->create_new_real_diode(name, Node1, Node2, 0.0);
    else if (model == "Z")
        circuit->create_new_zener_diode(name, Node1, Node2, 0.0);
    else
        cerr << "Undefined Diode model" << endl;
}

void Controller::addGND (string name){
    circuit->make_node_ground(name);
    circuit->ground(true);
}

void Controller::addVS (string name, string Node1, string Node2, double value){
    //cout << "called controller to add dc source" << endl;
    circuit->create_new_DC_voltage_source(name, Node1, Node2, value);
}

void Controller::addCS (string name, string Node1, string Node2, double value){
    //cout << "called controller to add current source" << endl;
    circuit->create_new_current_source(name, Node1, Node2, value);
}

void Controller::addSin (string name, string Node1, string Node2, double offset, double amplitude, double frequency){
    circuit->create_new_Sin_voltage_source(name, Node1, Node2, offset, amplitude, frequency);
}

void Controller::addPulse1(string name, string Node1, string Node2, double period, double value) {
    circuit->create_new_Pulse_voltage_source(name, Node1, Node2, period, value);
}

void Controller::addPulse2(string name, string Node1, string Node2, double period, double value){
    circuit->create_new_Square_voltage_source(name, Node1, Node2, period, value);
}

void Controller::addPulse3(string name, string Node1, string Node2, double period, double value){
    circuit->create_new_Triangle_voltage_source(name, Node1, Node2, period, value);
}
void Controller::addPulse4(string name, string Node1, string Node2, double time) {
    circuit->create_new_Delta_voltage_source(name, Node1, Node2, time);
}
void Controller::addVcVs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){
    circuit->create_new_VCVS(name, Node1, Node2, CtrlNode1, CtrlNode2, gain);
}

void Controller::addVcCs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){
    circuit->create_new_VCCS(name, Node1, Node2, CtrlNode1, CtrlNode2, gain);
}

void Controller::addCcVs (string name, string Node1, string Node2,string CtrlNode, double gain){
    //circuit->create_new_CCVS(name, Node1, Node2, CtrlNode1, CtrlNode2, gain);
}

void Controller::addCcCs (string name, string Node1, string Node2,string CtrlNode, double gain){
    //circuit->create_new_CCCS(name, Node1, Node2, CtrlNode1, CtrlNode2, gain);
}

void Controller::showNodes(){
    vector<Node*> nodes = circuit->get_Nodes();
    if (nodes.empty())
    {
        cout << "No available nodes" << endl;
        return;
    }
    cout << "Available nodes:" << endl;
    for (const auto& n : nodes)
    {
        if (n == nodes[nodes.size() - 1])
        {
            cout << n->get_name() << endl;
        }
        else
        {
            cout << n->get_name() << ", ";
        }
    }
}
void Controller::delGND(string name){
    circuit->make_node_NOT_ground(name);
    bool check = false;
    for (auto i : circuit->get_Nodes())
        if(i->is_the_node_ground())
            check = true;
    if (!check)
        circuit->ground(false);
}
void Controller::showAllElements(){
    vector<Element*> elements = circuit->get_Elements();
    if (elements.empty())
    {
        cout << "No elements have been added yet" << endl;
        return;
    }
    cout << "Elements List:" << endl;
    for (const auto& e : elements)
    {
        if (e == elements[elements.size() - 1])
        {
            cout << e->get_name() << endl;
        }
        else
        {
            cout << e->get_name() << ", ";
        }
    }
}
void Controller::showCircuits(){
    if (circuits.empty())
    {
        cout << "No circuits have been added yet !!!" << endl;
        return;
    }
    cout << "List Of Circuits:" << endl;
    for (const auto& c : circuits)
        cout << "- " << c->get_name() << endl;
}
void Controller::showResistors(){
    vector<Element*> elements = circuit->get_Elements_of_type(Element_Type::Resistor);
    if (elements.empty())
    {
        cout << "No elements have been added yet" << endl;
        return;
    }
    cout << "Resistors List:" << endl;
    for (const auto& e : elements)
    {
        if (e == elements[elements.size() - 1])
        {
            cout << e->get_name() << endl;
        }
        else
        {
            cout << e->get_name() << ", ";
        }
    }
}

void Controller::showInductors(){
    vector<Element*> elements = circuit->get_Elements_of_type(Element_Type::Inductor);
    if (elements.empty())
    {
        cout << "No elements have been added yet" << endl;
        return;
    }
    cout << "Inductors List:" << endl;
    for (const auto& e : elements)
    {
        if (e == elements[elements.size() - 1])
        {
            cout << e->get_name() << endl;
        }
        else
        {
            cout << e->get_name() << ", ";
        }
    }
}

void Controller::showCapacitors(){
    vector<Element*> elements = circuit->get_Elements_of_type(Element_Type::Capacitor);
    if (elements.empty())
    {
        cout << "No elements have been added yet" << endl;
        return;
    }
    cout << "Capacitors List:" << endl;
    for (const auto& e : elements)
    {
        if (e == elements[elements.size() - 1])
        {
            cout << e->get_name() << endl;
        }
        else
        {
            cout << e->get_name() << ", ";
        }
    }}

void Controller::showDiodes(){
    vector<Element*> elements = circuit->get_Elements_of_type(Element_Type::Real_Diode);
    if (elements.empty())
    {
        cout << "No elements have been added yet" << endl;
        return;
    }
    cout << "Diodes List:" << endl;
    for (const auto& e : elements)
    {
        if (e == elements[elements.size() - 1])
        {
            cout << e->get_name() << endl;
        }
        else
        {
            cout << e->get_name() << ", ";
        }
    }}

void Controller::renameNode(string oldName, string newName){
    Node* node = findNode(oldName);
    if (node == nullptr) {
        cerr << "ERROR: Node " << oldName << " not found." << endl;
        return;
    }
    node->change_name(newName);
    cout << "SUCCESS: Node renamed from " << oldName << " to " << newName << endl;
}

void Controller::tranAnalysis(double stepTime, double stopTime, double startTime, double maxTimeStep){
    time_step = stepTime;
    start_time = startTime;
    end_time = stopTime;
    circuit->set_time_start(startTime);
    circuit->set_time_end(end_time);
    circuit->set_time_step(time_step);
    circuit->analyse_data();
    circuit->transient();
}

void Controller::DcAnalysis(double sourceName, double startValue, double endValue, double increment){
    // Implementation
}

void Controller::tranAnalysisOrders(vector<string> orders){
    //cout << "dodo  " <<orders[0].substr(2, orders[0].size() - 3) << endl;
    vector<string> nodeVoltages;
    vector<string> elementCurrents;
    for (const auto& order : orders) {
        if (order.substr(0, 2) == "V(") {
            nodeVoltages.push_back(order.substr(2, order.size() - 3));
        } else if (order.substr(0, 2) == "I(") {
            elementCurrents.push_back(order.substr(2, order.size() - 3));
        } else {
            cerr << "Invalid order: " << order << endl;
        }
    }
    if (nodeVoltages.empty() && elementCurrents.empty()) {
        cerr << "No valid orders provided for transient analysis." << endl;
        return;
    }
    for (auto i : nodeVoltages) {
        Node* node = findNode(i);

        //cout << node->get_all_voltages().size() << endl;
        //cout << circuit->get_node_voltages(i).size() << endl;

        if (node == nullptr) {
            cerr << "Node " << i << " not found." << endl;
            return;
        }
        else {
            if (node->is_the_node_ground()){
                cout << "This is GND Voltage is = 0" << endl;
            }
            else
                for (auto jj: node->get_all_voltages())
                    cout << " " << jj.first;
            cout << endl;
        }
    }
    for (auto i : elementCurrents) {
        Element* element = findElement(i);
        if (element == nullptr) {
            cerr << "Element " << i << " not found." << endl;
            return;
        }
        else
            for(double t = start_time; t < end_time; t += time_step)
                cout << element->get_current(t, time_step) << " ";
            cout << endl;
    }
}

void Controller::DcAnalysisOrders(vector<string> orders){
    // Implementation
}

void Controller::delElement (Element* element){
    circuit->delete_element(element->get_name());
}

void Controller::showSchematics(){
    vector<string> file_names = file_handler.get_file_names();
    for (int i = 0; i < file_names.size(); i++)
        cout << i + 1 << "-" << file_names[i] << endl;
}

void Controller::showFile(int num){
    file_handler.set_file_index(num - 1);
    file_handler.show_current_file();
}

bool Controller::validSchematicChoice(string s){
    if (stoi(s) <= 0 || stoi(s) > file_handler.get_file_names().size())
    return false; // Placeholder
}

void Controller::handleNewFile(string path){
    file_handler.add_file(path);
}

bool Controller::is_files_empty()
{
    return file_handler.get_file_names().empty();
}
