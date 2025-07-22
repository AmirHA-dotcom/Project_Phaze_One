//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_VIEW_H
#define PROJECT_PHAZE_ONE_VIEW_H

#include "Libraries.h"
#include "Errors.h"
#include "Controller.h"
class View {
    bool mainMenu = true;
    bool circuitMenu = false;
    bool fileMenu = false;
    bool analysisMenu = false;

public:
    bool inputHandler (Controller* C);
    bool handleMainMenu (Controller* C);
    bool handleAnalysisMenu (Controller* C);
    bool handleFileMenu (Controller* C);
    bool handleCircuitMenu (Controller* C);

};


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
        if ((c < '0' || c > '9') && c != ')' && c != '.' && c != '(') return false;
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


#endif //PROJECT_PHAZE_ONE_VIEW_H
