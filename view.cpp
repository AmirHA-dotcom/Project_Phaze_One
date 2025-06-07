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
static vector<string> splitString(const string& input) {
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
bool isValidDiodeName(const std::string& input){
    return (input == "D" || input == "Z");
}
bool addRCheck (vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'R'){
        return false;
    }
    if (i[0] != "add" && i[1][0] != 'R'){
        Error::notFindElement(i[4]);
        return false;
    }
    if (stod(i[4]) <= 0 || !isValidSpiceNumber(i[4]) ) {
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
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'C'){
        return false;
    }
    if (i[0] != "add" && i[1][0] != 'C'){
        Error::notFindElement(i[4]);
        return false;
    }
    if (stod(i[4]) <= 0 || !isValidSpiceNumber(i[4]) ) {
        throw invalidⅭapaⅽity();
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
    if (i[0] != "add" && i[1][0] != 'L'){
        Error::notFindElement(i[4]);
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
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'D'){
        return false;
    }
    if (stod(i[4]) <= 0 || !isValidDiodeName(i[4]) ) {
        Error::notFindElement(i[4]);
        return false;
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
    if (i.size() != 7 || i[0] != "add" || i[1][0] != 'V'){
        return false;
    }
    if (i.size() != 7 && i[0] != "add" && i[1][0] != 'V' && !i[4].find("SIN")){
        Error::notFindElement(i[4]);
        return false;

    }
    if (!isDigit(i[5]) || !isDigit(i[6])) {
        throw invalidSyntax();
    }
    return true;
}
bool componentListCheck (vector<string> i) {
    if (i.size() != 2 || i[0] != ".list"){
        return false;
    }
    if (i[1] != "Resistor" && i[1] != "Inⅾuⅽtor" && i[1] != "Capacitor" && i[1] != "Diode") {
        throw invalidSyntax();
    }
    return true;
}
bool renameNodeCheck (vector<string> i) {
    if (i.size() != 4 || i[0] != ".rename" || i[1] == "node"){
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
    if (i.size() < 7 || i[0] != ".print" || i[1] == "TRAN"){
        return false;
    }
    return true;
}
bool printDcCheck (vector<string> i) {
    if (i.size() < 7 || i[0] != ".print" || i[1] == "DC"){
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
    if (s == "1")
        circuitMenu = true;
    if (s == "2")
        fileMenu = true;
    if (s == "3")
        analysisMenu = true;
    else {
        mainMenu = true;
        throw invalidSyntax();
    }
    return true;
}
bool View::handleCircuitMenu (Controller* C) {
    string line;
    getline(cin,line);
    vector<string> i = splitString(line);
    if (line == "end") {
        cout << "program ended!" << endl;
        return false;
    }
    if (line == "return") {
        cout << "program ended!" << endl;
        analysisMenu = false;
        mainMenu = true;
        return true;
    }
    if (addRCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addR(i[1],i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delRCheck(i)) {
        auto* r = C->findElement(i[1]);
        if (!r) {
            cout << "Error: Cannot delete resistor; component not found" << endl;
        }
        C->delElement(r);
        return true;
    }
    if (addCCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addC(i[1],i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delCCheck(i)) {
        auto* r = C->findElement(i[1]);
        if (!r) {
            throw elementFind(i[1]);
        }
        C->delElement(r);
        return true;
    }
    if (addICheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addI(i[1],i[2],i[3],toValue(i[4]));
        return true;
    }
    if (delICheck(i)) {
        auto* r = C->findElement(i[1]);
        if (!r) {
            throw elementFind(i[1]);
        }
        C->delElement(r);
        return true;
    }
    if (addDCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addD(i[1],i[2],i[3],i[4]);
        return true;
    }
    if (delDCheck(i)) {
        auto* r = C->findElement(i[1]);
        if (!r) {
            throw elementFind(i[1]);
        }
        C->delElement(r);
        return true;
    }
    if (addGCheck(i)) {
        if (C->findNode(i[4])) {
            throw elementExists("GND " + i[4]);
        }
        C->addGND(i[2]);
        return true;
    }

    if (addVSCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addVS(i[1],i[2],i[3],toValue(i[4]));
        return true;
    }
    if (addCSCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addCS(i[1],i[2],i[3],toValue(i[4]));
        return true;
    }
    if (addSinusoidalCheck(i)) {
        if (C->findElement(i[4])) {
            throw elementExists(i[4]);
        }
        C->addSin(i[1],i[2],i[3],toValue(i[4].substr(4,i[4].size()-4)),toValue(i[5]),toValue(i[5]));
        return true;

    }
    if (line == ".nodes"){

    }
    if (line == ".list"){

    }
    if (componentListCheck(i)){

    }
    if (renameNodeCheck(i)){

    }



    else
        throw invalidSyntax();
    return true;
}
bool View::handleFileMenu (Controller* C) {

}
bool View::handleAnalysisMenu (Controller* C) {
    string line;
    getline(cin,line);
    vector<string> i = splitString(line);
    if (line == "end") {
        cout << "program ended!" << endl;
        return false;
    }
    if (line == "return") {
        cout << "program ended!" << endl;
        analysisMenu = false;
        mainMenu = true;
        return true;
    }
    if (tranAnalysisCheck(i)) {


    }
    if (dcAnalysisCheck(i)) {


    }
    if (printTranCheck(i)) {


    }
    if (printDcCheck(i)) {


    }
    else
        throw invalidSyntax();
    return true;
}

bool View::inputHandler (Controller* C) {
    if (mainMenu)
        return handleMainMenu(C);
    if (circuitMenu)
        return handleCircuitMenu(C);
    if (fileMenu)
        return handleFileMenu(C);
    if (analysisMenu)
        return handleAnalysisMenu(C);
    else
        cout << "all menus are off !!!" << endl;
    return false;
}

