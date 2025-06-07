//
// Created by Arian Sadeghi on 6/7/25.
//
#include "Libraries.h"

#ifndef PROJECT_PHAZE_ONE_ERRORS_H
#define PROJECT_PHAZE_ONE_ERRORS_H


class invalidSyntax : public exception {
public:
    const char *what() const noexcept override;
};

class name : public exception {
public:
    const char *what() const noexcept override;
};

class namee : public exception {
public:
    const char *what() const noexcept override;
};

class nameee : public exception {
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
