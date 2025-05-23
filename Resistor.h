//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_RESISTOR_H
#define PROJECT_PHAZE_ONE_RESISTOR_H

#include "Element.h"

class Resistor : public Element
{
public:
    double get_current() override;
    void display_info() override;
};


#endif //PROJECT_PHAZE_ONE_RESISTOR_H
