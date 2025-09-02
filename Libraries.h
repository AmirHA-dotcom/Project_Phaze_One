//
// Created by amirh on 2025-06-03.
//

#ifndef PROJECT_PHAZE_ONE_LIBRARIES_H
#define PROJECT_PHAZE_ONE_LIBRARIES_H


#include <cereal/types/string.hpp>       // برای std::string
#include <cereal/types/vector.hpp>       // برای std::vector
#include <cereal/types/map.hpp>          // برای std::map
#include <cereal/types/unordered_map.hpp>// برای std::unordered_map
#include <cereal/types/unordered_set.hpp>// برای std::unordered_set
#include <cereal/types/utility.hpp>      // برای std::pair
#include <cereal/types/tuple.hpp>        // برای std::tuple
#include <cereal/types/optional.hpp>     // برای std::optional (C++17)
#include <cereal/types/memory.hpp>       // برای std::shared_ptr و std::unique_ptr
#include <cereal/types/base_class.hpp>       // برای serialize کردن کلاس پایه
#include <cereal/types/polymorphic.hpp>      // برای ثبت کلاس‌های مشتق
#include <cereal/archives/binary.hpp>        // برای ذخیره‌سازی باینری
#include <cereal/archives/json.hpp>          // برای ذخیره‌سازی JSON (مناسب برای دیباگ)
#include <cereal/archives/xml.hpp>           // اگر خواستی از XML استفاده کنی
#include <cereal/cereal.hpp>                // هدر اصلی cereal

// Fix any previous macros from other libs
#ifdef Success
#undef Success
#endif
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <Eigen/Dense>
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
#include <tuple>
#include <vector>
#include <utility>
#include <cmath>
#include <cassert>

using namespace Eigen;
using namespace std;

using ComplexNum = std::complex<double>;
using MatrixXc = Eigen::Matrix<ComplexNum, Eigen::Dynamic, Eigen::Dynamic>;
using VectorXc = Eigen::Matrix<ComplexNum, Eigen::Dynamic, 1>;

extern const char* FONT;
extern const char* PROBE;
extern const char* DELETE;
extern const char* GROUND;
extern const char* TICK;

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
    Current_Source, VC_Voltage_Source,AC_Voltage_Source, VC_Current_Source, CC_Voltage_Source, CC_Current_source};

#endif //PROJECT_PHAZE_ONE_LIBRARIES_H
