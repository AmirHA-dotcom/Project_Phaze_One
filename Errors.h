// --- CORRECTED Errors.h (Minimal Change Version) ---
// You only need to replace this file and Errors.cpp

#ifndef PROJECT_PHAZE_ONE_ERRORS_H
#define PROJECT_PHAZE_ONE_ERRORS_H

#include "Libraries.h" // For cout, string, etc.
#include <exception>   // For std::exception

class invalidSyntax : public std::exception {
public:
    const char *what() const noexcept override;
};
class duplicatePath : public std::exception {
public:
    const char *what() const noexcept override;
};

class invalidResistance : public std::exception {
public:
    const char *what() const noexcept override;
};

class invalidInductance : public std::exception {
public:
    // Added override for consistency
    const char *what() const noexcept override;
};
class sameNode : public std::exception {
public:
    // Added override for consistency
    const char *what() const noexcept override;
};

class invalidCapacity : public std::exception {
public:
    const char *what() const noexcept override;
};

class invalidDiodeModel : public std::exception {
private:
    // Using a mutable string allows us to modify it in a const function,
    // which is needed for this specific minimal-change fix.
    mutable std::string message;
public:
    explicit invalidDiodeModel(const std::string& name);
    const char* what() const noexcept override;
};

class elementExists : public std::exception {
private:
    mutable std::string message;
public:
    explicit elementExists(const std::string& name);
    const char* what() const noexcept override;
};

class elementNotFind : public std::exception {
private:
    mutable std::string message;
public:
    explicit elementNotFind(const std::string& name);
    const char* what() const noexcept override;
};

class notFindInLibrary : public std::exception {
private:
    mutable std::string message;
public:
    explicit notFindInLibrary(const std::string& name);
    const char* what() const noexcept override;
};

// ... and so on for all your other classes ...
// The pattern is the same. For brevity, I'll provide the .cpp file
// with all the implementations which is the more important part.

class elementFind : public std::exception {
private:
    mutable std::string message;
public:
    explicit elementFind(const std::string& name);
    const char* what() const noexcept override;
};

class Error55 : public std::exception {
private:
    mutable std::string message;
public:
    explicit Error55(const std::string& name);
    const char* what() const noexcept override;
};

class InappropriateInput : public std::exception {
public:
    const char *what() const noexcept override;
};

class invalidSchematicChoice : public std::exception {
public:
    const char *what() const noexcept override;
};

class InvalidSyntaxForRenameNode : public std::exception {
private:
    mutable std::string message;
public:
    explicit InvalidSyntaxForRenameNode(const std::string& oldName, const std::string& newName);
    const char* what() const noexcept override;
};
class circuitExists : public std::exception {
public:
    const char *what() const noexcept override;
};

class circuitNotFind : public std::exception {
private:
    mutable std::string message;
public:
    explicit circuitNotFind(const std::string& name);
    const char* what() const noexcept override;
};

class circuitNottFind : public std::exception {
private:
    mutable std::string message;
public:
    explicit circuitNottFind(const std::string& name);
    const char* what() const noexcept override;
};

#endif //PROJECT_PHAZE_ONE_ERRORS_H
