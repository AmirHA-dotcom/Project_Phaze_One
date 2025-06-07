//
// Created by Arian Sadeghi on 6/7/25.
//

#include "Libraries.h"
#include "view.h"
int main() {
    View v;
    string line;
    bool cond = true;
    while (cond) {
        try {
            cond = v.inputHandler();
        } catch (const exception& e) {
            cout << e.what() << endl;
        }
    }
    return 0;


}
