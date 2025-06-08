//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

void Controller::addCircuit(string name){

}
Circuit* Controller::findCircuit(string name){

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
}

void Controller::addVS (string name, string Node1, string Node2, double value){
    circuit->create_new_voltage_source(name, Node1, Node2, value);
}

void Controller::addCS (string name, string Node1, string Node2, double value){
    circuit->create_new_current_source(name, Node1, Node2, value);
}

void Controller::addSin (string name, string Node1, string Node2, double offset, double amplitude, double frequency){
    // Implementation
}

void Controller::addPulse1(string name, string Node1, string Node2, double time, double value){
    // Implementation
}

void Controller::addPulse2(string name, string Node1, string Node2, double period, double value){
    // Implementation
}

void Controller::addPulse3(string name, string Node1, string Node2, double period, double value){
    // Implementation
}

void Controller::addVcVs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){
    circuit->create_new_VCVS(name, Node1, Node2, gain);
}

void Controller::addVcCs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){
    circuit->create_new_VCCS(name, Node1, Node2, gain);
}

void Controller::addCcVs (string name, string Node1, string Node2,string CtrlNode, double gain){
    circuit->create_new_CCVS(name, Node1, Node2, gain);
}

void Controller::addCcCs (string name, string Node1, string Node2,string CtrlNode, double gain){
    circuit->create_new_CCCS(name, Node1, Node2, gain);
}

void Controller::showNodes(){
    // Implementation
}
void Controller::delGND(string name){

}
void Controller::showAllElements(){
    // Implementation
}
void Controller::showCircuits(){

}
void Controller::showResistors(){
    // Implementation
}

void Controller::showInductors(){
    // Implementation
}

void Controller::showCapacitors(){
    // Implementation
}

void Controller::showDiodes(){
    // Implementation
}

void Controller::renameNode(string oldName, string newName){
    // Implementation
}

void Controller::tranAnalysis(double stepTime, double stopTime, double startTime, double maxTimeStep){
    // Implementation
}

void Controller::DcAnalysis(double sourceName, double startValue, double endValue, double increment){
    // Implementation
}

void Controller::tranAnalysisOrders(vector<string> orders){
    // Implementation
}

void Controller::DcAnalysisOrders(vector<string> orders){
    // Implementation
}

void Controller::delElement (Element* element){
    // Implementation
}

void Controller::showSchematics(){
    // Implementation
}

void Controller::showFile(int num){
    // Implementation
}

bool Controller::validSchematicChoice(string s){
    return false; // Placeholder
}

void Controller::handleNewFile(string path){
    // Implementation
}
