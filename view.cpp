//
// Created by Arian Sadeghi on 6/7/25.
//

#include "view.h"
#include "Libraries.h"
#include "Errors.h"
#include "Resistor.h"
#include "Circuit.h"
#include "Controller.h"
// we have  NULL function here !!!
vector<string> splitString(const string& input) {
    vector<string> words;
    istringstream iss(input);
    string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}
double toValue(const std::string& inputRaw) {
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
bool isDigit(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if ((c < '0' || c > '9') && c != ')') return false;
    }
    return true;
}
bool isValidSpiceNumber(const std::string& input) {
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
bool
isValidDiodeName(const std::string& input){
    return (input == "D" || input == "Z");
}
bool addRCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'R'){
        return false;
    }
    if (!isDigit(i[4]) || stod(i[4]) <= 0 || !isValidSpiceNumber(i[4]) ) {
        throw invalidResistance();

    }
    return true;
}
bool delRCheck (const vector<string> i) {
    if (i.size() != 2 || i[0] != "delete" || i[1][0] != 'R'){
        return false;
    }
    return true;
}
bool addCCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'C' || i[1].find("CurrentSourse") != string::npos) {
        return false;
    }
    if (stod(i[4]) <= 0 || !isValidSpiceNumber(i[4]) ) {
        throw invalidCapacity();
    }
    return true;
}
bool delCCheck (const vector<string> i) {
    if (i.size() != 2 || i[0] != "delete" || i[1][0] != 'C'){
        return false;
    }
    return true;
}
bool addICheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'L'){
        return false;
    }
    if (stod(i[4]) <= 0 || !isValidSpiceNumber(i[4]) ) {
        throw invalidInductance();
    }
    return true;
}
bool delICheck (const vector<string> i) {
    if (i.size() != 2 || i[0] != "delete" || i[1][0] != 'L'){
        return false;
    }
    return true;
}
bool addDCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add"){
        return false;
    }
    if (i[1][0] != 'D' && i[1][0] != 'R' && i[1][0] != 'L' && i[1][0] != 'C' && i[1][0] != 'G' && i[1][0] != 'E' &&
        i[1][0] != 'F' && i[1][0] != 'V' && i[1][0] != 'H' && i[1][0] != 'C'){
        throw notFindInLibrary(i[1]);
    }
    if (i[1][0] != 'D'){
        return false;
    }
    if ( !isValidDiodeName(i[4]) ) {
        throw invalidDiodeModel(i[4]);
    }
    return true;
}
bool delDCheck (const vector<string> i) {
    if (i.size() != 2 || i[0] != "delete" || i[1][0] != 'D'){
        return false;
    }
    return true;
}
bool addGCheck (vector<string> i) {
    if (i.size() != 3 || i[0] != "add" || i[1] != "GND"){
        return false;
    }
    return true;
}
bool delGCheck (const vector<string> i) {
    if (i.size() != 3 || i[0] != "delete" || i[1] == "GND"){
        return false;
    }
    return true;
}
bool addVSCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || !i[1].find("VoltageSource")){
        return false;
    }
//    if (i[0] != "add" && i[1][0] != 'C'){
//        Error::notFindElement(i[4]);
//    }
    if (!isDigit(i[4])) {
        throw invalidSyntax();
    }
    return true;
}
bool addCSCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || !i[1].find("CurrentSource")){
        return false;
    }
//    if (i[0] != "add" && i[1][0] != 'C'){
//        Error::notFindElement(i[4]);
//    }
    if (!isDigit(i[4])) {
        throw invalidSyntax();
    }
    return true;
}
bool addSinusoidalCheck (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V' || !i[4].find("SIN") ){
        return false;
    }
//    if (!i[4].find("SIN") ){
//        throw notFindInLibrary(i[4]);
//    }
    if (!isDigit(i[5])) {
        throw invalidSyntax();
    }
    return true;
}
bool addPulse1eCheck (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V' || !i[4].find("PULSE1")){
        return false;
    }
    if (!isDigit(i[6].substr(1,i[6].size()-2))) {
        throw invalidSyntax();
    }
    return true;
}
bool addPulse2Check (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V' || !i[4].find("PULSE2")){
        return false;
    }
    if (i[4] != "PULSE")
        throw notFindInLibrary(i[1]);
    if (!isDigit(i[6].substr(1,i[6].size()-2))) {
        throw invalidSyntax();
    }
    return true;
}
bool addPulse3Check (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V'){
        return false;
    }
    if (i[4] != "PULSE3")
        throw notFindInLibrary(i[1]);
    if (!isDigit(i[6].substr(1,i[6].size()-2))) {
        throw invalidSyntax();
    }
    return true;
}
bool addPulse4Check (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V'){
        return false;
    }
    if (i[4] != "PULSE4")
        throw notFindInLibrary(i[1]);
    if (!isDigit(i[6].substr(1,i[6].size()-2))) {
        throw invalidSyntax();
    }
    return true;
}
bool addVcVsCheck (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'E'){
        return false;
    }
    if (!isDigit(i[6])) {
        throw invalidSyntax();
    }
    return true;
}
bool addCcVsCheck (vector<string> i) {
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'G'){
        return false;
    }
    if (!isDigit(i[6])) {
        throw invalidSyntax();
    }
    return true;
}
bool addVcCsCheck (vector<string> i) {
    if (i.size() != 6 || i[0] != "add" || i[1][0] != 'H'){
        return false;
    }
    if (!isDigit(i[5])) {
        throw invalidSyntax();
    }
    return true;
}
bool addCcCsCheck (vector<string> i) {
    if (i.size() != 6 || i[0] != "add"){
        return false;
    }
    if (i[1][0] != 'F' && i[1][0] != 'G' && i[1][0] != 'E' && i[1][0] != 'H'){
        throw notFindInLibrary(i[1]);
    }
    if (i[1][0] != 'F'){
        return false;
    }
    if (!isDigit(i[5])) {
        throw invalidSyntax();
    }
    return true;
}


bool componentListCheck (vector<string> i) {
    if (i.size() != 2 || i[0] != ".list"){
        return false;
    }
    if (i[1] != "resistor" && i[1] != "induⅽtor" && i[1] != "capacitor" && i[1] != "diode") {
        throw invalidSyntax();
    }
    return true;
}
bool renameNodeCheck (vector<string> i) {
    if (i.size() != 4 || i[0] != ".rename" || i[1] != "node"){
        return false;
    }
    return true;
}
bool tranAnalysisCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != ".TRAN"){
        return false;
    }

    return true;
}
bool dcAnalysisCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != ".DC"){
        return false;
    }
    return true;
}
bool printTranCheck (vector<string> i) {
    if ( i.size() < 3 || i[0] != ".print" || i[1] != "TRAN"){
        return false;
    }
    return true;
}
bool printDcCheck (vector<string> i) {
    if (i.size() < 3 || i[0] != ".print" || i[1] == "DC"){
        return false;
    }
    return true;
}


bool View::handleMainMenu (Controller* C) {
    cout << " 1 -> circuitMenu" << endl;
    cout << " 2 -> fileMenu" << endl;
    cout << " 3 -> analysisMenu" << endl;
    string s;
    getline(cin,s);
    mainMenu = false;
    if (s == "end") {
        return false;
    }
    if (s == "1") {
        circuitMenu = true;
        return true;
    }
    else if (s == "2") {
        fileMenu = true;
        return true;
    }
    else if (s == "3") {
        analysisMenu = true;
        C->circuit = nullptr;
        return true;
    }
    else {
        mainMenu = true;
        throw invalidSyntax();
    }
}
bool View::handleCircuitMenu (Controller* C) {
    if (C->circuit == nullptr) {
        cout << "No circuit selected. Please add or switch to a circuit first." << endl;
    }
    string line;
    getline(cin,line);
    vector<string> i = splitString(line);
    if (line == "end") {
        cout << "program ended!" << endl;
        return false;
    }
    if (line == "return") {
        circuitMenu = false;
        mainMenu = true;
        return true;
    }
    if (line.empty()) {
        return true;
    }

    if (i.size() == 3 && i[0] == "add" && i[1] == "circuit") {
        // add circuit <Name>


        if (C->findCircuit(i[2])) {
            throw circuitExists();
        }
        C->addCircuit(i[2]);
        C->circuit = C->findCircuit(i[2]);
        return true;
    }
    if (i.size() == 3 && i[0] == "switch" && i[1] == "to") {
        if (!C->findCircuit(i[2])) {
            throw circuitNotFind(i[2]);
        }
        C->circuit = C->findCircuit(i[2]);
        return true;
    }
    if (i.size() == 2 && i[0] == "delete") {
        if (!C->findCircuit(i[1])) {
            throw circuitNotFind(i[1]);
        }
        C->deleteCircuit(C->findCircuit(i[1]));
        C->circuit = nullptr;
        return true;
    }
    if (i.size() == 3 && i[0] == "rename") {
        auto c = C->findCircuit(i[1]);
        if (!C->findCircuit(i[1])) {
            throw circuitNotFind(i[1]);
        }
        C->renameCircuit(c,i[2]);
        return true;
    }
    if (line == "show circuits") {
        C->showCircuits();
        return true;
    }
    if (addRCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addR(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delRCheck(i)) {

        auto* r = C->findElement(i[1].substr(1,i[1].size()-1));
        if (!r) {
            cout << "Error: Cannot delete resistor; component not found" << endl;
        }
        C->delElement(r);
        return true;
    }
    if (addCCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addC(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delCCheck(i)) {
        auto* r = C->findElement(i[1].substr(1,i[1].size()-1));
        if (!r) {
            throw elementFind(i[1].substr(1,i[1].size()-1));
        }
        C->delElement(r);
        return true;
    }
    if (addICheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addI(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delICheck(i)) {
        auto* r = C->findElement(i[1].substr(1,i[1].size()-1));
        if (!r) {
            throw elementFind(i[1].substr(1,i[1].size()-1));
        }
        C->delElement(r);
        return true;
    }
    if (addDCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addD(i[1].substr(1,i[1].size()-1),i[2],i[3],i[4]);
        return true;
    }
    if (delDCheck(i)) {
        auto* r = C->findElement(i[1].substr(1,i[1].size()-1));
        if (!r) {
            throw elementFind(i[1].substr(1,i[1].size()-1));
        }
        C->delElement(r);
        return true;
    }
    if (addGCheck(i)) {
        if (!C->findNode(i[2])) {
            throw elementNotFind("GND " + i[2]);
        }
        C->addGND(i[2]);
        return true;
    }
    if (delGCheck(i)) {
        if (C->findNode(i[2])) {
            throw elementExists("GND " + i[2]);
        }
        C->delGND(i[2]);
        return true;
    }
    if (addVSCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(13,i[1].size()-13))) {
            throw elementExists(i[1].substr(13,i[1].size()-13));
        }
        C->addVS(i[1].substr(13,i[1].size()-13),i[2],i[3],toValue(i[4]));
        return true;
    }
    if (addCSCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(13,i[1].size()-13))) {
            throw elementExists(i[1].substr(13,i[1].size()-13));
        }
        C->addCS(i[1].substr(13,i[1].size()-13),i[2],i[3],toValue(i[4]));
        return true;
    }
    if (addSinusoidalCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addSin(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[4].substr(4,i[4].size()-4)),
                  toValue(i[5]),toValue(i[6].substr(0,i[6].size()-1)));
        return true;
    }
    if (addPulse1eCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addPulse1(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[5]),toValue(i[6].substr(1,i[6].size()-2)));
        return true;
    }
    if (addPulse2Check(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addPulse2(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[5]),toValue(i[6].substr(1,i[6].size()-2)));
        return true;
    }
    if (addPulse3Check(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addPulse3(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[5]),toValue(i[6].substr(1,i[6].size()-2)));
        return true;
    }
    if (addPulse4Check(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addPulse4(i[1].substr(1,i[1].size()-1),i[2],i[3],toValue(i[5]));
        return true;
    }
    if (addVcVsCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addVcVs(i[1].substr(1,i[1].size()-1),i[2],i[3],i[4],i[5],toValue(i[6]));
        return true;
    }
    if (addCcVsCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addVcVs(i[1].substr(1,i[1].size()-1),i[2],i[3],i[4],i[5],toValue(i[6]));
        return true;
    }
    if (addVcCsCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addCcVs(i[1].substr(1,i[1].size()-1),i[2],i[3],i[4],toValue(i[5]));
        return true;
    }
    if (addCcCsCheck(i)) {
        if (i[2] == i[3])
            throw sameNode();
        if (C->findElement(i[1].substr(1,i[1].size()-1))) {
            throw elementExists(i[1].substr(1,i[1].size()-1));
        }
        C->addCcCs(i[1].substr(1,i[1].size()-1),i[2],i[3],i[4],toValue(i[5]));
        return true;
    }
    if (line == ".nodes"){
        C->showNodes();
          return true;
    }
    if (line == ".list"){
        C->showAllElements();
         return true;
    }
    if (componentListCheck(i)) {
        if(i[1] == "resistor") {
            C->showResistors();
        } else if (i[1] == "induⅽtor") {
            C->showInductors();
        } else if (i[1] == "capacitor") {
            C->showCapacitors();
        } else if (i[1] == "diode") {
            C->showDiodes();
        }
        else
            cout << "ERROR: Invalid component type" << endl;
        return true;
    }
    if (renameNodeCheck(i)) {
        if (!C->findNode(i[2])) {
            throw elementNotFind(i[2]);
        }
        C->renameNode(i[2],i[3]);
        return true;
    }
        throw invalidSyntax();
}
bool View::handleFileMenu (Controller* C) {
    C->showSchematics();
    string line;
    getline(cin,line);
    vector<string> i = splitString(line);
    if (line == "end") {
        cout << "program ended!" << endl;
        return false;
    }
    if (line == "return") {
        fileMenu = false;
        mainMenu = true;
        return true;
    }
    if (line.empty()) {
        return true;
    }
    if (C->validSchematicChoice(line)) {
        C->showFile(stoi(line));
        return true;
    }
    if (i.size() == 1 && isDigit(line) && !C->validSchematicChoice(line)) {
        throw invalidSchematicChoice();
    }
    if (i.size() == 2 && i[0] == "NewFile") {
        C->handleNewFile(i[1]);
        return true;
    }
        throw InappropriateInput();
}
bool View::handleAnalysisMenu (Controller* C) {
    string line;
    if (C->circuit == nullptr)
        cout << "Enter the name of the circuit in form <circuit> <name> to load: " << endl;
    getline(cin,line);
    vector<string> i = splitString(line);
    if (line == "end") {
        cout << "program ended!" << endl;
        return false;
    }
    if (line == "return") {
        analysisMenu = false;
        mainMenu = true;
        return true;
    }
    if (line.empty()) {
        return true;
    }
    if (i.size() == 2 && i[0] == "circuit"){
        C->circuit = C->findCircuit(i[1]);
        return true;
    }
    if (i.size() == 3 && i[0] == "switch" && i[1] == "to") {
        // add circuit <Name>
        if (!C->findCircuit(i[2])) {
            throw circuitNotFind(i[2]);
        }
        C->circuit = C->findCircuit(i[2]);
        return true;
    }
    if (i.size() == 3 && i[0] == "rename") {
        auto c = C->findCircuit(i[1]);
        if (!C->findCircuit(i[1])) {
            throw circuitNotFind(i[1]);
        }
        C->renameCircuit(c,i[2]);
        return true;
    }
    if (line == "show circuits") {
        C->showCircuits();
    }
    if (tranAnalysisCheck(i)) {
        if (!C->circuit->isGround()) {
            cout << "please Ground first !" << endl;
            return true;
        }
        C->tranAnalysis(toValue(i[1]), toValue(i[2]),toValue(i[3].substr(1,i[3].size()-2))
                        , toValue(i[4].substr(1,i[4].size()-2)));
        return true;
    }
    if (dcAnalysisCheck(i)) {
        C->DcAnalysis(toValue(i[1]), toValue(i[2]), toValue(i[3].substr(1,i[3].size()-2))
                      , toValue(i[3].substr(1,i[3].size()-2)));
        return true;
    }
    if (printTranCheck(i)) {    //  .print TRAN V(in) V(out) I(R1) I(D1)
        vector<string> orders;
        for (size_t j = 2; j < i.size(); ++j) {
            orders.push_back(i[j]);
        }
        //cout << orders.size() << endl;
        C->tranAnalysisOrders(orders);
        return true;
    }
    if (printDcCheck(i)) {        //  .print DC V(in) V(out) I(R1) I(D1)
        vector<string> orders;
        for (size_t j = 2; j < i.size(); ++j) {
            orders.push_back(i[j]);
        }
        C->DcAnalysisOrders(orders);
        return true;

    }
        throw invalidSyntax();
}

bool View::inputHandler (Controller* C) {
    if (mainMenu)
        return handleMainMenu(C);
    if (circuitMenu) {
        return handleCircuitMenu(C);
    }
    if (fileMenu)
        return handleFileMenu(C);
    if (analysisMenu)
        return handleAnalysisMenu(C);
    cout << "all menus are off !!!" << endl;
    return false;
}

