// --- CORRECTED Errors.cpp (Minimal Change Version) ---
// This file works with the minimal-change Errors.h

#include "Errors.h"

// The key fix is to print the message AND return a valid (but empty) C-string.
// This stops the crash. The empty string "" is a valid const char*.

const char* invalidSyntax::what() const noexcept {
    cout << "Error: Syntax error" << endl;
    return "";
}
const char* duplicatePath::what() const noexcept {
    cout << "Error: This file already added to list" << endl;
    return "";
}

const char* invalidResistance::what() const noexcept {
    cout << "Error: Resistance cannot be zero or negative" << endl;
    return "";
}

const char* invalidInductance::what() const noexcept {
    cout << "Error: Inductance cannot be zero or negative" << endl;
    return "";
}

const char* invalidCapacity::what() const noexcept {
    cout << "Error: Capacity cannot be zero or negative" << endl;
    return "";
}
const char* sameNode::what() const noexcept {
    cout << "Error: Both nodes are same!" << endl;
    return "";
}

// For classes with custom messages, we build the message string first.
invalidDiodeModel::invalidDiodeModel(const std::string& name) {
    message = "Error: Model \"" + name + "\" not found in library";
}
const char* invalidDiodeModel::what() const noexcept {
    cout << message << endl; // Print the message
    return ""; // Return a valid string
}

elementExists::elementExists(const std::string& name) {
    message = "Error: Element \"" + name + "\" already exists in the circuit.";
}
const char* elementExists::what() const noexcept {
    cout << message << endl;

    return "";
}

elementNotFind::elementNotFind(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
}
const char* elementNotFind::what() const noexcept {
    cout << message << endl;
    return "";
}

notFindInLibrary::notFindInLibrary(const std::string& name) {
    message = "Error: Element \"" + name + "\" not found in library";
}
const char* notFindInLibrary::what() const noexcept {
    cout << message << endl;
    return "";
}

elementFind::elementFind(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
}
const char* elementFind::what() const noexcept {
    cout << message << endl;
    return "";
}

Error55::Error55(const std::string& name) {
    message = "Error: Cannot delete \"" + name + "\"; component not found";
}
const char* Error55::what() const noexcept {
    cout << message << endl;
    return "";
}

const char* InappropriateInput::what() const noexcept {
    cout << "Error : Inappropriate input" << endl;
    return "";
}

const char* invalidSchematicChoice::what() const noexcept {
    cout << "Error : invalid Schematic choice input" << endl;
    return "";
}

InvalidSyntaxForRenameNode::InvalidSyntaxForRenameNode(const std::string& oldName, const std::string& newName) {
    message = "ERROR: Invalid syntax for renaming node '" + oldName + "' to '" + newName + "'.\n"
               "Correct format:\n.rename node <old_name> <new_name>";
}

const char* InvalidSyntaxForRenameNode::what() const noexcept {
    cout << message << endl;
    return "";
}


circuitNotFind::circuitNotFind(const std::string& name) {
    message = "Error: Cannot find \"" + name + "\" ;try a correct circuit name";
}
const char* circuitNotFind::what() const noexcept {
    cout << message << endl;
    return "";
}

const char* circuitExists::what() const noexcept {
    cout << "Error: circuit already exists!" << endl;
    return "";
}

circuitNottFind::circuitNottFind(const std::string& name) {
    message = "Error: Cannot find \"" + name + "\" ;try a correct circuit name";
}
const char* circuitNottFind::what() const noexcept {
    cout << message << endl;
    return "";
}
