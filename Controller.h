//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_CONTROLLER_H
#define PROJECT_PHAZE_ONE_CONTROLLER_H

#include "Element.h"
#include "Circuit.h"
#include "Libraries.h"
#include "File_Handler.h"
#include "graphical_element.h"

class Controller {
private:
    File_Handler file_handler;
    double time_step = 0;
    double start_time = 0;
    double end_time = 0;

    int m_resistor_count = 0;
    int m_capacitor_count = 0;
    int m_inductor_count = 0;
    int m_node_count = 0;
    int m_current_source_count = 0;

    vector<unique_ptr<Graphical_Element>> m_graphical_elements;
public:
    Controller();
    vector<Circuit *> circuits;
    Circuit *circuit;

    Circuit *findCircuit(string name);
    void renameCircuit(Circuit* circuit,string name);
    void deleteCircuit(Circuit* circuit);
    Element *findElement(string name);

    Node *findNode(string name);
    void addCircuit(string name);
    void addR(string name, string Node1, string Node2, double value);

    void addC(string name, string Node1, string Node2, double value);

    void addI(string name, string Node1, string Node2, double value);

    void addD(string name, string Node1, string Node2, string model);

    void addGND(string name);

    void addVS(string name, string Node1, string Node2, double value);

    void addCS(string name, string Node1, string Node2, double value);

    void addSin(string name, string Node1, string Node2, double offset, double amplitude, double frequency);

    void addPulse1(string name, string Node1, string Node2, double period, double value); // Pulse
    void addPulse2(string name, string Node1, string Node2, double period, double value); // square Pulse
    void addPulse3(string name, string Node1, string Node2, double period, double value); // triangle Pulse
    void addPulse4(string name, string Node1, string Node2, double time); // delta Dirac Pulse
    void addVcVs(string name, string Node1, string Node2, string CtrlNode1, string CtrlNode2, double gain);

    void addVcCs(string name, string Node1, string Node2, string CtrlNode1, string CtrlNode2, double gain);

    void addCcVs(string name, string Node1, string Node2, string CtrlNode, double gain);

    void addCcCs(string name, string Node1, string Node2, string CtrlNode, double gain);

    void showNodes();
    void showCircuits();
    void showAllElements();
    void delGND(string name);
    void showResistors();

    void showInductors();

    void showCapacitors();

    void showDiodes();
    void createFile(string name, string path);
    void renameNode(string oldName, string newName);

    void tranAnalysis(double stepTime, double stopTime, double startTime, double maxTimeStep);

    void DcAnalysis(double sourceName, double startValue, double endValue, double increment);

    void tranAnalysisOrders(vector<string> orders);

    void tranAnalysis2(double stepTime, double stopTime);

    void DcAnalysisOrders(vector<string> orders);

    void saveCircuit(Circuit* circuit, string path);

    void addFileToCircuits(int fileIndex);

    void delElement(Element *element);

    void showSchematics();

    void showFile(int num);

    bool validSchematicChoice(string s);

    void handleNewFile(string path);

    bool is_files_empty();

    // GRAPHICS!!!

    void add_Graphical_Resistor(int screenX, int screenY);

    void add_Graphical_Capacitor(int screenX, int screenY);

    void add_Graphical_Inductor(int screenX, int screenY);

    void add_Graphical_Current_Source(int screenX, int screenY);

    vector<unique_ptr<Graphical_Element>>& get_graphical_elements();


};
#endif //PROJECT_PHAZE_ONE_CONTROLLER_H
