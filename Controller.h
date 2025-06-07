//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_CONTROLLER_H
#define PROJECT_PHAZE_ONE_CONTROLLER_H

#include "Element.h"

class Controller {
public:
    Element* findElement (string name);
    Element* findNode (string name);
    void addR (string name, string Node1, string Node2, double value);
    void addC (string name, string Node1, string Node2, double value);
    void addI (string name, string Node1, string Node2, double value);
    void addD (string name, string Node1, string Node2, string model);
    void addGND (string name);
    void addVS (string name, string Node1, string Node2, double value);
    void addCS (string name, string Node1, string Node2, double value);
    void addSin (string name, string Node1, string Node2, double offset, double amplitude, double frequency);


    void delElement (Element* element);
};


#endif //PROJECT_PHAZE_ONE_CONTROLLER_H
