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
double parseSpiceNumber(const std::string& inputRaw) {
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
    }
    if (stod(i[4]) <= 0 || isValidSpiceNumber(i[4]) ) {
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
    }
    if (stod(i[4]) <= 0 || isValidSpiceNumber(i[4]) ) {
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
    }
    if (stod(i[4]) <= 0 || isValidSpiceNumber(i[4]) ) {
        throw invalidInⅾuⅽtance();
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
    if (stod(i[4]) <= 0 || isValidDiodeName(i[4]) ) {
        Error::notFindElement(i[4]);
    }
    return true;
}
bool delDCheck (const vector<string> i) {
    if (i.size() != 2 || i[0] != "delete" || i[1][0] != 'D'){
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
    if (stod(i[4]) <= 0 || isValidDiodeName(i[4]) ) {
        throw invalidResistance();
    }
    return true;
}


bool inputHandler (string line) {
        vector<string> i = splitString(line);
        if (line == "end") {
            cout << "program ended!" << endl;
            return false;
        }
        if (addRCheck(i)) {
            if (Controller::findResistor(i[4])) {
                cout << "Error: Resistor " << i[4] << " already exists in the circuit" << endl;

            }

        }
        if (delRCheck(i)) {
            auto* r = Controller::findResistor(i[1]);
            if (!r) {
                cout << "Error: Cannot delete resistor; component not found" << endl;

            }

        }
        if (addCCheck(i)) {
            if (Controller::findCapacitor(i[4])) {
                cout << "Error: Capacitor " << i[4] << " already exists in the circuit" << endl;

            }

        }
        if (delCCheck(i)) {
            auto* r = Controller::findCapacitor(i[1]);
            if (!r) {
                cout << "Error: Cannot delete capacitor; component not found" << endl;

            }

        }
        if (addICheck(i)) {
            if (Controller::findInductor(i[4])) {
                cout << "Error: Inductor " << i[4] << " already exists in the circuit" << endl;

            }

        }
        if (delICheck(i)) {
            auto* r = Controller::findInductor(i[1]);
            if (!r) {
                cout << "Error: Cannot delete inductor; component not found" << endl;

            }

        }
        if (addDCheck(i)) {
            if (Controller::findDiode(i[4])) {
                cout << "Error: Diode " << i[4] << " already exists in the circuit" << endl;

            }

        }
        if (delDCheck(i)) {
            auto* r = Controller::findDiode(i[1]);
            if (!r) {
                cout << "Error: Cannot delete diode; component not found" << endl;

            }

        }
        if ( ) {

        }
        if ( ) {

        }


        else
            throw invalidSyntax();
    return true;
}
