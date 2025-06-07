//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Libraries.h"

#ifndef PROJECT_PHAZE_ONE_ERRORS_H
#define PROJECT_PHAZE_ONE_ERRORS_H

class Error {
public:
    static void notFindElement (string name);
};

class invalidSyntax : public exception {
public:
    const char *what() const noexcept override;
};

class invalidResistance : public exception {
public:
    const char *what() const noexcept override;
};

class invalidInⅾuⅽtance : public exception {
public:
    const char *what() const noexcept;
};

class invalidⅭapaⅽity : public exception {
public:
    const char *what() const noexcept override;
};

class namme : public exception {
public:
    const char *what() const noexcept override;
};

class nname : public exception {
public:
    const char *what() const noexcept override;
};


#endif //PROJECT_PHAZE_ONE_ERRORS_H
