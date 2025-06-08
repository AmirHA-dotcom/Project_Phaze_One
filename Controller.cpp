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


void Controller::delElement (Element* element){

}

void Controller::showSchematics(){

}
void Controller::showFile(int num){

}
bool Controller::validSchematicChoice(string s){
    return false;
}
void Controller::handleNewFile(string path){

}