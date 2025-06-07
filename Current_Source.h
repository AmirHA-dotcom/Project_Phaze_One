//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_CURRENT_SOURCE_H
#define PROJECT_PHAZE_ONE_CURRENT_SOURCE_H

#include "Element.h"

class Current_Source : Element
{
public:
    void stamp(double time_step, vector<Triplet> &G_triplets, vector<double> &b, const vector<double> x_previous);
};


#endif //PROJECT_PHAZE_ONE_CURRENT_SOURCE_H
