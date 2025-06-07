//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_VIEW_H
#define PROJECT_PHAZE_ONE_VIEW_H

#include "Libraries.h"
#include "Errors.h"

class View {


    static vector<string> splitString(const string& input);


public:

    bool inputHandler (string line);

};


#endif //PROJECT_PHAZE_ONE_VIEW_H
