#include "Libraries.h"
#include "view.h"
#include "Controller.h"
#include "graphical_view.h"
#include <memory>
#include <iostream>
#include <limits>

bool validInput(const std::string& line) {
    return (line == "1" || line == "2" || line == "save" || line == "load" || line == "end");
}

std::string getInput() {
    std::cout << "\nSelect an option:\n"
              << "1: Graphical View\n"
              << "2: Text-Based View\n"
              << "save: Save circuit to JSON\n"
              << "load: Load circuit from JSON\n"
              << "end: Exit\n"
              << "Enter choice: ";
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int main(int argc, char* argv[]) {
    auto* view = new View();
    auto* graphicalView = new graphical_view();
    auto* controller = new Controller();

    bool continueRunning = true;
    std::string input;

    while (continueRunning) {
        try {
            input = getInput();
            if (!validInput(input)) {
                std::cerr << "Invalid input. Please enter 1, 2, save, load, or end.\n";
                continue;
            }

            if (input == "1") {
                continueRunning = graphicalView->run(controller);
            }
            else if (input == "2") {
                continueRunning = view->inputHandler(controller);
            }
            else if (input == "save") {
                std::string filename;
                std::cout << "Enter filename to save (e.g., circuit.json): ";
                std::getline(std::cin, filename);
                if (auto circuit = controller->circuit) {
                    circuit->save_to_file(filename);
                    std::cout << "✅ Circuit saved to " << filename << "\n";
                } else {
                    std::cerr << "❌ No circuit to save.\n";
                }
            }
            else if (input == "load") {
                std::string filename;
                std::cout << "Enter filename to load (e.g., circuit.json): ";
                std::getline(std::cin, filename);
                if (auto circuit = controller->circuit) {
                    circuit->load_from_file(filename);
                    std::cout << "📥 Circuit loaded from " << filename << "\n";
                } else {
                    std::cerr << "❌ No circuit object available to load into.\n";
                }
            }
            else if (input == "end") {
                continueRunning = false;
            }
        }
        catch (const Quit_Event_Exception&) {
            std::cout << "Closing the app!\n";
            continueRunning = false;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    delete view;
    delete graphicalView;
    delete controller;

    return 0;
}
