#include "Libraries.h"
#include "view.h"
#include "Controller.h"
#include "graphical_view.h"

int main(int argc, char* argv[]) {
    cout << "Select a view to run:" << endl;
    cout << "1: Graphical View " << endl;
    cout << "2: Text-Based View " << endl;
    cout << "Enter choice: ";

    int choice = 0;
    cin >> choice;
    cin.ignore(numeric_limits<std::streamsize>::max(), '\n');

    Controller* C = new Controller;

    try {
        if (choice == 1) {  // graphical view
            graphical_view* gv = new graphical_view;
            gv->run(C);
            delete gv;

        } else if (choice == 2) {   // old view
            View* V = new View;
            bool running = true;
            while (running) {
                running = V->inputHandler(C);
            }
            delete V;

        } else {
            cout << "Invalid choice. Exiting." << endl;
        }
    } catch (const std::exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
    }

    delete C;
    return 0;
}