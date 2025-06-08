//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

void addCircuit(string name){

}
Circuit* findCircuit(string name){

}
Element* Controller::findElement (string name){
    return nullptr;
}

Element* Controller::findNode (string name){
    return nullptr;
}

void Controller::addR (string name, string Node1, string Node2, double value){
    // Implementation
}

void Controller::addC (string name, string Node1, string Node2, double value){
    // Implementation
}

void Controller::addI (string name, string Node1, string Node2, double value){
    // Implementation
}

void Controller::addD (string name, string Node1, string Node2, string model){
    // Implementation
}

void Controller::addGND (string name){
    // Implementation
}

void Controller::addVS (string name, string Node1, string Node2, double value){
    // Implementation
}

void Controller::addCS (string name, string Node1, string Node2, double value){
    // Implementation
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
    // Implementation
}

void Controller::addVcCs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){
    // Implementation
}

void Controller::addCcVs (string name, string Node1, string Node2,string CtrlNode, double gain){
    // Implementation
}

void Controller::addCcCs (string name, string Node1, string Node2,string CtrlNode, double gain){
    // Implementation
}

void Controller::showNodes(){
    // Implementation
}
void delGND(string name){

}
void Controller::showAllElements(){
    // Implementation
}
void showCircuits(){

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
