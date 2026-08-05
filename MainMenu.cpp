#include "MainMenu.h"
#include "Helpers.h"
#include "TicketCreator.h"
#include "TicketViewer.h"

#include <chrono>
#include <iostream>
#include <thread>

using namespace std;

void MainMenu::run() {
    while (true) {
        Helpers::clearScreen();
        cout << "----------------------------------------------" << "\n";
        cout << "--------------  Ticket Manager  --------------" << "\n";
        cout << "----------------------------------------------" << "\n\n";

        cout << "                  Main Menu    \n"
             << "             -------------------\n";

        cout << "              1. Ticket Creator\n";
        cout << "              2. Ticket Viewer\n";
        cout << "              3. Exit\n\n";

        char sel;
        if (!Helpers::promptMenuChar("", sel)) {
            continue;
        }
        if (sel == 27) {
            continue;
        }

        switch (sel) {
            case '1': {
                TicketCreator creator;
                creator.run();
                break;
            }
            case '2': {
                TicketViewer viewer;
                viewer.run();
                break;
            }
            case '3': {
                cout << "Goodbye." << "\n";
                this_thread::sleep_for(chrono::milliseconds(500));
                return;
            }
            default: {
                cout << "Invalid selection." << "\n";
                break;
            }
        }

        cout << "\n";
    }
}
