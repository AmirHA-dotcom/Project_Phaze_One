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
#include "Graphical_Wire.h"

enum class AC_Sweep_Type { Octave, Decade, Linear };
enum class Phase_Sweep_Type { Octave, Decade, Linear };

inline double toValue(const std::string& inputRaw) {
    std::string input;
    for (char c : inputRaw) {
        if (c == ',') input += '.';
        else input += c;
    }

    static const std::unordered_map<std::string, double> suffixes = {
            {"f", 1e-15}, {"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3},
            {"k", 1e3}, {"meg", 1e6}, {"g", 1e9}, {"t", 1e12}
    };

    // پیدا کردن جایی که suffix شروع میشه (یعنی جایی که دیگه e-style number تموم شده)
    size_t pos = 0;
    bool eSeen = false;
    while (pos < input.size()) {
        char c = input[pos];
        if (std::isdigit(c) || c == '.' || c == '-' || c == '+') {
            pos++;
        } else if ((c == 'e' || c == 'E') && !eSeen) {
            eSeen = true;
            pos++;
        } else {
            break;
        }
    }

    std::string numberPart = input.substr(0, pos);
    std::string suffixPart = input.substr(pos);

    double number = std::stod(numberPart);

    for (char& c : suffixPart) c = std::tolower(c);
    if (!suffixPart.empty()) {
        auto it = suffixes.find(suffixPart);
        if (it != suffixes.end()) {
            number *= it->second;
        } else {
            throw std::invalid_argument("Unknown suffix: " + suffixPart);
        }
    }

    return number;
}
inline bool isDigit(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if ((c < '0' || c > '9') && c != ')' && c != '.' && c != '(') return false;
    }
    return true;
}
inline bool isValidSpiceNumber(const std::string& input) {
    static const std::unordered_set<std::string> validSuffixes = {
            "f", "p", "n", "u", "m", "k", "meg", "g", "t"
    };

    if (input.find(',') != std::string::npos)
        return false;

    size_t i = 0;
    if (input[i] == '-') i++;

    bool hasDigit = false;
    bool eSeen = false;
    bool digitAfterE = false;
    int dotCount = 0;

    while (i < input.size()) {
        char c = input[i];
        if (std::isdigit(c)) {
            hasDigit = true;
            if (eSeen) digitAfterE = true;
        } else if (c == '.') {
            if (eSeen) break; // نقطه بعد از e نامعتبر است
            if (++dotCount > 1) return false;
        } else if (c == 'e' || c == 'E') {
            if (eSeen || !hasDigit) return false;
            eSeen = true;
            // انتظار علامت یا عدد بعد از e
            if (i + 1 < input.size() && (input[i+1] == '+' || input[i+1] == '-')) {
                i++; // علامت را رد کن
            }
            if (i + 1 >= input.size() || !std::isdigit(input[i+1])) return false;
        } else {
            break; // شروع suffix
        }
        i++;
    }

    if (!hasDigit || (eSeen && !digitAfterE)) return false;

    std::string numberPart = input.substr(0, i);
    std::string suffixPart = input.substr(i);

    std::transform(suffixPart.begin(), suffixPart.end(), suffixPart.begin(), ::tolower);
    if (!suffixPart.empty() && validSuffixes.find(suffixPart) == validSuffixes.end())
        return false;

    try {
        std::stod(numberPart);
    } catch (...) {
        return false;
    }

    return true;
}

class Controller {
private:
    File_Handler file_handler;

    // tansient
    double time_step = 0;
    double start_time = 0;
    double end_time = 0;

    // AC sweep
    double start_freq = 0;
    double end_freq = 0;
    double num_of_points = 0;
    AC_Sweep_Type ac_sweep_type = AC_Sweep_Type::Linear;

    // Phase sweep
    double start_phase = 0;
    double end_phase = 0;
    double num_of_points_phase = 0;
    double fixed_frequency = 0;
    Phase_Sweep_Type phase_sweep_type = Phase_Sweep_Type::Linear;

    // naming
    int resistor_count = 0;
    int capacitor_count = 0;
    int inductor_count = 0;
    int node_count = 0;
    int current_source_count = 0;
    int real_diode_count = 0;
    int zener_diode_count = 0;
    int voltage_source_count = 0;
    int VCVS_count = 0;
    int VCCS_count = 0;
    int CCVS_count = 0;
    int CCCS_count = 0;

    vector<unique_ptr<Graphical_Wire>> graphical_wires;

    vector<unique_ptr<Graphical_Element>> graphical_elements;

    map<string, shared_ptr<Node>> named_nets;
public:
    int get_voltage_source_count() {return voltage_source_count;}
    Controller();
    vector<Circuit *> circuits;
    vector<SubCircuit *> subCircuits;
    Circuit *circuit;
    void saveSubCircuits ();
    void saveGraphicalSubCircuits ();
    void loadSubCircuits ();
    void loadGraphicalSubCircuits ();
    File_Handler& getFile_Handler () { return file_handler; }
    Circuit *findCircuit(string name);
    void renameCircuit(Circuit* circuit,string name);
    void deleteCircuit(Circuit* circuit);
    shared_ptr<Element>findElement(string name);

    shared_ptr<Node> findNode(string name);
    void addCircuit(string name);
    void addSubCircuit(string name, Circuit* circuit, shared_ptr<Node> inputNode, shared_ptr<Node> outputNode);
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
    void saveSubCircuit(SubCircuit* SubCircuit, string path);

    void saveGraphicalCircuit(Circuit* circuit, string path);
    void saveGraphicalSubCircuit(SubCircuit* SubCircuit, string path);

    void addFileToCircuits(int fileIndex);

    void delElement(shared_ptr<Element>element);

    void showSchematics();

    void showFile(int num);

    bool validSchematicChoice(string s);

    void handleNewFile(string path);

    bool is_files_empty();
    void add_Waveform_Voltage_Source(shared_ptr<Waveform_Voltage_Source> waveform) const;

    // GRAPHICS!!!

    void add_Graphical_Resistor(int screenX, int screenY);

    void add_Graphical_Capacitor(int screenX, int screenY);

    void add_Graphical_Inductor(int screenX, int screenY);

    void add_Graphical_Current_Source(int screenX, int screenY);

    void add_Graphical_Real_Diode(int screenX, int screenY);

    void add_Graphical_Zener_Diode(int screenX, int screenY);

    void add_Graphical_DC_Source(int screenX, int screenY);

    void add_Graphical_Sin_Source(int screenX, int screenY);

    void add_Graphical_Pulse_Source(int screenX, int screenY);

    void add_Graphical_Dirac_Source(int screenX, int screenY);

    void add_Graphical_Square_Source(int screenX, int screenY);

    void add_Graphical_Triangular_Source(int screenX, int screenY);

    void add_Graphical_AC_Phase_Source(int screenX, int screenY);

    void add_Graphical_WF_Source(int screenX, int screenY);

    void add_Graphical_VCVS(int screenX, int screenY);

    void add_Graphical_VCCS(int screenX, int screenY);

    void add_Graphical_CCVS(int screenX, int screenY);

    void add_Graphical_CCCS(int screenX, int screenY);

    void add_Graphical_Ground(SDL_Point pos, shared_ptr<Node> node);

    void add_Graphical_Sub_Circuit(int screenX, int screenY, string name);

    Graphical_Net_Label* add_Graphical_Net_Label(SDL_Point pos, shared_ptr<Node> node);

    vector<unique_ptr<Graphical_Element>>& get_graphical_elements();

    void update_element_properties(int element_index, const vector<string>& new_values);

    vector<unique_ptr<Graphical_Wire>>& get_graphical_wires();

    void add_Graphical_Wire(const vector<Connection_Point>& points, shared_ptr<Node> start, shared_ptr<Node> end);
    static void addSubCircuitToCircuit(SubCircuit* subCircuit, Circuit* circuit, const std::string& inputNodeName, const std::string& outputNodeName);
    static void addGraphicalSubCircuitToCircuit(SubCircuit* subCircuit, Circuit* circuit, const std::string& inputNodeName, const std::string& outputNodeName,int screenX, int screenY);

    shared_ptr<Node> connect_nodes(shared_ptr<Node> node_to_keep, shared_ptr<Node> node_to_merge);

    void assign_net_name(shared_ptr<Node> node_to_name, const string& new_name);

    void set_transient_values(double stepTime, double stopTime, double startTime, double maxTimeStep);

    void get_tran_params(double& start, double& stop, double& step);

    void do_transient();

    void delete_element(Graphical_Element* g_element_to_delete);

    void delete_wire(Graphical_Wire* wire_to_delete);

    vector<string> get_file_names();

    void New_File();

    void build_graphical_elements_from_circuit();

    void load_file(string name);

    void set_AC_sweep_variables(double start_f, double end_f, double num_of_points_f, AC_Sweep_Type type);
    void get_ac_params(double& start, double& stop, double& num_of_points_f, AC_Sweep_Type& type);
    AC_Sweep_Type get_AC_sweep_t() { return ac_sweep_type; }

    void set_phase_sweep_variables(double start_p, double stop_p, double base_p,double num_of_points_p, Phase_Sweep_Type type);
    void get_phase_params(double& start_p, double& stop_p, double& num_of_points_p, double& fixed_p, Phase_Sweep_Type& type);
    void get_phase_params_without_type(double &start_p, double &stop_p, double &num_of_points_p, double &fixed_p);

    void addAdmittance(MatrixXc& Y, int node1, int node2, ComplexNum val);
    void performACSweep(Circuit* circuit);
    void performPhaseSweep(Circuit* circuit);

};
#endif //PROJECT_PHAZE_ONE_CONTROLLER_H
