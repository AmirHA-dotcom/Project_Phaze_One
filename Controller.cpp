//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

Controller::Controller() {
    // Initialize the circuit member pointer
    circuit = nullptr; // Start it as null just in case

    // Create a new default circuit and make it the active one
    circuit = new Circuit("default_circuit");
    circuits.push_back(circuit);
}

void Controller::addCircuit(string name){
    auto* c = new Circuit(name);
    circuits.push_back(c);
}
void Controller::addSubCircuit(string name, Node* inputNode, Node* outputNode) {
    auto* sc = new SubCircuit(name,inputNode,outputNode);
    subCircuits.push_back(sc);
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
            remove(circuits.begin(), circuits.end(), circuit),
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
void Controller::tranAnalysis2(double stepTime, double stopTime){
    time_step = stepTime;
    start_time = 0;
    end_time = stopTime;
    circuit->set_time_start(start_time);
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



#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>

void Controller::saveCircuit(Circuit* circuit, string path) {
    if (!circuit) {
        cerr << "Error: Null circuit pointer provided" << endl;
        return;
    }

    // Ensure path ends with appropriate separator
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
#ifdef _WIN32
        path += "\\";
#else
        path += "/";
#endif
    }
    string fullPath = path + circuit->get_name() + ".txt";
    filesystem::create_directories(path);

    ofstream file(fullPath);
    if (!file.is_open()) {
        cerr << "Error: could not create file at " << fullPath << endl;
        return;
    }

    // Write circuit name
    file << circuit->get_name() << ": " << "\n";

    // Helper function to format numerical values with appropriate suffixes
    auto formatValue = [](double value) -> string {
        const vector<pair<string, double>> suffixes = {
                {"t", 1e12}, {"g", 1e9}, {"meg", 1e6}, {"k", 1e3},
                {"", 1.0}, {"m", 1e-3}, {"u", 1e-6}, {"n", 1e-9}, {"p", 1e-12}, {"f", 1e-15}
        };
        for (const auto& suffix : suffixes) {
            if (abs(value) >= suffix.second || suffix.second == 1.0) {
                double scaled = value / suffix.second;
                string result = to_string(scaled);
                result.erase(result.find_last_not_of('0') + 1);
                if (result.back() == '.') result.pop_back();
                return result + suffix.first;
            }
        }
        return to_string(value);
    };

    // Iterate through circuit components
    for (auto* component : circuit->get_Elements()) {
        string line;
        Element_Type type = component->get_type();
        string element_name = component->get_name();
        const pair<Node*, Node*> nodes = component->get_nodes();
        string node1 = nodes.first ? nodes.first->get_name() : "0";
        string node2 = nodes.second ? nodes.second->get_name() : "0";
        double value = component->get_value();

        switch (type) {
            case Element_Type::Resistor:
                line = "R" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            case Element_Type::Capacitor:
                line = "C" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            case Element_Type::Inductor:
                line = "L" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            case Element_Type::Current_Source:
                line = "I" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            case Element_Type::Voltage_Source:
                // Assume DC voltage source, as Element doesn't store sourceType
                line = "V" + element_name + " " + node1 + " " + node2 + " DC " + formatValue(value);
                break;
            case Element_Type::VC_Voltage_Source:
                // Control nodes not in Element; assume "0 0" or handle via derived class if available
                line = "E" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value);
                break;
            case Element_Type::CC_Current_source:
                line = "F" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value);
                break;
            case Element_Type::VC_Current_Source:
                line = "G" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value);
                break;
            case Element_Type::CC_Voltage_Source:
                line = "H" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value);
                break;
            case Element_Type::Real_Diode:
                line = "D" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            case Element_Type::Zener_Diode:
                line = "Z" + element_name + " " + node1 + " " + node2 + " " + formatValue(value);
                break;
            default:
                cerr << "Unsupported component type: " << static_cast<int>(type) << endl;
                continue;
        }
        file << line << "\n";
    }

    // Write ground directive if present
    if (circuit->isGround()) {
        file << "GND GND\n";
    }

    // End of circuit
    file << ".END\n";

    file.close();
}

double Value(const string& inputRaw) {
    string input;
    for (char c : inputRaw) {
        if (c == ',') input += '.';
        else input += c;
    }

    static const unordered_map<string, double> suffixes = {
            {"f", 1e-15}, {"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3},
            {"k", 1e3}, {"meg", 1e6}, {"g", 1e9}, {"t", 1e12}
    };

    // پیدا کردن جایی که suffix شروع میشه (یعنی جایی که دیگه e-style number تموم شده)
    size_t pos = 0;
    bool eSeen = false;
    while (pos < input.size()) {
        char c = input[pos];
        if (isdigit(c) || c == '.' || c == '-' || c == '+') {
            pos++;
        } else if ((c == 'e' || c == 'E') && !eSeen) {
            eSeen = true;
            pos++;
        } else {
            break;
        }
    }

    string numberPart = input.substr(0, pos);
    string suffixPart = input.substr(pos);

    double number = stod(numberPart);

    for (char& c : suffixPart) c = tolower(c);
    if (!suffixPart.empty()) {
        auto it = suffixes.find(suffixPart);
        if (it != suffixes.end()) {
            number *= it->second;
        } else {
            throw invalid_argument("Unknown suffix: " + suffixPart);
        }
    }

    return number;
}

Circuit* textToCircuit(string Name, const vector<vector<string>>& lines) {
    Circuit* circuit = new Circuit(Name);

    auto groundCheck = [&](const string& node) {
        if (node == "0" || node == "GND") {
            circuit->make_node_ground(node);
        }
    };

    for (const auto& tokens : lines) {
        if (tokens.empty()) continue;

        string type = tokens[0];
        char prefix = toupper(type[0]);
        string element_name = type.substr(1); // Remove type prefix (e.g., R1 → 1)

        try {
            if(tokens == lines[0] && tokens[0][tokens.size()-1] == '.' && tokens.size() == 1) {
                circuit->change_name(tokens[0].substr(0,tokens[0].size()-1));
            }
            else if (prefix == 'R' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_resistor(element_name, n1, n2, val);
            }
            else if (prefix == 'C' && tokens.size() >= 4 && type != "CCVS" && type != "CCCS") {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_capacitor(element_name, n1, n2, val);
            }
            else if (prefix == 'L' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_inductor(element_name, n1, n2, val);
            }
            else if (prefix == 'I' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_current_source(element_name, n1, n2, val);
            }
            else if (prefix == 'V' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                groundCheck(n1); groundCheck(n2);
                if (tokens[3] == "DC" && tokens.size() >= 5) {
                    double val = Value(tokens[4]);
                    circuit->create_new_DC_voltage_source(element_name, n1, n2, val);
                }
                else if (tokens[3] == "SIN" && tokens.size() >= 7) {
                    double offset = Value(tokens[4]);
                    double amp = Value(tokens[5]);
                    double freq = Value(tokens[6]);
                    circuit->create_new_Sin_voltage_source(element_name, n1, n2, offset, amp, freq);
                }
                else if (tokens[3] == "PULSE" && tokens.size() >= 6) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Pulse_voltage_source(element_name, n1, n2, period, val);
                }
                else if (tokens[3] == "SQUARE" && tokens.size() >= 6) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Square_voltage_source(element_name, n1, n2, period, val);
                }
                else if (tokens[3] == "TRIANGLE" && tokens.size() >= 6) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Triangle_voltage_source(element_name, n1, n2, period, val);
                }
                else if (tokens[3] == "DELTA" && tokens.size() >= 5) {
                    double time = Value(tokens[4]);
                    circuit->create_new_Delta_voltage_source(element_name, n1, n2, time);
                }
                else {
                    double val = stod(tokens[3]);
                    circuit->create_new_DC_voltage_source(element_name, n1, n2, val);
                }
            }
            else if (prefix == 'E' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                groundCheck(n1); groundCheck(n2); groundCheck(ctrl1); groundCheck(ctrl2);
                circuit->create_new_VCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'F' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                groundCheck(n1); groundCheck(n2); groundCheck(ctrl1); groundCheck(ctrl2);
                circuit->create_new_CCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'G' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                groundCheck(n1); groundCheck(n2); groundCheck(ctrl1); groundCheck(ctrl2);
                circuit->create_new_VCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'H' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                groundCheck(n1); groundCheck(n2); groundCheck(ctrl1); groundCheck(ctrl2);
                circuit->create_new_CCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'D' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_real_diode(element_name, n1, n2, dummy);
            }
            else if (prefix == 'Z' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                groundCheck(n1); groundCheck(n2);
                circuit->create_new_zener_diode(element_name, n1, n2, dummy);
            }
            else if (prefix == 'G' && tokens.size() >= 3 && tokens[1] == "GND") {
                circuit->make_node_ground("GND");
                circuit->ground(true);
            }
            else if (type == ".END") {
                break;
            }
            else {
                cerr << "Unsupported or malformed element: ";
                for (const string& t : tokens) cerr << t << " ";
                cerr << endl;
            }
        }
        catch (const exception& e) {
            cerr << "Error processing element " << type << ": " << e.what() << endl;
        }
    }

    return circuit;
}


void Controller::addFileToCircuits(int fileIndex) {
    string name = file_handler.get_file_names()[fileIndex-1];
    auto text = file_handler.showText(fileIndex - 1);
    auto* c = textToCircuit(name,text);
    circuits.push_back(c);
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
        cout << i + 1 << " - " << file_names[i] << endl;
}

void Controller::showFile(int num){
    file_handler.set_file_index(num - 1);
    file_handler.show_current_file();
}

bool Controller::validSchematicChoice(string s){
    if (stoi(s) <= 0 || stoi(s) > file_handler.get_file_names().size())
      return false;
    return true;
}

void Controller::handleNewFile(string path){
    file_handler.add_file(path);
}

bool Controller::is_files_empty()
{
    return file_handler.get_file_names().empty();
}

// GRAPHICS!!!

void Controller::add_Graphical_Resistor(int screenX, int screenY) {
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_resistor_count++;
    string r_name = "R" + to_string(m_resistor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Resistor* sim_resistor = new Resistor(r_name, n1, n2, 1000.0);

    circuit->addElement(sim_resistor);

    auto gfx_resistor = make_unique<Graphical_Resistor>(sim_resistor);

    gfx_resistor->bounding_box = {screenX, screenY, 100, 40};

    m_graphical_elements.push_back(move(gfx_resistor));
}

void Controller::add_Graphical_Capacitor(int screenX, int screenY) {
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_capacitor_count++;
    string c_name = "C" + to_string(m_capacitor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Capacitor* sim_capacitor = new Capacitor(c_name, n1, n2, 1e-6);

    circuit->addElement(sim_capacitor);

    auto gfx_capacitor = make_unique<Graphical_Capacitor>(sim_capacitor);
    gfx_capacitor->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_capacitor));
}

void Controller::add_Graphical_Inductor(int screenX, int screenY) {
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_inductor_count++;
    string i_name = "I" + to_string(m_inductor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Inductor* sim_inductor = new Inductor(i_name, n1, n2, 1e-6);

    circuit->addElement(sim_inductor);

    auto gfx_inductor = make_unique<Graphical_Inductor>(sim_inductor);
    gfx_inductor->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_inductor));
}

void Controller::add_Graphical_Current_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_current_source_count++;
    string cs_name = "CS" + to_string(m_current_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Current_Source* sim_current_source = new Current_Source(cs_name, n1, n2, 1e-6);

    circuit->addElement(sim_current_source);

    auto gfx_current_source = make_unique<Graphical_Current_Source>(sim_current_source);
    gfx_current_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_current_source));
}

void Controller::add_Graphical_Real_Diode(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_real_diode_count++;
    string RD_name = "RD" + to_string(m_real_diode_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Real_Diode* sim_real_diode = new Real_Diode(RD_name, n1, n2, 1e-6);

    circuit->addElement(sim_real_diode);

    auto gfx_sim_real_diode = make_unique<Graphical_Real_Diode>(sim_real_diode);
    gfx_sim_real_diode->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_real_diode));
}

void Controller::add_Graphical_Zener_Diode(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_zener_diode_count++;
    string ZD_name = "ZD" + to_string(m_zener_diode_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Zener_Diode* sim_zener_diode = new Zener_Diode(ZD_name, n1, n2, 1e-6);

    circuit->addElement(sim_zener_diode);

    auto gfx_sim_zener_diode = make_unique<Graphical_Zener_Diode>(sim_zener_diode);
    gfx_sim_zener_diode->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_zener_diode));
}

void Controller::add_Graphical_DC_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new DC_Source(VS_name, n1, n2, 1e-6);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Sin_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new Sine_Source(VS_name, n1, n2, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Pulse_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new Pulse_Source(VS_name, n1, n2, 0, 0, 0, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Dirac_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new Delta_Dirac(VS_name, n1, n2, 1e-6);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Square_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new Square_Source(VS_name, n1, n2, 0, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Triangular_Source(int screenX, int screenY)
{
    m_node_count++;
    string n1_name = "N" + to_string(m_node_count);
    m_node_count++;
    string n2_name = "N" + to_string(m_node_count);
    m_voltage_source_count++;
    string VS_name = "VS" + to_string(m_voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    Voltage_Source* sim_voltage_source = new Triangular_Source(VS_name, n1, n2, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    m_graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Ground(SDL_Point pos, Node* node)
{
    auto ground_symbol = make_unique<Graphical_Ground>(pos, node);
    m_graphical_elements.push_back(move(ground_symbol));
}

Graphical_Net_Label* Controller::add_Graphical_Net_Label(SDL_Point pos, Node* node)
{
    auto new_label = make_unique<Graphical_Net_Label>(pos, node);
    Graphical_Net_Label* ptr = new_label.get();
    m_graphical_elements.push_back(move(new_label));
    return ptr;
}

vector<unique_ptr<Graphical_Element>>& Controller::get_graphical_elements()
{
    return m_graphical_elements;
}

void Controller::update_element_properties(int element_index, const vector<string>& new_values)
{
    if (element_index < 0 || element_index >= m_graphical_elements.size()) return;

    Element* element_model = m_graphical_elements[element_index]->get_model();
    if (!element_model) return;

    if (new_values.size() < 2) return;

    element_model->change_name(new_values[0]);

    if (auto* element = dynamic_cast<Resistor*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* element = dynamic_cast<Capacitor*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* element = dynamic_cast<Inductor*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* element = dynamic_cast<Current_Source*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* element = dynamic_cast<Real_Diode*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* element = dynamic_cast<Zener_Diode*>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
        {
            element->change_value(toValue(new_values[1]));
        }
    }
    else if (auto* v_source = dynamic_cast<Voltage_Source*>(element_model))
    {
        if (auto* dc = dynamic_cast<DC_Source*>(v_source))
        {
            if (new_values.size() > 1 && isValidSpiceNumber(new_values[1]))
                dc->set_value(toValue(new_values[1]));
        }
        else if (auto* sine = dynamic_cast<Sine_Source*>(v_source))
        {
            if (new_values.size() > 4)
            {
                if (isValidSpiceNumber(new_values[1])) sine->set_offset(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) sine->set_amplitude(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) sine->set_frequency(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) sine->set_phase_degrees(toValue(new_values[4]));
            }
        }
        else if (auto* pulse = dynamic_cast<Pulse_Source*>(v_source))
        {
            if (new_values.size() > 7) {
                if (isValidSpiceNumber(new_values[1])) pulse->set_v_initial(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) pulse->set_v_pulsed(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) pulse->set_time_delay(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) pulse->set_time_rise(toValue(new_values[4]));
                if (isValidSpiceNumber(new_values[5])) pulse->set_time_fall(toValue(new_values[5]));
                if (isValidSpiceNumber(new_values[6])) pulse->set_pulse_width(toValue(new_values[6]));
                if (isValidSpiceNumber(new_values[7])) pulse->set_period(toValue(new_values[7]));
            }
        }
        else if (auto* square = dynamic_cast<Square_Source*>(v_source))
        {
            if (new_values.size() > 5)
            {
                if (isValidSpiceNumber(new_values[1])) square->set_v_down(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) square->set_v_up(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) square->set_time_delay(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) square->set_square_width(toValue(new_values[4]));
                if (isValidSpiceNumber(new_values[5])) square->set_period(toValue(new_values[5]));
            }
        }
        else if (auto* tri = dynamic_cast<Triangular_Source*>(v_source))
        {
            if (new_values.size() > 4)
            {
                if (isValidSpiceNumber(new_values[1])) tri->set_v_initial(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) tri->set_v_peak(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) tri->set_time_delay(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) tri->set_period(toValue(new_values[4]));
            }
        }
        else if (auto* delta = dynamic_cast<Delta_Dirac*>(v_source))
        {
            if (new_values.size() > 1 && isValidSpiceNumber(new_values[1]))
                delta->set_time_of_delta(toValue(new_values[1]));
        }
    }
}

vector<unique_ptr<Graphical_Wire>> &Controller::get_graphical_wires()
{
    return m_graphical_wires;
}

void Controller::add_Graphical_Wire(const vector<Connection_Point>& points, Node* start, Node* end) {
    auto new_wire = std::make_unique<Graphical_Wire>();
    for (const auto& p : points) {
        new_wire->path.push_back(p.pos);
    }

    new_wire->start_node = start;
    new_wire->end_node = end;
    m_graphical_wires.push_back(std::move(new_wire));
}

void Controller::connect_nodes(Node* node_to_keep, Node* node_to_merge)
{
    if (!node_to_keep || !node_to_merge || node_to_keep == node_to_merge)
    {
        return;
    }

    if (node_to_merge->is_the_node_ground())
    {
        node_to_keep->make_ground();
    }

    // for label naming
    if (!node_to_merge->net_name.empty())
    {
        m_named_nets[node_to_merge->net_name] = node_to_keep;
        if (node_to_keep->net_name.empty())
        {
            node_to_keep->net_name = node_to_merge->net_name;
        }
    }

    for (const auto& element : circuit->get_Elements())
    {
        element->replace_node(node_to_merge, node_to_keep);
    }

    for (const auto& wire : m_graphical_wires)
    {
        if (wire->start_node == node_to_merge) {
            wire->start_node = node_to_keep;
        }
        if (wire->end_node == node_to_merge) {
            wire->end_node = node_to_keep;
        }
    }

    circuit->delete_node(node_to_merge);
}

void Controller::assign_net_name(Node* node_to_name, const string& new_name)
{
    // if the node already had a name
    if (!node_to_name->net_name.empty())
    {
        m_named_nets.erase(node_to_name->net_name);
    }

    node_to_name->net_name = new_name;

    // finding if another node has this name too
    auto it = m_named_nets.find(new_name);
    if (it != m_named_nets.end())
    {
        Node* existing_node = it->second;
        if (existing_node != node_to_name)
        {
            connect_nodes(existing_node, node_to_name);
        }
    }
    else
    {
        m_named_nets[new_name] = node_to_name;
    }
}

void Controller::set_transient_values(double stepTime, double stopTime, double startTime, double maxTimeStep)
{
    time_step = stepTime;
    start_time = startTime;
    end_time = stopTime;
    circuit->set_time_start(start_time);
    circuit->set_time_end(end_time);
    circuit->set_time_step(time_step);
    cout << "time step: " << time_step << endl;
    cout << "start_time: " << start_time << endl;
    cout << "end_time: " << end_time << endl;
//    circuit->analyse_data();
//    circuit->transient();
}

void Controller::get_tran_params(double &start, double &stop, double &step)
{
    start = start_time;
    stop = end_time;
    step = time_step;
}

void Controller::do_transient()
{
    cout << "time step: " << time_step << endl;
    cout << "start_time: " << start_time << endl;
    cout << "end_time: " << end_time << endl;
    circuit->analyse_data();
    circuit->transient();
}