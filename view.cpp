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
    // Replace comma with dot
    for (char c : inputRaw) {
        if (c == ',') input += '.';
        else input += c;
    }

    // Map of suffix multipliers
    static const std::unordered_map<std::string, double> suffixes = {
            {"f", 1e-15}, {"p", 1e-12}, {"n", 1e-9}, {"u", 1e-6}, {"m", 1e-3},
            {"k", 1e3}, {"meg", 1e6}, {"g", 1e9}, {"t", 1e12}
    };

    // Separate numeric part and suffix
    size_t pos = 0;
    while (pos < input.size() && (std::isdigit(input[pos]) || input[pos] == '.' || input[pos] == '-')) {
        ++pos;
    }
    std::string numberPart = input.substr(0, pos);
    std::string suffixPart = input.substr(pos);

    double number = std::stod(numberPart);
    // Convert suffix to lowercase
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
    // پسوندهای معتبر SPICE
    static const std::unordered_set<std::string> validSuffixes = {
            "f", "p", "n", "u", "m", "k", "meg", "g", "t"
    };

    // اگر ورودی حاوی ',' باشد، نامعتبر است
    if (input.find(',') != std::string::npos)
        return false;

    // جدا کردن عدد و پسوند
    size_t i = 0;
    if (input[i] == '-') i++; // علامت منفی مجاز است

    bool hasDigit = false;
    int dotCount = 0;
    while (i < input.size() && (std::isdigit(input[i]) || input[i] == '.')) {
        if (std::isdigit(input[i])) hasDigit = true;
        if (input[i] == '.') dotCount++;
        i++;
    }

    if (!hasDigit || dotCount > 1)
        return false;

    std::string numberPart = input.substr(0, i);
    std::string suffixPart = input.substr(i);

    // تبدیل suffix به حروف کوچک
    std::transform(suffixPart.begin(), suffixPart.end(), suffixPart.begin(), ::tolower);

    // اگر پسوند هست ولی معتبر نیست → نامعتبر
    if (!suffixPart.empty() && validSuffixes.find(suffixPart) == validSuffixes.end())
        return false;

    // بررسی اینکه بخش عددی واقعاً عدد هست یا نه
    try {
        std::stod(numberPart);
    } catch (...) {
        return false;
    }

    return true;
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




bool inputHandler (string line) {
        vector<string> i = splitString(line);
        if (line == "end") {
            cout << "program ended!" << endl;
            return false;
        }
        if (addRCheck(i)) {
            auto* r = Controller::findResistor(i[4]);
            if (r) {
                cout << "Error: Resistor " << name> already exists in the circuit" << endl;
            }
        }
        if (delRCheck(i)) {
            auto* r = Controller::findResistor(i[1]);
            if (!r) {
                cout << "Error: Cannot delete resistor; component not found" << endl;
            }

        }
        if ( ) {

        }
        if ( ) {

        }
        if ( ) {

        }
        else
            throw invalidSyntax();
    return true;
}
