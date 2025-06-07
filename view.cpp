//
// Created by Arian Sadeghi on 6/7/25.
//

#include "view.h"
#include "Libraries.h"
#include "Errors.h"


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
bool addR (const vector<string> i) {
    if (i.size() != 5 || i[0] != "add" || i[1][0] != 'R'){
        return false;
    }
    if (stod(i[4]) <= 0 || ) {
        throw invalidSyntax();
    }


}
bool delR (const vector<string> i) {


}




bool inputHandler (string line) {
        vector<string> i = splitString(line);
        if (line == "end") {
            cout << "program ended!" << endl;
            return false;
        }
        if (addR(i)) {

        }
        if (delR(i)) {

        }
        if (delR(i)) {

        }
        if (delR(i)) {

        }
        if (delR(i)) {

        }
        else
            throw invalidSyntax();
}








    return true;
}
