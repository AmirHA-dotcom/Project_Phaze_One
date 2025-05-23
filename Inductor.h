//
// Created by amirh on 2025-05-23.
//

#ifndef PROJECT_PHAZE_ONE_INDUCTOR_H
#define PROJECT_PHAZE_ONE_INDUCTOR_H

#include "Element.h"

class Inductor : public Element
{
    double get_current() override;
    void display_info() override;
};


#endif //PROJECT_PHAZE_ONE_INDUCTOR_H
