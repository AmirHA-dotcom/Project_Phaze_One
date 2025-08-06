//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"




Controller::Controller() {
    circuit = nullptr;

    circuit = new Circuit("default_circuit");
    circuits.push_back(circuit);
}

void Controller::addCircuit(string name){
    auto* c = new Circuit(name);
    circuits.push_back(c);
}
void Controller::addSubCircuit(string name, Circuit* circuit, Node* inputNode, Node* outputNode) {
    auto* sc = new SubCircuit(name,circuit,inputNode,outputNode);
    subCircuits.push_back(sc);
}


void changeSubCircuitAllNodeNames(SubCircuit* subCircuit) {
    for (auto& node : subCircuit->get_Nodes()) {
        if (node->get_name() != subCircuit->getInput()->get_name() && node->get_name() != subCircuit->getOutput()->get_name()) {
            node->change_name(node->get_name() + "_sub");
        }
    }
}
void changeSubCircuitAllElementNames(SubCircuit* subCircuit) {
    for (auto& element : subCircuit->get_Elements()) {
        if (element->get_name() != subCircuit->getInput()->get_name() && element->get_name() != subCircuit->getOutput()->get_name()) {
            element->change_name(element->get_name() + "_sub");
        }
    }
}

void Controller::addSubCircuitToCircuit(SubCircuit* subCircuit, Circuit* circuit, const std::string& inputNodeName, const std::string& outputNodeName) {

    auto input = circuit->findNode(inputNodeName);
    if (!input) {
        std::cerr << "Input node not found in the main circuit!" << std::endl;
        return;
    }
    auto output = circuit->findNode(outputNodeName);
    if (!output) {
        std::cerr << "Output node not found in the main circuit!" << std::endl;
        return;
    }

    static int subCircuitCounter = 0;
    std::string unique_suffix = "_sub" + std::to_string(subCircuitCounter++);

    for (const auto& node : subCircuit->get_Nodes()) {
        if (node->get_name() != subCircuit->getInput()->get_name() && node->get_name() != subCircuit->getOutput()->get_name()) {
            std::string new_name = node->get_name() + unique_suffix;
            circuit->create_new_node(new_name);
        }
    }

    for (auto& element : subCircuit->get_Elements()) {
        std::string new_name = element->get_name() + unique_suffix;
        element->change_name(new_name);
        circuit->addElement(std::move(element));
    }
    circuit->checkHaveGround();
    tuple<string, int, Rotation, pair<int, int>> sub =
            make_tuple(subCircuit->get_name(), subCircuitCounter, Rotation::Right, make_pair(0, 0));
    circuit->getSubs().push_back(sub);
}
void Controller::addGraphicalSubCircuitToCircuit(SubCircuit* subCircuit, Circuit* circuit, const std::string& inputNodeName, const std::string& outputNodeName,int screenX, int screenY) {

    auto input = circuit->findNode(inputNodeName);
    if (!input) {
        std::cerr << "Input node not found in the main circuit!" << std::endl;
        return;
    }
    auto output = circuit->findNode(outputNodeName);
    if (!output) {
        std::cerr << "Output node not found in the main circuit!" << std::endl;
        return;
    }

    static int subCircuitCounter = 0;
    std::string unique_suffix = "_sub" + std::to_string(subCircuitCounter++);

    for (const auto& node : subCircuit->get_Nodes()) {
        if (node->get_name() != subCircuit->getInput()->get_name() && node->get_name() != subCircuit->getOutput()->get_name()) {
            std::string new_name = node->get_name() + unique_suffix;
            circuit->create_new_node(new_name);
        }
    }

    for (auto& element : subCircuit->get_Elements()) {
        std::string new_name = element->get_name() + unique_suffix;
        element->change_name(new_name);
        circuit->addElement(std::move(element));
    }
    circuit->checkHaveGround();
    tuple<string, int, Rotation, pair<int, int>> sub =
            make_tuple(subCircuit->get_name(), subCircuitCounter, Rotation::Right, make_pair(screenX, screenY));
    circuit->getSubs().push_back(sub);
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

int rotationToInt (Rotation rotation) {
    switch (rotation) {
        case Rotation::Right:
            return 0;
        case Rotation::Left:
            return 1;
        case Rotation::Up:
            return 2;
        case Rotation::Down:
            return 3;
    }
    return -1; // Should never reach here if rotation is set correctly
}
Rotation intToRotation (int r){
    Rotation rotation;
    switch (r) {
        case 0:
            rotation = Rotation::Right;
            break;
        case 1:
            rotation = Rotation::Left;
            break;
        case 2:
            rotation = Rotation::Up;
            break;
        case 3:
            rotation = Rotation::Down;
            break;
        default:
            throw invalid_argument("Invalid rotation value");
    }
    return rotation;
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
    file << circuit->get_name() << ": " << "\n";

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
    for ( auto node : circuit->get_Nodes()) {
        if (node->is_the_node_ground()) {
            file << ".GND " << node->get_name() << "\n";
        }
    }
    for (auto& sub : circuit->getSubs()) {
        file << ".SUB " << get<0>(sub) << " " << get<1>(sub) << "\n";
    }
    // End of circuit
    file << ".END\n";
    file.close();
    handleNewFile(path + circuit->get_name() + ".txt" );
}

void Controller::saveSubCircuit(SubCircuit* subCircuit, string path) {

    if (!subCircuit) {
        cerr << "Error: Null or invalid subcircuit pointer provided" << endl;
        return;
    }

    // Prepare path
    string fullPath = path;
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
#ifdef _WIN32
        fullPath += "\\";
#else
        fullPath += "/";
#endif
    }
    fullPath += subCircuit->get_name() + ".txt";
    filesystem::create_directories(path);

    // Write SubCircuit-specific lines first
    ofstream tempFile(fullPath);
    if (!tempFile.is_open()) {
        cerr << "Error: could not create file at " << fullPath << endl;
        return;
    }
    tempFile << subCircuit->get_name() << ":\n";
    tempFile << "Input: " << (subCircuit->getInput() ? subCircuit->getInput()->get_name() : "") << "\n";
    tempFile << "Output: " << (subCircuit->getOutput() ? subCircuit->getOutput()->get_name() : "") << "\n";
    tempFile.close();

    // Call saveCircuit to append circuit components and ground
    saveCircuit(subCircuit, path);

    // Reorder file to match desired format
    string tempPath = path + subCircuit->get_name() + "_temp.txt";
    ofstream outFile(tempPath);
    ifstream inFile(fullPath);
    if (!outFile.is_open() || !inFile.is_open()) {
        cerr << "Error: could not process files for " << fullPath << endl;
        inFile.close();
        outFile.close();
        return;
    }

    // Copy SubCircuit-specific lines
    outFile << subCircuit->get_name() << ":\n";
    outFile << "Input: " << (subCircuit->getInput() ? subCircuit->getInput()->get_name() : "") << "\n";
    outFile << "Output: " << (subCircuit->getOutput() ? subCircuit->getOutput()->get_name() : "") << "\n";

    // Skip the circuit name line from saveCircuit output
    string line;
    bool firstLineSkipped = false;
    while (getline(inFile, line)) {
        if (!firstLineSkipped && line.find(subCircuit->get_name() + ":") == 0) {
            firstLineSkipped = true;
            continue;
        }
        outFile << line << "\n";
    }

    inFile.close();
    outFile.close();

    // Replace original file
    filesystem::remove(fullPath);
    filesystem::rename(tempPath, fullPath);

    handleNewFile(fullPath);
}
void Controller::saveGraphicalCircuit(Circuit* circuit, string path) {
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
    file << circuit->get_name() << ": " << "\n";

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
                line = "R" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Capacitor:
                line = "C" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Inductor:
                line = "L" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Current_Source:
                line = "I" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Voltage_Source:
                // Assume DC voltage source, as Element doesn't store sourceType
                line = "V" + element_name + " " + node1 + " " + node2 + " DC " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::VC_Voltage_Source:
                // Control nodes not in Element; assume "0 0" or handle via derived class if available
                line = "E" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::CC_Current_source:
                line = "F" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::VC_Current_Source:
                line = "G" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::CC_Voltage_Source:
                line = "H" + element_name + " " + node1 + " " + node2 + " 0 0 " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Real_Diode:
                line = "D" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
                break;
            case Element_Type::Zener_Diode:
                line = "Z" + element_name + " " + node1 + " " + node2 + " " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
            default:
                cerr << "Unsupported component type: " << static_cast<int>(type) << endl;
                continue;
        }
        file << line << "\n";
    }
    // Write ground directive if present
    for ( auto node : circuit->get_Nodes()) {
        if (node->is_the_node_ground()) {
            file << ".GND " << node->get_name() << " " << node->get_ground_coordinates().first
                 << " " << node->get_ground_coordinates().second << "\n";            }
    }
    for ( auto node : circuit->get_Nodes()) {
        if (node->haveNetLabel()) {
            file << ".NL " << node->get_name() << " " << node->net_name << " " <<
            node->get_net_label_coordinates().first << " " << node->get_net_label_coordinates().second << "\n";
        }
    }
    for (auto & sub : circuit->getSubs()) {
        file << ".SUB " << get<0>(sub) << " " << get<1>(sub) << " " << rotationToInt(get<2>(sub)) << " "
             << get<3>(sub).first << " " << get<3>(sub).second << "\n";

    }
    // End of circuit
    file << ".END\n";
    file.close();
    handleNewFile(path + circuit->get_name() + ".txt" );
}

void Controller::saveGraphicalSubCircuit(SubCircuit* subCircuit, string path) {
    if (!subCircuit) {
        cerr << "Error: Null or invalid subcircuit pointer provided" << endl;
        return;
    }

    // Prepare path
    string fullPath = path;
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
#ifdef _WIN32
        fullPath += "\\";
#else
        fullPath += "/";
#endif
    }
    fullPath += subCircuit->get_name() + ".txt";
    filesystem::create_directories(path);

    // Write SubCircuit-specific lines first
    ofstream tempFile(fullPath);
    if (!tempFile.is_open()) {
        cerr << "Error: could not create file at " << fullPath << endl;
        return;
    }
    tempFile << subCircuit->get_name() << ":\n";
    tempFile << "Input: " << (subCircuit->getInput() ? subCircuit->getInput()->get_name() : "") << "\n";
    tempFile << "Output: " << (subCircuit->getOutput() ? subCircuit->getOutput()->get_name() : "") << "\n";
    tempFile << "Rotation: " << subCircuit->get_rotation_as_int() << "\n";
    tempFile.close();

    // Call saveCircuit to append circuit components and ground
    saveGraphicalCircuit(subCircuit, path);

    // Reorder file to match desired format
    string tempPath = path + subCircuit->get_name() + "_temp.txt";
    ofstream outFile(tempPath);
    ifstream inFile(fullPath);
    if (!outFile.is_open() || !inFile.is_open()) {
        cerr << "Error: could not process files for " << fullPath << endl;
        inFile.close();
        outFile.close();
        return;
    }

    // Copy SubCircuit-specific lines
    outFile << subCircuit->get_name() << ":\n";
    tempFile << "Input: " << (subCircuit->getInput() ? subCircuit->getInput()->get_name() : "") << "\n";
    tempFile << "Output: " << (subCircuit->getOutput() ? subCircuit->getOutput()->get_name() : "") << "\n";
    outFile << "Rotation: " << subCircuit->get_rotation_as_int() << "\n";

    // Skip the circuit name line from saveCircuit output
    string line;
    bool firstLineSkipped = false;
    while (getline(inFile, line)) {
        if (!firstLineSkipped && line.find(subCircuit->get_name() + ":") == 0) {
            firstLineSkipped = true;
            continue;
        }
        outFile << line << "\n";
    }

    inFile.close();
    outFile.close();

    // Replace original file
    filesystem::remove(fullPath);
    filesystem::rename(tempPath, fullPath);

    handleNewFile(fullPath);
}

void Controller::saveSubCircuits () {
    string path = file_handler.getMainFolderPath() + "/subcircuits";
    for (const auto& subCircuit : subCircuits) {
        saveSubCircuit(subCircuit,path);
    }
}
void Controller::saveGraphicalSubCircuits () {
    string path = file_handler.getMainFolderPath() + "/subcircuits";
    for (const auto& subCircuit : subCircuits) {
        saveGraphicalSubCircuit(subCircuit,path);
    }
}




Circuit* textToCircuit(string Name, const vector<vector<string>>& lines) {
    Circuit* circuit = new Circuit(Name);

    for (const auto& tokens : lines) {
        if (tokens.empty()) continue;

        string type = tokens[0];
        char prefix = toupper(type[0]);
        string element_name = type.substr(1); // Remove type prefix (e.g., R1 → 1)

        try {
            if (tokens == lines[0] && tokens[0][tokens[0].size()-1] == ':' && tokens.size() == 1) {
                circuit->change_name(tokens[0].substr(0,tokens[0].size()-1));
            }
            else if (prefix == 'R' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_resistor(element_name, n1, n2, val);
            }
            else if (prefix == 'C' && tokens.size() >= 4 && type != "CCVS" && type != "CCCS") {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_capacitor(element_name, n1, n2, val);
            }
            else if (prefix == 'L' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_inductor(element_name, n1, n2, val);
            }
            else if (prefix == 'I' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_current_source(element_name, n1, n2, val);
            }
            else if (prefix == 'V' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
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
                circuit->create_new_VCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'F' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_CCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'G' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_VCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'H' && tokens.size() >= 6) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_CCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
            }
            else if (prefix == 'D' && tokens.size() >= 4) {
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                circuit->create_new_real_diode(element_name, n1, n2, dummy);
            }
            else if (prefix == 'Z' && tokens.size() >= 4) {
                // Example .Z1 N1 N2 5.0
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                circuit->create_new_zener_diode(element_name, n1, n2, dummy);
            }
            else if (prefix == 'G' && tokens.size() == 2 && tokens[0] == "GND") {
                // Example .GND a
                circuit->make_node_ground(tokens[1]);
                circuit->ground(true);
            }
            else if (prefix == 'S' && tokens.size() == 3 && tokens[0] == "SUB") {
                // Example .SUB subName index
                tuple<string, int, Rotation, pair<int, int>> sub =
                        make_tuple(tokens[1], stoi(tokens[2]), Rotation::Right, make_pair(0, 0));
                circuit->getSubs().push_back(sub);
            }
            else if (type == ".END") {
                break;
            }
            else {
                cerr << "Unsupported or malformed element: ";
                for (const string& t : tokens) cout << t << " ";
                cerr << endl;
            }
        }
        catch (const exception& e) {
            cerr << "Error processing element " << type << ": " << e.what() << endl;
        }
    }

    return circuit;
}
SubCircuit* textToSubCircuit(string Name, const vector<vector<string>>& lines) {
    // Use textToCircuit to create the base Circuit
    Circuit* baseCircuit = textToCircuit(Name, lines);

    // If textToCircuit didn't return a SubCircuit, create one
    SubCircuit* subCircuit = new SubCircuit();
    subCircuit->change_name(Name);
    // Copy elements and nodes from baseCircuit if necessary
    for (auto* element : baseCircuit->get_Elements()) {
        subCircuit->addElement(element);
    }
    for (auto* node : baseCircuit->get_Nodes()) {
        subCircuit->addNode(node);
    }
    delete baseCircuit; // Clean up if baseCircuit is not a SubCircuit

    // Map to store nodes by name for reuse
    unordered_map<string, Node*> nodeMap;
    for (auto* node : subCircuit->get_Nodes()) {
        nodeMap[node->get_name()] = node;
    }

    // Process SubCircuit-specific lines (Input, Output)
    for (const auto& line : lines) {
        if (line.empty()) continue;

        // Handle Input line
        if (line[0] == "Input:") {
            if (line.size() < 2) {
                cerr << "Error: Invalid Input line format" << endl;
                continue;
            }
            string nodeName = line[1];
            if (nodeMap.find(nodeName) == nodeMap.end()) {
                nodeMap[nodeName] = new Node(nodeName);
                subCircuit->addNode(nodeMap[nodeName]);
            }
            subCircuit->setInput(nodeMap[nodeName]); // Set input node
        }
            // Handle Output line
        else if (line[0] == "Output:") {
            if (line.size() < 2) {
                cerr << "Error: Invalid Output line format" << endl;
                continue;
            }
            string nodeName = line[1];
            if (nodeMap.find(nodeName) == nodeMap.end()) {
                nodeMap[nodeName] = new Node(nodeName);
                subCircuit->addNode(nodeMap[nodeName]);
            }
            subCircuit->setOutput(nodeMap[nodeName]); // Set output node
        }
    }

    return subCircuit;
}

Circuit* textToGraphicalCircuit(string Name, const vector<vector<string>>& lines) {
    Circuit* circuit = new Circuit(Name);

    for (const auto& tokens : lines) {
        if (tokens.empty()) continue;

        string type = tokens[0];
        char prefix = toupper(type[0]);
        string element_name = type.substr(1); // Remove type prefix (e.g., R1 → 1)

        try {
            if (tokens == lines[0] && tokens[0][tokens[0].size()-1] == ':' && tokens.size() == 1) {
                circuit->change_name(tokens[0].substr(0,tokens[0].size()-1));
            }
            else if (prefix == 'R' && tokens.size() >= 7) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_resistor(element_name, n1, n2, val);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'C' && tokens.size() >= 7 && type != "CCVS" && type != "CCCS") {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_capacitor(element_name, n1, n2, val);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'L' && tokens.size() >= 7) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_inductor(element_name, n1, n2, val);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'I' && tokens.size() >= 7) {
                string n1 = tokens[1], n2 = tokens[2];
                double val = Value(tokens[3]);
                circuit->create_new_current_source(element_name, n1, n2, val);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'V' && tokens.size() >= 7) {
                string n1 = tokens[1], n2 = tokens[2];
                if (tokens[3] == "DC" && tokens.size() >= 8) {
                    double val = Value(tokens[4]);
                    circuit->create_new_DC_voltage_source(element_name, n1, n2, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[5]),stoi(tokens[6]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[7]));
                }
                else if (tokens[3] == "SIN" && tokens.size() >= 10) {
                    double offset = Value(tokens[4]);
                    double amp = Value(tokens[5]);
                    double freq = Value(tokens[6]);
                    circuit->create_new_Sin_voltage_source(element_name, n1, n2, offset, amp, freq);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[7]),stoi(tokens[8]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[9]));
                }
                else if (tokens[3] == "PULSE" && tokens.size() >= 9) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Pulse_voltage_source(element_name, n1, n2, period, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
                }
                else if (tokens[3] == "SQUARE" && tokens.size() >= 9) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Square_voltage_source(element_name, n1, n2, period, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
                }
                else if (tokens[3] == "TRIANGLE" && tokens.size() >= 9) {
                    double period = Value(tokens[4]);
                    double val = Value(tokens[5]);
                    circuit->create_new_Triangle_voltage_source(element_name, n1, n2, period, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
                }
                else if (tokens[3] == "DELTA" && tokens.size() >= 8) {
                    double time = Value(tokens[4]);
                    circuit->create_new_Delta_voltage_source(element_name, n1, n2, time);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
                }
                else {
                    double val = stod(tokens[3]);
                    circuit->create_new_DC_voltage_source(element_name, n1, n2, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
                }
            }
            else if (prefix == 'E' && tokens.size() >= 9) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_VCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
            }
            else if (prefix == 'F' && tokens.size() >= 9) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_CCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
            }
            else if (prefix == 'G' && tokens.size() >= 9) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_VCCS(element_name, n1, n2, ctrl1, ctrl2, gain);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
            }
            else if (prefix == 'H' && tokens.size() >= 9) {
                string n1 = tokens[1], n2 = tokens[2], ctrl1 = tokens[3], ctrl2 = tokens[4];
                double gain = Value(tokens[5]);
                circuit->create_new_CCVS(element_name, n1, n2, ctrl1, ctrl2, gain);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[6]),stoi(tokens[7]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[8]));
            }
            else if (prefix == 'D' && tokens.size() >= 7) {
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                circuit->create_new_real_diode(element_name, n1, n2, dummy);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'Z' && tokens.size() >= 7) {
                // Example .Z1 N1 N2 5.0
                string n1 = tokens[1], n2 = tokens[2];
                double dummy = Value(tokens[3]);
                circuit->create_new_zener_diode(element_name, n1, n2, dummy);
                circuit->findElement(element_name)->set_coordinates(stoi(tokens[4]),stoi(tokens[5]));
                circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[6]));
            }
            else if (prefix == 'G' && tokens.size() == 4 && tokens[0] == "GND") {
                // Example .GND a x y
                Node* node = circuit->findNode(tokens[1]);
                node->make_ground();    node->set_ground_coordinates(stoi(tokens[2]), stoi(tokens[3]));
                circuit->ground(true);
            }
            else if (prefix == 'N' && tokens.size() == 5 && tokens[0] == "NL") {
                // Example .NL a Netname x y
                Node* node = circuit->findNode(tokens[1]);
                node->net_name = tokens[2];    node->set_net_label_coordinates(stoi(tokens[3]), stoi(tokens[4]));
            }
            else if (prefix == 'S' && tokens.size() == 6 && tokens[0] == "SUB") {
                // Example .SUB subName index Rotation(int) x y
                tuple<string, int, Rotation, pair<int, int>> sub =
                        make_tuple(tokens[1], stoi(tokens[2]), intToRotation(stoi(tokens[3])), make_pair(stoi(tokens[4]), stoi(tokens[5])));

                circuit->getSubs().push_back(sub);
            }
            else if (type == ".END") {
                break;
            }
            else {
                cerr << "Unsupported or malformed element: ";
                for (const string& t : tokens) cout << t << " ";
                cerr << endl;
            }
        }
        catch (const exception& e) {
            cerr << "Error processing element " << type << ": " << e.what() << endl;
        }
    }

    return circuit;
}

SubCircuit* textToGraphicalSubCircuit(string Name, const vector<vector<string>>& lines) {
    // Use textToCircuit to create the base Circuit
    Circuit* baseCircuit = textToCircuit(Name, lines);

        SubCircuit* subCircuit = new SubCircuit();
        subCircuit->change_name(Name);
        // Copy elements and nodes from baseCircuit
        for (auto* element : baseCircuit->get_Elements()) {
            subCircuit->addElement(element);
        }
        for (auto* node : baseCircuit->get_Nodes()) {
            subCircuit->addNode(node);
        }
        delete baseCircuit; // Clean up if baseCircuit is not a SubCircuit

    // Map to store nodes by name for reuse
    unordered_map<string, Node*> nodeMap;
    for (auto* node : subCircuit->get_Nodes()) {
        nodeMap[node->get_name()] = node;
    }

    // Process SubCircuit-specific lines (Input, Output, Rotation)
    for (const auto& line : lines) {
        if (line.empty()) continue;

        // Handle Input line
        if (line[0] == "Input:") {
            if (line.size() < 2) {
                cerr << "Error: Invalid Input line format" << endl;
                continue;
            }
            string nodeName = line[1];
            if (nodeMap.find(nodeName) == nodeMap.end()) {
                nodeMap[nodeName] = new Node(nodeName);
                subCircuit->addNode(nodeMap[nodeName]);
            }
            subCircuit->setInput(nodeMap[nodeName]); // Set input node
        }
        // Handle Output line
        else if (line[0] == "Output:") {
            if (line.size() < 2) {
                cerr << "Error: Invalid Output line format" << endl;
                continue;
            }
            string nodeName = line[1];
            if (nodeMap.find(nodeName) == nodeMap.end()) {
                nodeMap[nodeName] = new Node(nodeName);
                subCircuit->addNode(nodeMap[nodeName]);
            }
            subCircuit->setOutput(nodeMap[nodeName]); // Set output node
        }
            // Handle Rotation line
        else if (line[0] == "Rotation:") {
            if (line.size() < 2) {
                cerr << "Error: Invalid Rotation line format" << endl;
                continue;
            }
            try {
                int rotationValue = stoi(line[1]);
                switch (rotationValue) {
                    case 0: subCircuit->set_rotation(Rotation::Right); break;
                    case 1: subCircuit->set_rotation(Rotation::Down); break;
                    case 2: subCircuit->set_rotation(Rotation::Left); break;
                    case 3: subCircuit->set_rotation(Rotation::Up); break;
                    default:
                        cerr << "Error: Invalid rotation value: " << line[1] << endl;
                        subCircuit->set_rotation(Rotation::Up); // Default
                        break;
                }
            } catch (...) {
                cerr << "Error: Invalid rotation value format: " << line[1] << endl;
                subCircuit->set_rotation(Rotation::Up); // Default
            }
        }
    }

    return subCircuit;
}

void Controller::loadSubCircuits () {
    string path = file_handler.getMainFolderPath() + "/subcircuits";
    vector<string> files = file_handler.getFilesInDirectory(path);
    for (const auto& file : files) {
        if (file.substr(file.find_last_of(".") + 1) == "txt") {
            ifstream inFile(path + file);
            if (!inFile.is_open()) {
                cerr << "Error opening file: " << path + file << endl;
                continue;
            }
            string line;
            vector<vector<string>> lines;
            while (getline(inFile, line)) {
                istringstream iss(line);
                vector<string> tokens((istream_iterator<string>(iss)), istream_iterator<string>());
                lines.push_back(tokens);
            }
            inFile.close();
            if (!lines.empty()) {
                string name = lines[0][0].substr(0, lines[0][0].size() - 1);
                SubCircuit *subCircuit = textToSubCircuit(name, lines);
                addSubCircuit(name, subCircuit, subCircuit->getInput(), subCircuit->getOutput());
            }
        }
    }
}

void Controller::loadGraphicalSubCircuits () {
    string path = file_handler.getMainFolderPath() + "/subcircuits";
    vector<string> files = file_handler.getFilesInDirectory(path);
    for (const auto& file : files) {
        if (file.substr(file.find_last_of(".") + 1) == "txt") {
            ifstream inFile(path + file);
            if (!inFile.is_open()) {
                cerr << "Error opening file: " << path + file << endl;
                continue;
            }
            string line;
            vector<vector<string>> lines;
            while (getline(inFile, line)) {
                istringstream iss(line);
                vector<string> tokens((istream_iterator<string>(iss)), istream_iterator<string>());
                lines.push_back(tokens);
            }
            inFile.close();
            if (!lines.empty()) {
                string name = lines[0][0].substr(0, lines[0][0].size() - 1);
                SubCircuit* subCircuit = textToGraphicalSubCircuit(name, lines);
                addSubCircuit(name, subCircuit, subCircuit->getInput(), subCircuit->getOutput());
            }
        }
    }
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
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    resistor_count++;
    string r_name = "R" + to_string(resistor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Resistor* sim_resistor = new Resistor(r_name, n1, n2, 1000.0);

    circuit->addElement(sim_resistor);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_resistor = make_unique<Graphical_Resistor>(sim_resistor);

    gfx_resistor->bounding_box = {screenX, screenY, 100, 40};

    graphical_elements.push_back(move(gfx_resistor));
}

void Controller::add_Graphical_Capacitor(int screenX, int screenY) {
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    capacitor_count++;
    string c_name = "C" + to_string(capacitor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Capacitor* sim_capacitor = new Capacitor(c_name, n1, n2, 1e-6);

    circuit->addElement(sim_capacitor);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_capacitor = make_unique<Graphical_Capacitor>(sim_capacitor);
    gfx_capacitor->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_capacitor));
}

void Controller::add_Graphical_Inductor(int screenX, int screenY) {
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    inductor_count++;
    string i_name = "I" + to_string(inductor_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Inductor* sim_inductor = new Inductor(i_name, n1, n2, 1e-6);

    circuit->addElement(sim_inductor);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_inductor = make_unique<Graphical_Inductor>(sim_inductor);
    gfx_inductor->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_inductor));
}

void Controller::add_Graphical_Current_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    current_source_count++;
    string cs_name = "CS" + to_string(current_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Current_Source* sim_current_source = new Current_Source(cs_name, n1, n2, 1e-6);

    circuit->addElement(sim_current_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_current_source = make_unique<Graphical_Current_Source>(sim_current_source);
    gfx_current_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_current_source));
}

void Controller::add_Graphical_Real_Diode(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    real_diode_count++;
    string RD_name = "RD" + to_string(real_diode_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Real_Diode* sim_real_diode = new Real_Diode(RD_name, n1, n2, 1e-6);

    circuit->addElement(sim_real_diode);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_real_diode = make_unique<Graphical_Real_Diode>(sim_real_diode);
    gfx_sim_real_diode->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_real_diode));
}

void Controller::add_Graphical_Zener_Diode(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    zener_diode_count++;
    string ZD_name = "ZD" + to_string(zener_diode_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Zener_Diode* sim_zener_diode = new Zener_Diode(ZD_name, n1, n2, 1e-6);

    circuit->addElement(sim_zener_diode);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_zener_diode = make_unique<Graphical_Zener_Diode>(sim_zener_diode);
    gfx_sim_zener_diode->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_zener_diode));
}

void Controller::add_Graphical_DC_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new DC_Source(VS_name, n1, n2, 1e-6);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Sin_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new Sine_Source(VS_name, n1, n2, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Pulse_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new Pulse_Source(VS_name, n1, n2, 0, 0, 0, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Dirac_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new Delta_Dirac(VS_name, n1, n2, 1e-6);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Square_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new Square_Source(VS_name, n1, n2, 0, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_Triangular_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    Voltage_Source* sim_voltage_source = new Triangular_Source(VS_name, n1, n2, 0, 0, 0, 0);

    circuit->addElement(sim_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_AC_Phase_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);

    string VS_name = "Source";

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    AC_Voltage_Source* sim_source = new AC_Voltage_Source(VS_name, n1, n2, 1, 1, 0.0);

    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_Voltage_Source>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_VCVS(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    VCVS_count++;
    string name = "VCVS" + to_string(VCVS_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    VCVS* sim_source = new VCVS(name, n1, n2, 1, nullptr, nullptr);
    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_VCVS>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_VCCS(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    VCCS_count++;
    string name = "VCCS" + to_string(VCVS_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    VCCS* sim_source = new VCCS(name, n1, n2, 1, nullptr, nullptr);
    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_VCCS>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_CCVS(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    CCVS_count++;
    string name = "CCVS" + to_string(VCVS_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    CCVS* sim_source = new CCVS(name, n1, n2, 1, nullptr, nullptr);
    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_CCVS>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_CCCS(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    CCCS_count++;
    string name = "CCCS" + to_string(VCVS_count);

    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    CCCS* sim_source = new CCCS(name, n1, n2, 1, nullptr, nullptr);
    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_CCCS>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_Ground(SDL_Point pos, Node* node)
{
    auto ground_symbol = make_unique<Graphical_Ground>(pos, node);
    ground_symbol->bounding_box = {pos.x - 12, pos.y, 24, 18};
    graphical_elements.push_back(move(ground_symbol));

    node->set_ground_coordinates(pos.x, pos.y);
}

void Controller::add_Graphical_Sub_Circuit(int screenX, int screenY, string name)
{
    cout << "called add graphical sub circuit" << endl;
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    Node* n1 = circuit->create_new_node(n1_name);
    Node* n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    SubCircuit* sim_Sub_C = nullptr;

    for (const auto& SubC : subCircuits)
    {
        if (SubC->get_name() == name)
        {
            sim_Sub_C = SubC;
            break;
        }
    }

    if (sim_Sub_C != nullptr)
    {
        addGraphicalSubCircuitToCircuit(sim_Sub_C, circuit, n1->get_name(), n2->get_name(), screenX, screenY);
        auto gfx_Sub_C = make_unique<Graphical_SubCircuit>(sim_Sub_C, name);

        gfx_Sub_C->bounding_box = {screenX, screenY, 150, 60};

        graphical_elements.push_back(move(gfx_Sub_C));
        //cout << "added subcircuit, so the bug is not from controller not being called" << endl;
    }
}

Graphical_Net_Label* Controller::add_Graphical_Net_Label(SDL_Point pos, Node* node)
{
    auto new_label = make_unique<Graphical_Net_Label>(pos, node);

    node->set_net_label_coordinates(pos.x, pos.y);

    Graphical_Net_Label* ptr = new_label.get();
    graphical_elements.push_back(move(new_label));
    return ptr;
}

vector<unique_ptr<Graphical_Element>>& Controller::get_graphical_elements()
{
    return graphical_elements;
}

void Controller::update_element_properties(int element_index, const vector<string>& new_values)
{
    if (element_index < 0 || element_index >= graphical_elements.size()) return;

    Element* element_model = graphical_elements[element_index]->get_model();
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
        else if (auto* AC = dynamic_cast<AC_Voltage_Source*>(v_source))
        {
            if (new_values.size() > 3)
            {
                if (isValidSpiceNumber(new_values[1])) AC->set_amplitude(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) AC->set_frequency(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) AC->set_phase_degrees(toValue(new_values[3]));
            }
        }
    }
}

vector<unique_ptr<Graphical_Wire>> &Controller::get_graphical_wires()
{
    return graphical_wires;
}

void Controller::add_Graphical_Wire(const vector<Connection_Point>& points, Node* start, Node* end) {
    auto new_wire = make_unique<Graphical_Wire>();
    for (const auto& p : points) {
        new_wire->path.push_back(p.pos);
    }

    new_wire->start_node = start;
    new_wire->end_node = end;
    graphical_wires.push_back(move(new_wire));
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
        pair<int,int> g_c = node_to_merge->get_ground_coordinates();
        node_to_keep->set_ground_coordinates(g_c.first, g_c.second);
    }

    for (int i = 0; i < node_to_merge->connected_elements_count(); i++)
    {
        node_to_keep->connect_element();
    }

    // for label naming
    if (!node_to_merge->net_name.empty())
    {
        named_nets[node_to_merge->net_name] = node_to_keep;
        if (node_to_keep->net_name.empty())
        {
            node_to_keep->net_name = node_to_merge->net_name;
        }
    }

    for (const auto& element : circuit->get_Elements())
    {
        element->replace_node(node_to_merge, node_to_keep);
    }

    for (const auto& wire : graphical_wires)
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
        named_nets.erase(node_to_name->net_name);
    }

    node_to_name->net_name = new_name;

    // finding if another node has this name too
    auto it = named_nets.find(new_name);
    if (it != named_nets.end())
    {
        Node* existing_node = it->second;
        if (existing_node != node_to_name)
        {
            connect_nodes(existing_node, node_to_name);
        }
    }
    else
    {
        named_nets[new_name] = node_to_name;
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
}

void Controller::get_tran_params(double &start, double &stop, double &step)
{
    start = start_time;
    stop = end_time;
    step = time_step;
}

void Controller::do_transient()
{
    int i = 0;
    for (auto& node : circuit->get_Nodes())
    {
        if (!node->net_name.empty())
        {
            node->change_name(node->net_name);
            continue;
        }
        node->change_name("N" + to_string(++i));
    }


    circuit->analyse_data();
    circuit->transient();

    for (auto& element : circuit->get_Elements())
    {
        cout << "name: " << element->get_name() << endl;
    }

    for (auto& node : circuit->get_Nodes())
    {
        for (auto& data : node->get_all_voltages())
        {
            cout << "time: " << data.second << " V: " << data.first << endl;
        }
    }
}

void Controller::delete_element(Graphical_Element* g_element_to_delete)
{
    if (!g_element_to_delete) return;

    vector<Connection_Point> terminals = g_element_to_delete->get_connection_points();

    // removing any wire connected to the connection points
    auto& wires = get_graphical_wires();
    wires.erase(
            remove_if(wires.begin(), wires.end(),[&](const unique_ptr<Graphical_Wire>& wire) {
                               if (wire->path.empty()) return false;

                               const SDL_Point& start_point = wire->path.front();
                               const SDL_Point& end_point = wire->path.back();

                               for (const auto& terminal : terminals) {
                                   if ((start_point.x == terminal.pos.x && start_point.y == terminal.pos.y) || (end_point.x == terminal.pos.x && end_point.y == terminal.pos.y))
                                   {
                                       return true;
                                   }
                               }
                               return false;
                           }),
            wires.end());


    if (g_element_to_delete->get_model() != nullptr)
    {
        string name_to_delete = g_element_to_delete->get_model()->get_name();
        circuit->delete_element(name_to_delete);
//        switch (g_element_to_delete->get_model()->get_element_type())
//        {
//            case Element_Type::Resistor: resistor_count--;
//            case Element_Type::Capacitor: capacitor_count--;
//            case Element_Type::Inductor: inductor_count--;
//            case Element_Type::Current_Source: current_source_count--;
//            case Element_Type::Voltage_Source: voltage_source_count--;
//            case Element_Type::Real_Diode: real_diode_count--;
//            case Element_Type::Zener_Diode: zener_diode_count--;
//        }
    }

    auto& g_elements = get_graphical_elements();

    g_elements.erase(remove_if(g_elements.begin(), g_elements.end(),[&](const unique_ptr<Graphical_Element>& element) {
                               return element.get() == g_element_to_delete;
                           }),
            g_elements.end());
}

void Controller::delete_wire(Graphical_Wire *wire_to_delete)
{
    if (wire_to_delete->start_node->connected_elements_count() == 0)
    {
        circuit->delete_node(wire_to_delete->start_node);
        node_count--;
    }
    graphical_wires.erase(remove_if(graphical_wires.begin(), graphical_wires.end(), [&](const unique_ptr<Graphical_Wire>& wire) {
                        return wire.get() == wire_to_delete;
                    }
            ),
                          graphical_wires.end()
    );
}

vector<string> Controller::get_file_names()
{
    return file_handler.get_file_names();
}

void Controller::New_File()
{
    graphical_elements.clear();
    graphical_wires.clear();
    named_nets.clear();

    circuit = nullptr;

    circuit = new Circuit("default_circuit");
    circuits.push_back(circuit);
}

inline SDL_Point snap_to_grid(int x, int y, int grid_size)
{
    int snapped_x = round((float)x / grid_size) * grid_size;
    int snapped_y = round((float)y / grid_size) * grid_size;
    return {snapped_x, snapped_y};
}

void Controller::build_graphical_elements_from_circuit()
{
    if (!circuit) return;

    graphical_elements.clear();
    graphical_wires.clear();
    named_nets.clear();

    // elements
    const auto& logical_elements = circuit->get_Elements();

    for (Element* model : logical_elements)
    {
        if (auto* resistor = dynamic_cast<Resistor*>(model))
        {
            auto gfx = make_unique<Graphical_Resistor>(resistor);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* capacitor = dynamic_cast<Capacitor*>(model))
        {
            auto gfx = make_unique<Graphical_Capacitor>(capacitor);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* inductor = dynamic_cast<Inductor*>(model))
        {
            auto gfx = make_unique<Graphical_Inductor>(inductor);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* current_source = dynamic_cast<Current_Source*>(model))
        {
            auto gfx = make_unique<Graphical_Current_Source>(current_source);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* real_diode = dynamic_cast<Real_Diode*>(model))
        {
            auto gfx = make_unique<Graphical_Real_Diode>(real_diode);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* zener_diode = dynamic_cast<Zener_Diode*>(model))
        {
            auto gfx = make_unique<Graphical_Zener_Diode>(zener_diode);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
        else if (auto* voltage_source = dynamic_cast<Voltage_Source*>(model))
        {
            auto gfx = make_unique<Graphical_Voltage_Source>(voltage_source);
            gfx->bounding_box = {model->get_x(), model->get_y(), 100, 40};
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(move(gfx));
        }
    }

    // net labels
    const auto& logical_nodes = circuit->get_Nodes();
    for (Node* node : logical_nodes)
    {
        if (!node->net_name.empty())
        {
            SDL_Point label_pos = {node->get_net_label_coordinates().first, node->get_net_label_coordinates().second };

            add_Graphical_Net_Label(label_pos, node);
        }
        if (node->is_the_node_ground())
        {
            SDL_Point ground_pos = { node->get_ground_coordinates().first, node->get_ground_coordinates().second };

            add_Graphical_Ground(ground_pos, node);
        }
    }
    
    // grouping all connection points by their shared node
    map<Node*, vector<Connection_Point>> node_to_points_map;
    for (const auto& element : graphical_elements)
    {
        for (const auto& point : element->get_connection_points())
        {
            if (point.node != nullptr) 
            {
                node_to_points_map[point.node].push_back(point);
            }
        }
    }

    // create wire for each group
    for (const auto& pair : node_to_points_map)
    {
        Node* common_node = pair.first;
        const vector<Connection_Point>& points = pair.second;

        if (!common_node->net_name.empty())
        {
            continue;
        }

        if (points.size() < 2) 
        {
            continue;
        }

        if (points.size() == 2)
        {
            const Connection_Point& start_cp = points[0];
            const Connection_Point& end_cp = points[1];

            SDL_Point start_pos = start_cp.pos;
            SDL_Point end_pos = end_cp.pos;
            Rotation start_rot = start_cp.rotation;
            Rotation end_rot = end_cp.rotation;

            vector<SDL_Point> path_points;
            path_points.push_back(start_pos);
            SDL_Point corner;

            if (start_rot == Rotation::Right)
            {
                if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                {
                    corner = { start_pos.x, end_pos.y };
                }
                else if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                {
                    if (start_pos.x < end_pos.x)
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                    else
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                }
            }
            else if (start_rot == Rotation::Left)
            {
                if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                {
                    corner = { start_pos.x, end_pos.y };
                }
                else if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                {
                    if (start_pos.x > end_pos.x)
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                    else
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                }
            }
            else if (start_rot == Rotation::Up)
            {
                if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                {
                    corner = { end_pos.x, start_pos.y };
                }
                else if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                {
                    if (start_pos.y < end_pos.y)
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                    else
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                }
            }
            else if (start_rot == Rotation::Down)
            {
                if (end_rot == Rotation::Right || end_rot == Rotation::Left)
                {
                    corner = { end_pos.x, start_pos.y };
                }
                else if (end_rot == Rotation::Up || end_rot == Rotation::Down)
                {
                    if (start_pos.y > end_pos.y)
                    {
                        corner = { start_pos.x, end_pos.y };
                    }
                    else
                    {
                        corner = { end_pos.x, start_pos.y };
                    }
                }
            }
            path_points.push_back(corner);
            path_points.push_back(end_pos);

            // Create and add the wire
            vector<Connection_Point> final_wire_points;
            for(const auto& p : path_points) {
                final_wire_points.push_back({ p, nullptr, {} });
            }
            add_Graphical_Wire(final_wire_points, common_node, common_node);
        }

        else if (points.size() > 2)
        {
            SDL_Point junction_pos = {0, 0};
            for (const auto& p : points)
            {
                junction_pos.x += p.pos.x;
                junction_pos.y += p.pos.y;
            }
            junction_pos.x /= points.size();
            junction_pos.y /= points.size();
            junction_pos = snap_to_grid(junction_pos.x, junction_pos.y, 10);

            // connect each terminal to the central junction point
            for (const auto& start_cp : points)
            {
                vector<SDL_Point> path_points;
                path_points.push_back(start_cp.pos);

                const int STUB_LENGTH = 20;
                SDL_Point stub_pos;
                SDL_Point corner_pos;
                bool is_start_horizontal = (start_cp.rotation == Rotation::Left || start_cp.rotation == Rotation::Right);

                // stubbing
                if (is_start_horizontal)
                {
                    stub_pos = { start_cp.pos.x + STUB_LENGTH * (start_cp.rotation == Rotation::Right ? 1 : -1), start_cp.pos.y };
                    corner_pos = { stub_pos.x, junction_pos.y };
                }
                else
                {
                    stub_pos = { start_cp.pos.x, start_cp.pos.y + STUB_LENGTH * (start_cp.rotation == Rotation::Up ? -1 : 1) };
                    corner_pos = { junction_pos.x, stub_pos.y };
                }

                // adding intermediate points to the path
                if (stub_pos.x != start_cp.pos.x || stub_pos.y != start_cp.pos.y)
                {
                    path_points.push_back(stub_pos);
                }
                if ((corner_pos.x != stub_pos.x || corner_pos.y != stub_pos.y) && (corner_pos.x != junction_pos.x || corner_pos.y != junction_pos.y))
                {
                    path_points.push_back(corner_pos);
                }

                path_points.push_back(junction_pos);

                // add the wire
                vector<Connection_Point> final_wire_points;
                for(const auto& p : path_points)
                {
                    final_wire_points.push_back({ p, nullptr, {} });
                }
                add_Graphical_Wire(final_wire_points, common_node, common_node);
            }
        }
    }
}

void Controller::load_file(string name)
{

    int file_index = -1;
    for (int i = 0; i < file_handler.get_file_names().size(); i++)
    {
        if (name == file_handler.get_file_names()[i])
        {
            file_index = i;
            break;
        }
    }
    if (file_index == -1)
    {
        cout << "file not found" << endl;
        return;
    }
    circuit = textToGraphicalCircuit(name, file_handler.showText(file_index));

    build_graphical_elements_from_circuit();
}

void Controller::set_AC_sweep_variables(double start_f, double end_f, double num_of_points_f, AC_Sweep_Type type)
{
    start_freq = start_f;
    end_freq = end_f;
    num_of_points = num_of_points_f;
    ac_sweep_type = type;

}

void Controller::get_ac_params(double &start, double &stop, double &num_of_points_f, AC_Sweep_Type &type)
{
    start = start_freq;
    stop = end_freq;
    num_of_points_f = num_of_points;
    type = ac_sweep_type;
}

void Controller::set_phase_sweep_variables(double start_p, double stop_p, double base_p,double num_of_points_p, Phase_Sweep_Type type)
{
    start_phase = start_p;
    end_phase = stop_p;
    fixed_frequency = base_p;
    num_of_points_phase = num_of_points_p;
    phase_sweep_type = type;
}

void Controller::get_phase_params(double &start_p, double &stop_p, double &num_of_points_p, double &fixed_p, Phase_Sweep_Type &type)
{
    start_p = start_phase;
    stop_p = end_phase;
    num_of_points_p = num_of_points_phase;
    fixed_p = fixed_frequency;
    type = phase_sweep_type;
}

void Controller::addAdmittance(MatrixXc& Y, int node1, int node2, ComplexNum val) {
    int n1 = (node1 > 0) ? node1 - 1 : -1;
    int n2 = (node2 > 0) ? node2 - 1 : -1;
    if (n1 >= 0) Y(n1, n1) += val;
    if (n2 >= 0) Y(n2, n2) += val;
    if (n1 >= 0 && n2 >= 0) {
        Y(n1, n2) -= val;
        Y(n2, n1) -= val;
    }
}

inline void safeAdd(MatrixXc& Y, int r, int c, ComplexNum v) {
    assert(r >= 0 && r < Y.rows() && c >= 0 && c < Y.cols());
    Y(r, c) += v;
}

#include <vector>
#include <utility>
#include <cmath>
#include <cassert>

#include <vector>
#include <utility>
#include <cmath>
#include <cassert>
#include <tuple>

void Controller::performACSweep(Circuit* circuit) {
    int i = 0;
    for (auto& node : circuit->get_Nodes())
    {
        if (!node->net_name.empty())
        {
            node->change_name(node->net_name);
            continue;
        }
        node->change_name("N" + to_string(++i));
    }


    // محاسبه فرکانس‌ها
    std::vector<double> freqs;
    if (ac_sweep_type == AC_Sweep_Type::Linear) {
        double step = (end_freq - start_freq) / (num_of_points - 1);
        for (int i = 0; i < num_of_points; i++)
            freqs.push_back(start_freq + i * step);
    } else { // فرض بر Decade یا Octave
        double base = (ac_sweep_type == AC_Sweep_Type::Decade) ? 10.0 : 2.0;
        double ratio = end_freq / start_freq;
        for (int i = 0; i < num_of_points; i++) {
            double factor = (double)i / (num_of_points - 1);
            freqs.push_back(start_freq * pow(base, factor * log(ratio) / log(base)));
        }
    }

    // دریافت نودها و محاسبه تعداد نودها و منابع ولتاژ
    vector<Node*> nodes = circuit->get_Nodes();
    int N = (int)nodes.size() - 1; // بدون احتساب زمین
    int M = freqs.size(); // تعداد فرکانس‌ها
    int VsrcCount = 0;
    for (auto* el : circuit->get_Elements())
        if (el->get_type() == Element_Type::Voltage_Source)
            ++VsrcCount;
    int size = N + VsrcCount;

    // ساخت نقشه نام به اندیس
    unordered_map<string, int> nodeIndex;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        nodeIndex[nodes[i]->get_name()] = (i == 0 ? -1 : i - 1);
    }

    // پیش‌تخصیص بردار برای ذخیره نتایج هر نود
    std::vector<std::pair<Node*, std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>>> acResults;
    for (int i = 1; i <= N; ++i) {
        acResults.push_back({nodes[i], {std::vector<double>(M), std::vector<double>(M), std::vector<double>(M)}});
    }

    // حل سیستم برای هر فرکانس
    for (int k = 0; k < M; ++k) {
        double f = freqs[k];
        double omega = 2 * M_PI * f;
        MatrixXc Y = MatrixXc::Zero(size, size);
        VectorXc I = VectorXc::Zero(size);

        int voltageIndex = N; // اندیس شروع معادلات اضافی

        for (auto* el : circuit->get_Elements()) {
            int n1 = nodeIndex[el->get_nodes().first->get_name()];
            int n2 = nodeIndex[el->get_nodes().second->get_name()];

            switch (el->get_type()) {
                case Element_Type::Resistor: {
                    ComplexNum G = 1.0 / el->get_value();
                    if (n1 >= 0) safeAdd(Y, n1, n1, G);
                    if (n2 >= 0) safeAdd(Y, n2, n2, G);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -G);
                        safeAdd(Y, n2, n1, -G);
                    }
                    break;
                }
                case Element_Type::Capacitor: {
                    ComplexNum Yc(0, omega * el->get_value()); // jωC
                    if (n1 >= 0) safeAdd(Y, n1, n1, Yc);
                    if (n2 >= 0) safeAdd(Y, n2, n2, Yc);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -Yc);
                        safeAdd(Y, n2, n1, -Yc);
                    }
                    break;
                }
                case Element_Type::Inductor: {
                    ComplexNum Yl(0, -1.0 / (omega * el->get_value())); // 1/(jωL)
                    if (n1 >= 0) safeAdd(Y, n1, n1, Yl);
                    if (n2 >= 0) safeAdd(Y, n2, n2, Yl);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -Yl);
                        safeAdd(Y, n2, n1, -Yl);
                    }
                    break;
                }
                case Element_Type::Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    assert(voltageIndex < I.size());
                    I(voltageIndex) = el->getAmplitude();
                    voltageIndex++;
                    break;
                }
            }
        }

        assert(voltageIndex == size);
        VectorXc V = Y.fullPivLu().solve(I);

        // ذخیره بزرگی و فاز برای همه نودها
        for (int j = 0; j < N; ++j) {
            auto& [freq_vec, mag_vec, phase_vec] = acResults[j].second;
            freq_vec[k] = f;
            mag_vec[k] = 20 * log10(std::abs(V(j)));
            phase_vec[k] = std::arg(V(j)) * 180.0 / M_PI;
        }
    }

    // تنظیم نتایج در مدار
    circuit->setAcVoltage( std::move(acResults) );
}

void Controller::performPhaseSweep(Circuit* circuit) {
    int i = 0;
    for (auto& node : circuit->get_Nodes())
    {
        if (!node->net_name.empty())
        {
            node->change_name(node->net_name);
            continue;
        }
        node->change_name("N" + to_string(++i));
    }

    // تولید فازها
    std::vector<double> phases;
    if (phase_sweep_type == Phase_Sweep_Type::Linear) {
        double step = (end_phase - start_phase) / (num_of_points - 1);
        for (int i = 0; i < num_of_points; i++)
            phases.push_back(start_phase + i * step);
    } else { // لگاریتمی (Decade یا Octave)
        double base = (phase_sweep_type == Phase_Sweep_Type::Decade) ? 10.0 : 2.0;
        double ratio = end_phase / start_phase;
        for (int i = 0; i < num_of_points; i++) {
            double factor = (double)i / (num_of_points - 1);
            phases.push_back(start_phase * pow(base, factor * log(ratio) / log(base)));
        }
    }

    // دریافت نودها و محاسبه تعداد نودها و منابع ولتاژ
    vector<Node*> nodes = circuit->get_Nodes();
    int N = (int)nodes.size() - 1; // بدون احتساب زمین
    int M = phases.size(); // تعداد فازها
    int VsrcCount = 0;
    for (auto* el : circuit->get_Elements())
        if (el->get_type() == Element_Type::AC_Voltage_Source)
            ++VsrcCount;
    int size = N + VsrcCount;

    // ساخت نقشه نام به اندیس
    unordered_map<string, int> nodeIndex;
    for (int i = 0; i < (int)nodes.size(); ++i) {
        nodeIndex[nodes[i]->get_name()] = (i == 0 ? -1 : i - 1);
    }

    // پیش‌تخصیص بردار برای ذخیره نتایج هر نود
    std::vector<std::pair<Node*, std::tuple<std::vector<double>, std::vector<double>, std::vector<double>>>> phaseResults;
    for (int i = 1; i <= N; ++i) {
        phaseResults.push_back({nodes[i], {std::vector<double>(M), std::vector<double>(M), std::vector<double>(M)}});
    }

    // یافتن منبع ولتاژ AC
    AC_Voltage_Source* voltageSource = nullptr;
    for (auto* el : circuit->get_Elements()) {
        if (el->get_type() == Element_Type::AC_Voltage_Source) {
            voltageSource = dynamic_cast<AC_Voltage_Source*>(el);
            if (voltageSource) break;
        }
    }
    assert(voltageSource != nullptr); // باید حداقل یک منبع ولتاژ AC وجود داشته باشد

    // فرکانس ثابت
    double omega = 2 * M_PI * fixed_frequency;

    // حل سیستم برای هر فاز
    for (int k = 0; k < M; ++k) {
        double phase_deg = phases[k];
        double phase_rad = phase_deg * M_PI / 180.0;
        voltageSource->setPhase(phase_deg);

        MatrixXc Y = MatrixXc::Zero(size, size);
        VectorXc I = VectorXc::Zero(size);

        int voltageIndex = N; // اندیس شروع معادلات اضافی

        for (auto* el : circuit->get_Elements()) {
            int n1 = nodeIndex[el->get_nodes().first->get_name()];
            int n2 = nodeIndex[el->get_nodes().second->get_name()];

            switch (el->get_type()) {
                case Element_Type::Resistor: {
                    ComplexNum G = 1.0 / el->get_value();
                    if (n1 >= 0) safeAdd(Y, n1, n1, G);
                    if (n2 >= 0) safeAdd(Y, n2, n2, G);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -G);
                        safeAdd(Y, n2, n1, -G);
                    }
                    break;
                }
                case Element_Type::Capacitor: {
                    ComplexNum Yc(0, omega * el->get_value());
                    if (n1 >= 0) safeAdd(Y, n1, n1, Yc);
                    if (n2 >= 0) safeAdd(Y, n2, n2, Yc);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -Yc);
                        safeAdd(Y, n2, n1, -Yc);
                    }
                    break;
                }
                case Element_Type::Inductor: {
                    ComplexNum Yl(0, -1.0 / (omega * el->get_value()));
                    if (n1 >= 0) safeAdd(Y, n1, n1, Yl);
                    if (n2 >= 0) safeAdd(Y, n2, n2, Yl);
                    if (n1 >= 0 && n2 >= 0) {
                        safeAdd(Y, n1, n2, -Yl);
                        safeAdd(Y, n2, n1, -Yl);
                    }
                    break;
                }
                case Element_Type::AC_Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    AC_Voltage_Source* src = dynamic_cast<AC_Voltage_Source*>(el);
                    if (src) {
                        double amplitude = src->getAmplitude();
                        double phase_rad = src->getPhase() * M_PI / 180.0;
                        ComplexNum Vsrc = amplitude * ComplexNum(cos(phase_rad), sin(phase_rad));
                        I(voltageIndex) = Vsrc;
                    }
                    voltageIndex++;
                    break;
                }
            }
        }

        assert(voltageIndex == size);
        VectorXc V = Y.fullPivLu().solve(I);

        // ذخیره نتایج
        for (int j = 0; j < N; ++j) {
            auto& [phase_vec, mag_vec, phase_vec_out] = phaseResults[j].second;
            phase_vec[k] = phase_deg;
            mag_vec[k] = 20 * log10(std::abs(V(j)));
            phase_vec_out[k] = std::arg(V(j)) * 180.0 / M_PI;
        }
    }

    // ذخیره نتایج در مدار
    circuit->setPhaseVoltage( std::move(phaseResults) );
}