//
// Created by Arian Sadeghi on 6/7/25.
//

#ifndef PROJECT_PHAZE_ONE_VIEW_H
#define PROJECT_PHAZE_ONE_VIEW_H

#include "Libraries.h"
#include "Errors.h"
#include "Controller.h"
class View {
    bool mainMenu = true;
    bool circuitMenu = false;
    bool fileMenu = false;
    bool analysisMenu = false;

    static vector<string> splitString(const string& input);


public:

    bool inputHandler (Controller* C);
    bool handleMainMenu (Controller* C);
    bool handleAnalysisMenu (Controller* C);
    bool handleFileMenu (Controller* C);
    bool handleCircuitMenu (Controller* C);

};


#endif //PROJECT_PHAZE_ONE_VIEW_H
