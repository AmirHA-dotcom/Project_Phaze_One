//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Libraries.h"

#ifndef PROJECT_PHAZE_ONE_ERRORS_H
#define PROJECT_PHAZE_ONE_ERRORS_H

class Error {
public:

};

class invalidSyntax : public exception {
public:
    const char *what() const noexcept override;
};

class invalidResistance : public exception {
public:
    const char *what() const noexcept override;
};

class invalidInductance : public exception {
public:
    const char *what() const noexcept;
};

class invalidCapacity : public exception {
public:
    const char *what() const noexcept override;
};
class invalidDiodeModel : public exception {
public:
    const char *what() const noexcept override;
};
class elementExists : public std::exception {
private:
    std::string message;
public:
    explicit elementExists(const std::string& name);
    const char* what() const noexcept override;
};
class elementNotFind : public std::exception {
private:
    std::string message;
public:
    explicit elementNotFind(const std::string& name);
    const char* what() const noexcept override;
};
class notFindInLibrary : public std::exception {
private:
    std::string message;
public:
    explicit notFindInLibrary(const std::string& name);
    const char* what() const noexcept override;
};
class elementFind : public std::exception {
private:
    std::string message;
public:
    explicit elementFind(const std::string& name);
    const char* what() const noexcept override;
};
class elementFind : public std::exception {
private:
    std::string message;
public:
    explicit elementFind(const std::string& name);
    const char* what() const noexcept override;
};


class nname : public exception {
public:
    const char *what() const noexcept override;
};


#endif //PROJECT_PHAZE_ONE_ERRORS_H
