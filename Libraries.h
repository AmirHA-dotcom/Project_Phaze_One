//
// Created by amirh on 2025-06-03.
//

#ifndef PROJECT_PHAZE_ONE_LIBRARIES_H
#define PROJECT_PHAZE_ONE_LIBRARIES_H

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <sstream>
#include <string>
#include <regex>
#include <stdexcept>
#include <cmath>
#include <map>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Triplet
{
    int Row;
    int Column;
    int Value;
};

enum class Element_Type{Resistor, Capacitor, Inductor, Voltage_Source, Real_Diode, Zener_Diode,
    Current_Source, VC_Voltage_Source, VC_Current_Source, CC_Voltage_Source, CC_Current_source};

#endif //PROJECT_PHAZE_ONE_LIBRARIES_H
