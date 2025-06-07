//
// Created by Arian Sadeghi on 6/7/25.
//

#include "Libraries.h"
#include "view.h"
#include "Controller.h"
int main() {
    View* V = new View;
    Controller* C = new Controller;
    string line;
    bool cond = true;
    while (cond) {
        try {
            cond = V->inputHandler(C);
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return 0;


}
