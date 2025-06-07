//
// Created by Arian Sadeghi on 6/7/25.
//

#include "Errors.h"
void notFindElement (string name){
    cout << "Error: Element " << name << " not found in library" << endl;
}

const char  *invalidSyntax::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}
const char  *invalidResistance::what() const noexcept {
    cout << "Error: Resistance cannot be zero or negative" << endl;
}
const char  *invalidInⅾuⅽtance::what() const noexcept {
    cout << "Error: Inⅾuⅽtance cannot be zero or negative" << endl;
}const char  *invalidⅭapaⅽity::what() const noexcept {
    cout << "Error: Capacity cannot be zero or negative" << endl;
}const char  *invalidResistance::what() const noexcept {
    cout << "Error: Resistance cannot be zero or negative" << endl;
}
const char  *nammme::what() const noexcept {
    cout << "Error:  in library" << endl;
}
const char  *nameee::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}
const char  *nname::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}