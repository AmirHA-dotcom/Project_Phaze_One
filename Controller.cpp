//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Element.h"
#include "Controller.h"
#include "Libraries.h"

// helper
vector<double> separating_values_by_dash(const string& input_string)
{
    vector<double> numbers;
    stringstream ss(input_string);
    string token;

    while (getline(ss, token, '-'))
    {
        try
        {
            if (!token.empty())
            {
                numbers.push_back(toValue(token));
            }
        }
        catch (const exception& e)
        {
            cerr << "Warning: Could not parse token '" << token << "'. Skipping..." << endl;
        }
    }

    return numbers;
}

// helper
vector<pair<double, double>> create_data_points_from_strings(const string& time_string, const string& voltage_string)
{
    vector<double> times = separating_values_by_dash(time_string);
    vector<double> voltages = separating_values_by_dash(voltage_string);

    vector<pair<double, double>> data_points;

    int num_points = min(times.size(), voltages.size());

    for (int i = 0; i < num_points; ++i)
    {
        data_points.push_back({voltages[i], times[i]});
    }

    return data_points;
}


Controller::Controller() {
    circuit = nullptr;

    circuit = new Circuit("default_circuit");
    circuits.push_back(circuit);
}

void Controller::addCircuit(string name){
    auto* c = new Circuit(name);
    circuits.push_back(c);
}
void Controller::addSubCircuit(string name, Circuit* circuit, shared_ptr<Node> inputNode, std::shared_ptr<Node> outputNode) {
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
void Controller::addGraphicalSubCircuitToCircuit(SubCircuit* subCircuit, Circuit* circuit, const std::string& inputNodeName,
                                                 const std::string& outputNodeName,int screenX, int screenY) {
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

    for (auto& node : subCircuit->get_Nodes()) {
        if (node->get_name() != subCircuit->getInput()->get_name() && node->get_name() != subCircuit->getOutput()->get_name()) {
            std::string new_name = node->get_name() + unique_suffix;
            node->change_name(new_name);
            circuit->addNode(node);
            //circuit->create_new_node(new_name);
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
shared_ptr<Element> Controller::findElement (string name){
    for (auto& e : circuit->get_Elements())
    {
        if (e->get_name() == name)
            return e;
    }
    return nullptr;
}

shared_ptr<Node> Controller::findNode (string name){
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
    vector<shared_ptr<Node>> nodes = circuit->get_Nodes();
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
    vector<shared_ptr<Element>> elements = circuit->get_Elements();
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
    vector<shared_ptr<Element>> elements = circuit->get_Elements_of_type(Element_Type::Resistor);
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
    vector<shared_ptr<Element>> elements = circuit->get_Elements_of_type(Element_Type::Inductor);
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
    vector<shared_ptr<Element>> elements = circuit->get_Elements_of_type(Element_Type::Capacitor);
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
    vector<shared_ptr<Element>> elements = circuit->get_Elements_of_type(Element_Type::Real_Diode);
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
    shared_ptr<Node> node = findNode(oldName);
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
        shared_ptr<Node> node = findNode(i);

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
        shared_ptr<Element> element = findElement(i);
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

    for (shared_ptr<Element> component : circuit->get_Elements()) {
        string line;
        Element_Type type = component->get_type();
        string element_name = component->get_name();
        const pair<shared_ptr<Node>, shared_ptr<Node>> nodes = component->get_nodes();
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

    for (shared_ptr<Element> component : circuit->get_Elements()) {
        string line;
        Element_Type type = component->get_type();
        string element_name = component->get_name();
        const pair<shared_ptr<Node>, shared_ptr<Node>> nodes = component->get_nodes();
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
            case Element_Type::AC_Voltage_Source: //dodo
                line = "V" + element_name + " " + node1 + " " + node2 + " AC " + formatValue(value) + " " + to_string(component->get_x()) + " " + to_string(component->get_y()) + " " + to_string(component->get_rotation_as_int());
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
    for (auto node : circuit->get_Nodes()) { // jojo
        if (node->haveNetLabel()) {
            file << ".NET " << node->get_name() << " " << node->net_name << "\n";
            for (auto & c : node->get_net_label_coordinates_vector())
                file << ".NL " << node->get_name() << " " << c.first << " " << c.second << "\n";
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
                else if (tokens[3] == "AC" && tokens.size() >= 5) { //dodo
                    double val = Value(tokens[4]);
                    circuit->create_new_AC_voltage_source(element_name, n1, n2, val);
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
    vector<vector<string>> baseLines = lines;

    baseLines.erase(baseLines.begin() + 3);
    baseLines.erase(baseLines.begin() + 2);
    baseLines.erase(baseLines.begin() + 1);

    Circuit* baseCircuit = textToCircuit(Name, baseLines);

    // If textToCircuit didn't return a SubCircuit, create one
    SubCircuit* subCircuit = new SubCircuit();
    subCircuit->change_name(Name);
    // Copy elements and nodes from baseCircuit if necessary
    for (shared_ptr<Element> element : baseCircuit->get_Elements()) {
        subCircuit->addElement(element);
    }
    for (shared_ptr<Node> node : baseCircuit->get_Nodes()) {
        subCircuit->addNode(node);
    }
    for (auto sub : baseCircuit->getSubs()) {
        subCircuit->getSubs().push_back(sub);
    }
    delete baseCircuit; // Clean up if baseCircuit is not a SubCircuit

    // Map to store nodes by name for reuse
    unordered_map<string, shared_ptr<Node>> nodeMap;
    for (shared_ptr<Node> node : subCircuit->get_Nodes()) {
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
                nodeMap[nodeName] = make_shared<Node>(nodeName);
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
                nodeMap[nodeName] = make_shared<Node>(nodeName);
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
                else if (tokens[3] == "AC" && tokens.size() >= 8) { //dodo
                    double val = Value(tokens[4]);
                    circuit->create_new_AC_voltage_source(element_name, n1, n2, val);
                    circuit->findElement(element_name)->set_coordinates(stoi(tokens[5]),stoi(tokens[6]));
                    circuit->findElement(element_name)->set_rotation_by_int(stoi(tokens[7]));
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
            else if (tokens.size() >= 4 && tokens[0] == "GND") {
                // Example .GND a x y
                shared_ptr<Node> node = circuit->findNode(tokens[1]);
                node->make_ground();    node->set_ground_coordinates(stoi(tokens[2]), stoi(tokens[3]));
                circuit->ground(true);
            }
            else if (tokens.size() == 3 && tokens[0] == "NET") { //jojo
                // Example .NET Netname nodeName
                shared_ptr<Node> node = circuit->findNode(tokens[1]);
                node->net_name = tokens[2];
                // Example .NL nodeName x y
            }
            else if (tokens.size() == 4 && tokens[0] == "NL") { //jojo
                // Example .NL nodeName x y
                shared_ptr<Node> node = circuit->findNode(tokens[1]);
                node->get_net_label_coordinates_vector().push_back(make_pair(stoi(tokens[2]), stoi(tokens[3])));
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
    vector<vector<string>> baseLines = lines;

    baseLines.erase(baseLines.begin() + 3);
    baseLines.erase(baseLines.begin() + 2);
    baseLines.erase(baseLines.begin() + 1);

    Circuit* baseCircuit = textToCircuit(Name, baseLines);

    SubCircuit* subCircuit = new SubCircuit();

    subCircuit->change_name(Name);
    // Copy elements and nodes from baseCircuit
    for (shared_ptr<Element> element : baseCircuit->get_Elements()) {
        subCircuit->addElement(element);
    }
    for (shared_ptr<Node> node : baseCircuit->get_Nodes()) {
        subCircuit->addNode(node);
    }
    for (auto sub : baseCircuit->getSubs()) {
        subCircuit->getSubs().push_back(sub);
    }

    //delete baseCircuit; // Clean up if baseCircuit is not a SubCircuit

    // Map to store nodes by name for reuse
    unordered_map<string, shared_ptr<Node>> nodeMap;
    for (shared_ptr<Node> node : subCircuit->get_Nodes()) {
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
                nodeMap[nodeName] = make_shared<Node>(nodeName);
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
                nodeMap[nodeName] = make_shared<Node>(nodeName);
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
/**/
void Controller::loadSubCircuits () {
    string path = file_handler.getMainFolderPath() + "/Subcircuits/";
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
    string path = file_handler.getMainFolderPath() + "/Subcircuits/";
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

void Controller::delElement (shared_ptr<Element> element){
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
void Controller::add_Waveform_Voltage_Source(std::shared_ptr<Waveform_Voltage_Source> waveform) const {
    circuit->addElement(waveform); // shared_ptr<Waveform_Voltage_Source> implicitly converts to shared_ptr<Element>
}


// GRAPHICS!!!

void Controller::add_Graphical_Resistor(int screenX, int screenY) {
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    resistor_count++;
    string r_name = "R" + to_string(resistor_count);

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_resistor = make_shared<Resistor>(r_name, n1, n2, 1000.0);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_capacitor = make_shared<Capacitor>(c_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_inductor = make_shared< Inductor>(i_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_current_source = make_shared< Current_Source>(cs_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_real_diode = make_shared< Real_Diode>(RD_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_zener_diode = make_shared< Zener_Diode>(ZD_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< DC_Source>(VS_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< Sine_Source>(VS_name, n1, n2, 0, 0, 0, 0);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< Pulse_Source>(VS_name, n1, n2, 0, 0, 0, 0, 0, 0, 0);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< Delta_Dirac>(VS_name, n1, n2, 1e-6);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< Square_Source>(VS_name, n1, n2, 0, 0, 0, 0, 0);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_voltage_source = make_shared< Triangular_Source>(VS_name, n1, n2, 0, 0, 0, 0);

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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_source = make_shared<  AC_Voltage_Source>(VS_name, n1, n2, 1, 1, 0.0);

    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_Voltage_Source>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_WF_Source(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    voltage_source_count++;
    string VS_name = "VS" + to_string(voltage_source_count);

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_WF_voltage_source = make_shared<  Waveform_Voltage_Source>(VS_name, n1, n2);

    circuit->addElement(sim_WF_voltage_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_voltage_source = make_unique<Graphical_Voltage_Source>(sim_WF_voltage_source);
    gfx_sim_voltage_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_voltage_source));
}

void Controller::add_Graphical_VCVS(int screenX, int screenY)
{
    node_count++;
    string n1_name = "N" + to_string(node_count);
    node_count++;
    string n2_name = "N" + to_string(node_count);
    VCVS_count++;
    string name = "VCVS" + to_string(VCVS_count);

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_source = make_shared< VCVS>(name, n1, n2, 1, nullptr, nullptr);
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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_source = make_shared<VCCS>(name, n1, n2, 1, nullptr, nullptr);
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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_source = make_shared< CCVS>(name, n1, n2, 1, nullptr, nullptr);
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

    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

    n1->connect_element();
    n2->connect_element();

    auto sim_source = make_shared<  CCCS>(name, n1, n2, 1, nullptr, nullptr);
    circuit->addElement(sim_source);
    circuit->get_Elements().back()->set_coordinates(screenX, screenY);

    auto gfx_sim_source = make_unique<Graphical_CCCS>(sim_source);
    gfx_sim_source->bounding_box = {screenX, screenY, 100, 40};
    graphical_elements.push_back(move(gfx_sim_source));
}

void Controller::add_Graphical_Ground(SDL_Point pos, shared_ptr<Node> node)
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
    shared_ptr<Node> n1 = circuit->create_new_node(n1_name);
    shared_ptr<Node> n2 = circuit->create_new_node(n2_name);

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

Graphical_Net_Label* Controller::add_Graphical_Net_Label(SDL_Point pos, shared_ptr<Node> node)
{
    auto new_label = make_unique<Graphical_Net_Label>(pos, node);

    node->set_net_label_coordinates(pos.x, pos.y);


    TTF_Font* font = new_label->get_font();

    if (font && !node->net_name.empty())
    {
        int text_width, text_height;
        // size of text
        TTF_SizeText(font, node->net_name.c_str(), &text_width, &text_height);
        new_label->bounding_box = { pos.x, pos.y, text_width + 4, text_height + 4 };
    }
    else
    {
        new_label->bounding_box = { pos.x, pos.y, 30, 20 };
    }


    Graphical_Net_Label* ptr = new_label.get();
    graphical_elements.push_back(move(new_label));
    return ptr;
}

vector<unique_ptr<Graphical_Element>>& Controller::get_graphical_elements()
{
    return graphical_elements;
}

void Controller::update_element_properties(int element_index, const std::vector<std::string>& new_values)
{
    if (element_index < 0 || element_index >= graphical_elements.size()) return;

    std::shared_ptr<Element> element_model = graphical_elements[element_index]->get_model();
    if (!element_model || new_values.size() < 2) return;

    element_model->change_name(new_values[0]);

    if (auto resistor = std::dynamic_pointer_cast<Resistor>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            resistor->change_value(toValue(new_values[1]));
    }
    else if (auto capacitor = std::dynamic_pointer_cast<Capacitor>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            capacitor->change_value(toValue(new_values[1]));
    }
    else if (auto inductor = std::dynamic_pointer_cast<Inductor>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            inductor->change_value(toValue(new_values[1]));
    }
    else if (auto current = std::dynamic_pointer_cast<Current_Source>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            current->change_value(toValue(new_values[1]));
    }
    else if (auto diode = std::dynamic_pointer_cast<Real_Diode>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            diode->change_value(toValue(new_values[1]));
    }
    else if (auto zener = std::dynamic_pointer_cast<Zener_Diode>(element_model))
    {
        if (isValidSpiceNumber(new_values[1]))
            zener->change_value(toValue(new_values[1]));
    }
    else if (auto v_source = std::dynamic_pointer_cast<Voltage_Source>(element_model))
    {
        if (auto dc = std::dynamic_pointer_cast<DC_Source>(v_source))
        {
            if (new_values.size() > 1 && isValidSpiceNumber(new_values[1]))
                dc->set_value(toValue(new_values[1]));
        }
        else if (auto sine = std::dynamic_pointer_cast<Sine_Source>(v_source))
        {
            if (new_values.size() > 4)
            {
                if (isValidSpiceNumber(new_values[1])) sine->set_offset(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) sine->set_amplitude(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) sine->set_frequency(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) sine->set_phase_degrees(toValue(new_values[4]));
            }
        }
        else if (auto pulse = std::dynamic_pointer_cast<Pulse_Source>(v_source))
        {
            if (new_values.size() > 7)
            {
                if (isValidSpiceNumber(new_values[1])) pulse->set_v_initial(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) pulse->set_v_pulsed(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) pulse->set_time_delay(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) pulse->set_time_rise(toValue(new_values[4]));
                if (isValidSpiceNumber(new_values[5])) pulse->set_time_fall(toValue(new_values[5]));
                if (isValidSpiceNumber(new_values[6])) pulse->set_pulse_width(toValue(new_values[6]));
                if (isValidSpiceNumber(new_values[7])) pulse->set_period(toValue(new_values[7]));
            }
        }
        else if (auto square = std::dynamic_pointer_cast<Square_Source>(v_source))
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
        else if (auto tri = std::dynamic_pointer_cast<Triangular_Source>(v_source))
        {
            if (new_values.size() > 4)
            {
                if (isValidSpiceNumber(new_values[1])) tri->set_v_initial(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) tri->set_v_peak(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) tri->set_time_delay(toValue(new_values[3]));
                if (isValidSpiceNumber(new_values[4])) tri->set_period(toValue(new_values[4]));
            }
        }
        else if (auto delta = std::dynamic_pointer_cast<Delta_Dirac>(v_source))
        {
            if (new_values.size() > 1 && isValidSpiceNumber(new_values[1]))
                delta->set_time_of_delta(toValue(new_values[1]));
        }
        else if (auto AC = std::dynamic_pointer_cast<AC_Voltage_Source>(v_source))
        {
            if (new_values.size() > 3)
            {
                if (isValidSpiceNumber(new_values[1])) AC->set_amplitude(toValue(new_values[1]));
                if (isValidSpiceNumber(new_values[2])) AC->set_frequency(toValue(new_values[2]));
                if (isValidSpiceNumber(new_values[3])) AC->set_phase_degrees(toValue(new_values[3]));
            }
        }
        else if (auto WF = std::dynamic_pointer_cast<Waveform_Voltage_Source>(v_source))
        {
            if (new_values.size() > 2)
            {
                std::vector<std::pair<double, double>> data_points = create_data_points_from_strings(new_values[1], new_values[2]);
                WF->set_segments(data_points);
            }
        }
    }
}

vector<unique_ptr<Graphical_Wire>> &Controller::get_graphical_wires()
{
    return graphical_wires;
}

void Controller::add_Graphical_Wire(const vector<Connection_Point>& points, shared_ptr<Node> start, shared_ptr<Node> end) {
    auto new_wire = make_unique<Graphical_Wire>();
    for (const auto& p : points) {
        new_wire->path.push_back(p.pos);
    }

    new_wire->start_node = start;
    new_wire->end_node = end;
    graphical_wires.push_back(move(new_wire));
}

shared_ptr<Node> Controller::connect_nodes(shared_ptr<Node> node_to_keep, shared_ptr<Node> node_to_merge)
{
    if (!node_to_keep || !node_to_merge || node_to_keep == node_to_merge)
    {
        return node_to_keep ? node_to_keep : node_to_merge;
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
        for (const auto& coords : node_to_merge->get_net_label_coordinates_vector())
        {
            node_to_keep->set_net_label_coordinates(coords.first, coords.second);
        }

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

    return node_to_keep;
}

void Controller::assign_net_name(shared_ptr<Node> node_to_name, const string& new_name)
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
        shared_ptr<Node> existing_node = it->second;
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

    if (auto* g_label = dynamic_cast<Graphical_Net_Label*>(g_element_to_delete))
    {
        shared_ptr<Node> label_node = g_label->get_node();
        if (label_node && !label_node->net_name.empty())
        {
            string old_name = label_node->net_name;
            label_node->net_name.clear();

            named_nets.erase(old_name);
        }
    }

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

    for (std::shared_ptr<Element> model : logical_elements)
    {
        if (auto resistor = std::dynamic_pointer_cast<Resistor>(model))
        {
            auto gfx = std::make_unique<Graphical_Resistor>(resistor);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto capacitor = std::dynamic_pointer_cast<Capacitor>(model))
        {
            auto gfx = std::make_unique<Graphical_Capacitor>(capacitor);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto inductor = std::dynamic_pointer_cast<Inductor>(model))
        {
            auto gfx = std::make_unique<Graphical_Inductor>(inductor);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto current_source = std::dynamic_pointer_cast<Current_Source>(model))
        {
            auto gfx = std::make_unique<Graphical_Current_Source>(current_source);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto real_diode = std::dynamic_pointer_cast<Real_Diode>(model))
        {
            auto gfx = std::make_unique<Graphical_Real_Diode>(real_diode);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto zener_diode = std::dynamic_pointer_cast<Zener_Diode>(model))
        {
            auto gfx = std::make_unique<Graphical_Zener_Diode>(zener_diode);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());
            graphical_elements.push_back(std::move(gfx));
        }
        else if (auto voltage_source = std::dynamic_pointer_cast<Voltage_Source>(model))
        {
            auto gfx = std::make_unique<Graphical_Voltage_Source>(voltage_source);
            gfx->bounding_box = { model->get_x(), model->get_y(), 100, 40 };
            gfx->set_rotation_by_int(model->get_rotation_as_int());

            if (std::dynamic_pointer_cast<AC_Voltage_Source>(model))
            {
                gfx->make_AC();
            }

            graphical_elements.push_back(std::move(gfx));
        }
    }

    // net labels
    const auto& logical_nodes = circuit->get_Nodes();
    for (shared_ptr<Node> node : logical_nodes)
    {
        if (!node->net_name.empty())
        {
            //SDL_Point label_pos = {node->get_net_label_coordinates().first, node->get_net_label_coordinates().second };
            for (const auto& co : node->get_net_label_coordinates_vector())
            {
                    add_Graphical_Net_Label({co.first, co.second}, node);
            }
            //add_Graphical_Net_Label(label_pos, node);
        }
        if (node->is_the_node_ground())
        {
            SDL_Point ground_pos = { node->get_ground_coordinates().first, node->get_ground_coordinates().second };

            add_Graphical_Ground(ground_pos, node);
        }
    }

    // grouping all connection points by their shared node
    map<shared_ptr<Node>, vector<Connection_Point>> node_to_points_map;
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
        shared_ptr<Node> common_node = pair.first;
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

void Controller::get_phase_params_without_type(double &start_p, double &stop_p, double &num_of_points_p, double &fixed_p)
{
    start_p = start_phase;
    stop_p = end_phase;
    num_of_points_p = num_of_points_phase;
    fixed_p = fixed_frequency;
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
    vector<shared_ptr<Node>> nodes = circuit->get_Nodes();
    auto ground_it = find_if(nodes.begin(), nodes.end(),
                             [](const shared_ptr<Node>& n) {
                                 return n && n->is_the_node_ground();
                             });
    if (ground_it == nodes.end()) {
        throw runtime_error("No ground node found!");
    }
    if (ground_it != nodes.begin()) {
        iter_swap(nodes.begin(), ground_it);
    }

    // محاسبه تعداد نودها و منابع ولتاژ
    int N = nodes.size() - 1; // بدون زمین
    int M = freqs.size();     // تعداد فرکانس‌ها
    int VsrcCount = 0;
    for (const auto& el : circuit->get_Elements()) {
        if (el->get_type() == Element_Type::Voltage_Source ||
            el->get_type() == Element_Type::AC_Voltage_Source) {
            ++VsrcCount;
        }
    }
    if (VsrcCount == 0) {
        throw runtime_error("No voltage source found!");
    }
    int size = N + VsrcCount;

    // ساخت نقشه نام به اندیس
    unordered_map<string, int> nodeIndex;
    for (int i = 0; i < nodes.size(); ++i) {
        nodeIndex[nodes[i]->get_name()] = (i == 0 ? -1 : i - 1);
    }

    // پیش‌تخصیص بردار برای ذخیره نتایج
    vector<pair<shared_ptr<Node>, tuple<vector<double>, vector<double>, vector<double>>>> acResults;
    for (int i = 1; i <= N; ++i) {
        acResults.push_back({nodes[i], {vector<double>(M), vector<double>(M), vector<double>(M)}});
    }

    // حل سیستم برای هر فرکانس
    for (int k = 0; k < M; ++k) {
        double f = freqs[k];
        double omega = 2 * M_PI * f;
        MatrixXc Y = MatrixXc::Zero(size, size);
        VectorXc I = VectorXc::Zero(size);

        int voltageIndex = N;

        for (const auto& el : circuit->get_Elements()) {
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
                case Element_Type::Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    auto dc_src = dynamic_pointer_cast<DC_Source>(el);
                    if (dc_src && voltageIndex < size) {
                        I(voltageIndex) = dc_src->get_value();
                        voltageIndex++;
                    }
                    break;
                }
                case Element_Type::AC_Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    auto ac_src = dynamic_pointer_cast<AC_Voltage_Source>(el);
                    if (ac_src && voltageIndex < size) {
                        double amplitude = ac_src->getAmplitude();
                        double phase_rad = ac_src->getPhase() * M_PI / 180.0;
                        I(voltageIndex) = ComplexNum(amplitude * cos(phase_rad), amplitude * sin(phase_rad));
                        voltageIndex++;
                    }
                    break;
                }
            }
        }

        if (voltageIndex != size) {
            throw runtime_error("Voltage source count mismatch: expected " + to_string(size) +
                                ", got " + to_string(voltageIndex));
        }

        VectorXc V = Y.fullPivLu().solve(I);

        // ذخیره بزرگی و فاز
        for (int j = 0; j < N; ++j) {
            auto& [freq_vec, mag_vec, phase_vec] = acResults[j].second;
            freq_vec[k] = f;
            mag_vec[k] = 20 * log10(abs(V(j)));
            phase_vec[k] = arg(V(j)) * 180.0 / M_PI;
        }
    }
    circuit->setAcVoltage(move(acResults));
}

void Controller::performPhaseSweep(Circuit* circuit) {
    // تغییر نام نودها
    int i = 0;
    for (auto &node: circuit->get_Nodes()) {
        if (!node->net_name.empty()) {
            node->change_name(node->net_name);
            continue;
        }
        node->change_name("N" + std::to_string(++i));
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
            double factor = (double) i / (num_of_points - 1);
            phases.push_back(start_phase * pow(base, factor * log(ratio) / log(base)));
        }
    }

    vector<shared_ptr<Node>> nodes = circuit->get_Nodes();
    auto ground_it = find_if(nodes.begin(), nodes.end(),
                             [](const shared_ptr<Node>& n) {
                                 return n && n->is_the_node_ground();
                             });
    if (ground_it == nodes.end()) {
        throw runtime_error("No ground node found!");
    }
    if (ground_it != nodes.begin()) {
        iter_swap(nodes.begin(), ground_it);
    }

    // محاسبه تعداد نودها و منابع ولتاژ
    int N = nodes.size() - 1; // بدون زمین
    int M = phases.size();    // تعداد فازها
    int VsrcCount = 0;
    for (const auto& el : circuit->get_Elements()) {
        if (el->get_type() == Element_Type::Voltage_Source ||
            el->get_type() == Element_Type::AC_Voltage_Source) {
            ++VsrcCount;
        }
    }
    if (VsrcCount == 0) {
        throw runtime_error("No voltage source (DC or AC) found!");
    }
    int size = N + VsrcCount;

    // ساخت نقشه نام به اندیس
    unordered_map<string, int> nodeIndex;
    for (int i = 0; i < nodes.size(); ++i) {
        nodeIndex[nodes[i]->get_name()] = (i == 0 ? -1 : i - 1);
    }

    // پیش‌تخصیص بردار برای ذخیره نتایج
    vector<pair<shared_ptr<Node>, tuple<vector<double>, vector<double>, vector<double>>>> phaseResults;
    for (int i = 1; i <= N; ++i) {
        phaseResults.push_back({nodes[i], {vector<double>(M), vector<double>(M), vector<double>(M)}});
    }

    // پیدا کردن منبع ولتاژ AC
    shared_ptr<AC_Voltage_Source> voltageSource = nullptr;
    for (const auto& el : circuit->get_Elements()) {
        if (el->get_type() == Element_Type::AC_Voltage_Source || el->get_type() == Element_Type::Voltage_Source) {
            voltageSource = dynamic_pointer_cast<AC_Voltage_Source>(el);
            if (voltageSource) break;
        }
    }
    if (!voltageSource) {
        throw runtime_error("No AC voltage source found!");
    }

    // فرکانس ثابت
    double omega = 2 * M_PI * fixed_frequency;
    if (fixed_frequency <= 0) {
        throw runtime_error("Invalid fixed frequency!");
    }

    // حل سیستم برای هر فاز
    for (int k = 0; k < M; ++k) {
        double phase_deg = phases[k];
        double phase_rad = phase_deg * M_PI / 180.0;
        voltageSource->setPhase(phase_deg);

        // تعریف ماتریس و بردار
        MatrixXc Y = MatrixXc::Zero(size, size);
        VectorXc I = VectorXc::Zero(size);

        int voltageIndex = N;

        for (const auto& el : circuit->get_Elements()) {
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
                case Element_Type::Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    auto dc_src = dynamic_pointer_cast<DC_Source>(el);
                    if (dc_src && voltageIndex < size) {
                        I(voltageIndex) = dc_src->get_value();
                        voltageIndex++;
                    }
                    break;
                }
                case Element_Type::AC_Voltage_Source: {
                    if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                    if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                    if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                    if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                    auto src = dynamic_pointer_cast<AC_Voltage_Source>(el);
                    if (src && voltageIndex < size) {
                        double amplitude = src->getAmplitude();
                        double phase_rad = src->getPhase() * M_PI / 180.0;
                        ComplexNum Vsrc = amplitude * ComplexNum(cos(phase_rad), sin(phase_rad));
                        I(voltageIndex) = Vsrc;
                        voltageIndex++;
                    }
                    break;
                }
            }
        }

        if (voltageIndex != size) {
            throw runtime_error("Voltage source count mismatch: expected " + to_string(size) +
                                ", got " + to_string(voltageIndex));
        }

        VectorXc V = Y.fullPivLu().solve(I);

        // ذخیره نتایج
        for (int j = 0; j < N; ++j) {
            auto& [phase_vec, mag_vec, phase_vec_out] = phaseResults[j].second;
            phase_vec[k] = phase_deg;
            mag_vec[k] = 20 * log10(abs(V(j)));
            phase_vec_out[k] = arg(V(j)) * 180.0 / M_PI;
        }
    }

    // ذخیره نتایج در مدار
    circuit->setPhaseVoltage(move(phaseResults));
}

shared_ptr<Circuit> Controller::getTheveninEquivalent( SubCircuit& subCircuit) {
    // دریافت نودها و المان‌ها
    auto nodes = subCircuit.get_Nodes();
    auto elements = subCircuit.get_Elements();
    auto inputNode = subCircuit.getInput();
    auto outputNode = subCircuit.getOutput();

    // چک کردن نودهای ورودی و خروجی
    if (!inputNode || !outputNode) {
        throw runtime_error("Input or output node is null!");
    }
    if (inputNode == outputNode) {
        throw runtime_error("Input and output nodes cannot be the same!");
    }

    // پیدا کردن نود زمین
    auto ground_it = find_if(nodes.begin(), nodes.end(),
                             [](const shared_ptr<Node>& n) {
                                 return n && n->is_the_node_ground();
                             });
    if (ground_it == nodes.end()) {
        throw runtime_error("No ground node found in subcircuit!");
    }
    if (ground_it != nodes.begin()) {
        iter_swap(nodes.begin(), ground_it); // زمین به nodes[0]
    }

    // محاسبه تعداد نودها و منابع ولتاژ
    int N = nodes.size() - 1; // بدون زمین
    int VsrcCount = 0;
    for (const auto& el : elements) {
        if (el->get_type() == Element_Type::Voltage_Source ||
            el->get_type() == Element_Type::AC_Voltage_Source) {
            ++VsrcCount;
        }
    }
    if (VsrcCount == 0) {
        throw runtime_error("No voltage source found!");
    }
    int size = N + VsrcCount;

    // ساخت نقشه نام به اندیس
    unordered_map<string, int> nodeIndex;
    for (int i = 0; i < nodes.size(); ++i) {
        nodeIndex[nodes[i]->get_name()] = (i == 0 ? -1 : i - 1);
    }

    // -----------------------------------
    // 1. محاسبه ولتاژ تونن (Vth)
    // -----------------------------------
    MatrixXcd Y = MatrixXcd::Zero(size, size);
    VectorXcd I = VectorXcd::Zero(size);
    int voltageIndex = N;

    for (const auto& el : elements) {
        int n1 = nodeIndex[el->get_nodes().first->get_name()];
        int n2 = nodeIndex[el->get_nodes().second->get_name()];

        switch (el->get_type()) {
            case Element_Type::Resistor: {
                double G = 1.0 / el->get_value();
                if (n1 >= 0) safeAdd(Y, n1, n1, G);
                if (n2 >= 0) safeAdd(Y, n2, n2, G);
                if (n1 >= 0 && n2 >= 0) {
                    safeAdd(Y, n1, n2, -G);
                    safeAdd(Y, n2, n1, -G);
                }
                break;
            }
            case Element_Type::Voltage_Source: {
                if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                auto dc_src = dynamic_pointer_cast<DC_Source>(el);
                if (dc_src) {
                    I(voltageIndex) = dc_src->get_value();
                }
                voltageIndex++;
                break;
            }
            case Element_Type::AC_Voltage_Source: {
                if (n1 >= 0) safeAdd(Y, n1, voltageIndex, 1);
                if (n2 >= 0) safeAdd(Y, n2, voltageIndex, -1);
                if (n1 >= 0) safeAdd(Y, voltageIndex, n1, 1);
                if (n2 >= 0) safeAdd(Y, voltageIndex, n2, -1);
                auto ac_src = dynamic_pointer_cast<AC_Voltage_Source>(el);
                if (ac_src) {
                    // برای مدار مقاومتی، فقط amplitude استفاده می‌شه (DC equivalent)
                    double amplitude = ac_src->getAmplitude();
                    I(voltageIndex) = amplitude; // فرض RMS یا مقدار لحظه‌ای
                }
                voltageIndex++;
                break;
            }
            default:
                throw runtime_error("Unsupported element type for Thevenin!");
        }
    }

    if (voltageIndex != size) {
        throw runtime_error("Mismatch in voltage source count!");
    }

    // حل سیستم برای ولتاژها
    VectorXcd V = Y.fullPivLu().solve(I);

    // محاسبه Vth = V_input - V_output
    int inputIdx = nodeIndex[inputNode->get_name()];
    int outputIdx = nodeIndex[outputNode->get_name()];
    if (inputIdx < 0 || outputIdx < 0) {
        throw runtime_error("Input or output node is ground!");
    }
    double Vth = real(V(inputIdx) - V(outputIdx)); // فقط قسمت حقیقی (DC یا RMS)

    // -----------------------------------
    // 2. محاسبه مقاومت تونن (Rth)
    // -----------------------------------
    Y.setZero();
    I.setZero();
    voltageIndex = N;

    // اعمال منبع جریان تست (1 آمپر) بین input و output
    I(inputIdx) += 1.0;
    I(outputIdx) -= 1.0;

    // ساخت ماتریس Y با خاموش کردن منابع
    for (const auto& el : elements) {
        int n1 = nodeIndex[el->get_nodes().first->get_name()];
        int n2 = nodeIndex[el->get_nodes().second->get_name()];

        switch (el->get_type()) {
            case Element_Type::Resistor: {
                double G = 1.0 / el->get_value();
                if (n1 >= 0) safeAdd(Y, n1, n1, G);
                if (n2 >= 0) safeAdd(Y, n2, n2, G);
                if (n1 >= 0 && n2 >= 0) {
                    safeAdd(Y, n1, n2, -G);
                    safeAdd(Y, n2, n1, -G);
                }
                break;
            }
            case Element_Type::Voltage_Source:
            case Element_Type::AC_Voltage_Source:
                // اتصال کوتاه: نادیده گرفته می‌شه
                break;
            default:
                throw runtime_error("Unsupported element type for Thevenin!");
        }
    }

    // حل سیستم برای ولتاژهای تست
    V = Y.fullPivLu().solve(I);
    double V_test = real(V(inputIdx) - V(outputIdx));
    double Rth = V_test / 1.0; // I_test = 1

    // -----------------------------------
    // 3. ساخت مدار معادل تونن
    // -----------------------------------
    auto theveninCircuit = make_shared<Circuit>("Thevenin_" + subCircuit.get_name());

    // ایجاد نودها
    auto thInput = make_shared<Node>("TH_IN", false, 0);
    auto thOutput = make_shared<Node>("TH_OUT", true, -1); // خروجی زمین
    theveninCircuit->addNode(thInput);
    theveninCircuit->addNode(thOutput);

    // اضافه کردن منبع ولتاژ تونن (DC)
    theveninCircuit->addElement(make_shared<DC_Source>("V_TH", thInput, thOutput, Vth));

    // اضافه کردن مقاومت تونن (اگر غیرصفر)
    if (abs(Rth) > 1e-6) {
        theveninCircuit->addElement(make_shared<Resistor>("R_TH", thInput, thOutput, Rth));
    }

    return theveninCircuit;
}