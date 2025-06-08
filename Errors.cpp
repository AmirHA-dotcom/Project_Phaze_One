//
// Created by Arian Sadeghi on 6/7/25.
//

#include "Errors.h"

const char  *invalidSyntax::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}
const char  *invalidResistance::what() const noexcept {
    cout << "Error: Resistance cannot be zero or negative" << endl;
}
const char  *invalidInductance::what() const noexcept {
    cout << "Error: Inⅾuⅽtance cannot be zero or negative" << endl;
}
const char  *invalidCapacity::what() const noexcept {
    cout << "Error: Capacity cannot be zero or negative" << endl;
}
const char  *invalidDiodeModel::what() const noexcept {
    cout << "Error: Wrong Diode model" << endl;
}
elementExists::elementExists(const std::string& name) {
    message = "Error: Element \"" + name + "\" already exists in the circuit.";
}
const char* elementExists::what() const noexcept {
    return message.c_str();
}
elementNotFind::elementNotFind(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
    ///                  cout << "Error: Cannot delete capacitor; component not found" << endl;
}
const char* elementNotFind::what() const noexcept {
    return message.c_str();
}
notFindInLibrary::notFindInLibrary(const std::string& name) {
    message = "Error: Element \"" + name + "\" not found in library";
    ///                  cout << "Error: Cannot delete capacitor; component not found" << endl;
}
const char* notFindInLibrary::what() const noexcept {
    return message.c_str();
}
elementFind::elementFind(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
    ///                  cout << "Error: Cannot delete capacitor; component not found" << endl;
}
const char* elementFind::what() const noexcept {
    return message.c_str();
}
elementFind::elementFind(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
    ///                  cout << "Error: Cannot delete capacitor; component not found" << endl;
}
const char* elementFind::what() const noexcept {
    return message.c_str();
}

const char  *nameee::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}
const char  *nname::what() const noexcept {
    cout << "Error: Syntax error" << endl;
}