//
// Created by amirh on 2025-06-07.
//

#ifndef PROJECT_PHAZE_ONE_ALGORITHEMS_H
#define PROJECT_PHAZE_ONE_ALGORITHEMS_H

#include "Libraries.h"

class Algorithems
{
public:
    // solving determinants
    float determinant(vector<vector<float>> matrix);
    vector<float> solve_cramer(vector<vector<float>> G, vector<float> I);
}


#endif //PROJECT_PHAZE_ONE_ALGORITHEMS_H
