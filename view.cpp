//
// Created by Arian Sadeghi on 6/7/25.
//

#include "view.h"

static vector<string> splitString(const string& input) {
    vector<string> words;
    istringstream iss(input);
    string word;
    while (iss >> word) {
        words.push_back(word);
    }
    return words;
}
void handleInput () {
    string line,s;
    while (true) {
        getline(cin,line);
        vector<string> i = splitString(line);
        if (line == "end")
            break;


    }
