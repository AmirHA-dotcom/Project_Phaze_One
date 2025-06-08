//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

Element* Controller::findElement (string name){
    return nullptr;
}
Element* Controller::findNode (string name){
    return nullptr;
}
void Controller::addR (string name, string Node1, string Node2, double value){

}
void Controller::addC (string name, string Node1, string Node2, double value){

}
void Controller::addI (string name, string Node1, string Node2, double value){

}
void Controller::addD (string name, string Node1, string Node2, string model){

}
void Controller::addGND (string name){

}
void Controller::addVS (string name, string Node1, string Node2, double value){

}
void Controller::addCS (string name, string Node1, string Node2, double value){

}
void Controller::addSin (string name, string Node1, string Node2, double offset, double amplitude, double frequency){

}
void Controller::addPulse1(string name, string Node1, string Node2, double time, double value)// Pulse with one edge پله
{
    // Implementation for adding a pulse with one edge
}
void Controller::addPulse2(string name, string Node1, string Node2, double period, double value) // square Pulse
{

}
void Controller::addPulse3(string name, string Node1, string Node2, double period, double value) // triangle Pulse
{

}
void Controller::addVcVs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){

}
void Controller::addVcCs (string name, string Node1, string Node2,string CtrlNode1, string CtrlNode2, double gain){

}
void Controller::addCcVs (string name, string Node1, string Node2,string CtrlNode, double gain){

}
void Controller::addCcCs (string name, string Node1, string Node2,string CtrlNode, double gain){

}
void Controller::showNodes(){

}
void Controller::showAllElements(){

}
void Controller::showResistors(){

}
void Controller::showInductors(){

}
void Controller::showCapacitors(){

}
void Controller::showDiodes(){

}
void Controller::renameNode(string oldName, string newName){

}
void Controller::tranAnalysis(double stepTime, double stopTime, double startTime, double maxTimeStep){

}
void Controller::DcAnalysis(double sourceName, double startValue, double endValue, double increment){

}
void Controller::tranAnalysisOrders(vector<string> orders){

}
void Controller::DcAnalysisOrders(vector<string> orders){

}



void Controller::delElement (Element* element){

}

void Controller::showSchematics(){

}
void Controller::showFile(int num){

}
bool Controller::validSchematicChoice(string s){

}
void Controller::handleNewFile(string path){

}