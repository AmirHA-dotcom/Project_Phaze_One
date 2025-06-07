//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_CONTROLLER_H
#define PROJECT_PHAZE_ONE_CONTROLLER_H

#include "Element.h"

class Controller {
public:
    static Resistor* findResistor (string name);
    static Capacitor* findCapacitor (string name);
    static Inductor* findInductor (string name);
    static Diode* findDiode (string name);

};


#endif //PROJECT_PHAZE_ONE_CONTROLLER_H
