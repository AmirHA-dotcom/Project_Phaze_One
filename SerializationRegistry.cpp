//
// Created by Arian Sadeghi on 9/3/25.
//
//
// SerializationRegistry.cpp
// Created to register all polymorphic types for cereal
//

#include "Libraries.h"
#include "Element.h"
#include "Resistor.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Current_Source.h"
#include "Voltage_Source.h"
#include "Real_Diode.h"
#include "Zener_Diode.h"
#include "CCCS.h"
#include "CCVS.h"
#include "VCCS.h"
#include "VCVS.h"
#include "Circuit.h"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/json.hpp>  // یا هر archive دیگه

// برای هر زیرکلاس Element
CEREAL_REGISTER_TYPE(Resistor);
CEREAL_REGISTER_TYPE(Capacitor);
CEREAL_REGISTER_TYPE(Inductor);
CEREAL_REGISTER_TYPE(Real_Diode);
CEREAL_REGISTER_TYPE(Zener_Diode);
CEREAL_REGISTER_TYPE(DC_Source);  // فرض کنم Voltage_Source پایه‌ست
CEREAL_REGISTER_TYPE(AC_Voltage_Source);
CEREAL_REGISTER_TYPE(Sine_Source);
CEREAL_REGISTER_TYPE(Pulse_Source);
CEREAL_REGISTER_TYPE(Square_Source);
CEREAL_REGISTER_TYPE(Triangular_Source);
CEREAL_REGISTER_TYPE(Delta_Dirac);
CEREAL_REGISTER_TYPE(Waveform_Voltage_Source);

// اگر رابطه پایه-زیرکلاس مستقیم serialize نمی‌شه، اضافه کن:
// CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, DC_Source); و غیره برای هر کدوم
// پایه‌ها#include <cereal/types/polymorphic.hpp>
//#include <cereal/archives/json.hpp>  // یا هر archive دیگه
//
//// برای هر زیرکلاس Element
//CEREAL_REGISTER_TYPE(Resistor);
//CEREAL_REGISTER_TYPE(Capacitor);
//CEREAL_REGISTER_TYPE(Inductor);
//CEREAL_REGISTER_TYPE(Real_Diode);
//CEREAL_REGISTER_TYPE(Zener_Diode);
//CEREAL_REGISTER_TYPE(DC_Source);  // فرض کنم Voltage_Source پایه‌ست
//CEREAL_REGISTER_TYPE(AC_Voltage_Source);
//CEREAL_REGISTER_TYPE(Sine_Source);
//CEREAL_REGISTER_TYPE(Pulse_Source);
//CEREAL_REGISTER_TYPE(Square_Source);
//CEREAL_REGISTER_TYPE(Triangular_Source);
//CEREAL_REGISTER_TYPE(Delta_Dirac);
//CEREAL_REGISTER_TYPE(Waveform_Voltage_Source);
//
//// اگر رابطه پایه-زیرکلاس مستقیم serialize نمی‌شه، اضافه کن:
//// CEREAL_REGISTER_POLYMORPHIC_RELATION(Voltage_Source, DC_Source); و غیره برای هر کدوم