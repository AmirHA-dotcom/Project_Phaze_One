#include "Libraries.h"
#include "view.h"
#include "Controller.h"
#include "graphical_view.h"
#include <memory>
#include <iostream>
#include <limits>

bool validInput (string line){
    if (line == "2" || line == "1" || line == "end") {
        return true;
    }
    return false;
}
string getInput () {
    cout << "Select a view:\n1: Graphical View\n2: Text-Based View\nEnter choice: ";
    string input;
    getline(cin, input);
    return input;
}


int main(int argc, char* argv[]) {
    View* view = new View();
    auto* graphicalView = new graphical_view();
    auto* controller = new Controller();
    bool continueRunning = true;
    bool check = false;
    string input;
    while (continueRunning) {
        try {
            while (!check) {
            input = getInput();
            if (validInput(input))
                check = true;
            else
            cerr << "Invalid input. Please enter 1 or 2.\n";
            }
            if (input == "1") {
                continueRunning = graphicalView->run(controller);
            }
            if (input == "2") {
                continueRunning = view->inputHandler(controller);
            }
            if (input == "end") {
                continueRunning = false;
            }
        }
        catch (const Quit_Event_Exception& e)
        {
            cout << "closing the APP!" << endl;
            continueRunning = false;
        }
        catch (const exception& e)
        {
            cerr << e.what() << endl;
        }
    }

    delete view;
    delete graphicalView;
    delete controller;

    return 0;
}