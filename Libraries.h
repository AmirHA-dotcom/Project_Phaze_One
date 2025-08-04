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
#include <fstream>
#include <filesystem>
#include <SDL.h>
#include <memory>
#include <SDL_ttf.h>
#include <optional>
#include <SDL_image.h>
#include <limits>
#include <iterator>
#include <complex>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

using Complex = std::complex<double>;
using MatrixXc = Eigen::Matrix<Complex, Eigen::Dynamic, Eigen::Dynamic>;
using VectorXc = Eigen::Matrix<Complex, Eigen::Dynamic, 1>;

extern const char* FONT;
extern const char* PROBE;
extern const char* DELETE;
extern const char* GROUND;

struct Triplet
{
    int Row;
    int Column;
    double Value;
    Triplet() : Row(0), Column(0), Value(0.0) {}
    Triplet(int r, int c, double v)
    {
        Row = r;
        Column = c;
        Value = v;
    }
};

enum class Element_Type{Resistor, Capacitor, Inductor, Voltage_Source, Real_Diode, Zener_Diode,
    Current_Source, VC_Voltage_Source, VC_Current_Source, CC_Voltage_Source, CC_Current_source};

#endif //PROJECT_PHAZE_ONE_LIBRARIES_H
