#include "TicketViewer.h"
#include "Helpers.h"

#include <iostream>
#include <string>
#include <algorithm>
#include <filesystem>

using namespace std;

void TicketViewer::run() {
    const string baseFolder = "output/Tickets";

    while (true) {
        Helpers::clearScreen();
        Helpers::printViewerHeader();

        if (!std::filesystem::exists(baseFolder)) {
            cout << "No saved tickets found yet." << "\n";
            return;
        }

        cout << "1. View today's tickets" << "\n"
             << "2. Search by date" << "\n"
             << "3. View all saved tickets" << "\n"
             << "4. Search by ticket number" << "\n\n";

        char sel;
        if (!Helpers::promptMenuChar("Select an option ", sel)) {
            // input error — re-display viewer menu
            continue;
        }
        if (sel == 27) {
            // ESC pressed — return to main menu
            return;
        }
        string choiceInput;
        switch (sel) {
            case '1': choiceInput = "1"; break;
            case '2': choiceInput = "2"; break;
            case '3': choiceInput = "3"; break;
            case '4': choiceInput = "4"; break;
            default:
                cout << "Invalid selection. Please try again." << "\n";
                continue;
        }

        if (choiceInput == "1" || choiceInput == "today" || choiceInput == "t") {
            if (!Helpers::manageTicketDocument(baseFolder + "/" + Helpers::getCurrentDateString() + ".txt")) {
                return;
            }
            continue;
        }

        if (choiceInput == "2" || choiceInput == "date" || choiceInput == "d" || choiceInput == "search") {
            Helpers::clearScreen();
            Helpers::printViewerHeader();
            string dateInput;

            cout << "Search by Date\n";
            cout << "--------------\n" << "\n";

            while (true) {
                if (!Helpers::promptLine("Enter date (MM-DD or MM-DD-YYYY): ", dateInput)) break;

                if (dateInput.empty()) {
                    cout << "Date cannot be empty." << "\n";
                    continue;
                }

                if (!Helpers::isValidDateFormat(dateInput)) {
                    cout << "Invalid date format. Please use MM-DD or MM-DD-YYYY." << "\n";
                    continue;
                }

                string fileName = Helpers::normalizeDateToFileName(dateInput);
                Helpers::manageTicketDocument(baseFolder + "/" + fileName);
                break;
            }
            continue;
        }

        if (choiceInput == "3" || choiceInput == "all" || choiceInput == "a") {

            Helpers::clearScreen();
            Helpers::printViewerHeader();

            vector<string> ticketFiles = Helpers::getSortedTicketFiles(baseFolder);

            if (ticketFiles.empty()) {
                cout << "No saved tickets found yet." << "\n";
                continue;
            }

            cout << "\nSaved Ticket Files" << "\n"
                 << "------------------" << "\n";

            for (size_t i = 0; i < ticketFiles.size(); ++i) {
                cout << (i + 1) << ". " << std::filesystem::path(ticketFiles[i]).filename().string() << "\n";
            }

            const int cancelChoice = static_cast<int>(ticketFiles.size()) + 1;
            cout << cancelChoice << ". Cancel" << "\n" << "\n";

            while (true) {
                string choice;
                if (!Helpers::promptLine("Select a file to open: ", choice)) return;

                if (choice.empty()) {
                    cout << "Please enter a selection." << "\n";
                    continue;
                }

                transform(choice.begin(), choice.end(), choice.begin(), [](unsigned char c) { return tolower(c); });

                if (choice == "cancel" || choice == "c") {
                    cout << "View canceled." << "\n";
                    break;
                }

                try {
                    int idx = stoi(choice);
                    if (idx == cancelChoice) {
                        cout << "View canceled." << "\n";
                        break;
                    }
                    if (idx >= 1 && idx <= static_cast<int>(ticketFiles.size())) {
                        if (!Helpers::manageTicketDocument(ticketFiles[idx - 1])) {
                            return;
                        }
                        break;
                    }
                } catch (...) {}

                cout << "Invalid selection. Please try again." << "\n";
            }
            continue;
        }

        if (choiceInput == "4" || choiceInput == "search" || choiceInput == "s") {

            Helpers::clearScreen();
            Helpers::printViewerHeader();

            cout << "Ticket Search\n";
            cout << "-------------\n" << "\n";
            
            while (true) {
                string ticketInput;
                if (!Helpers::promptLine("Enter ticket number to search (or type 'back' to return): ", ticketInput)) return;

                string lowerInput = ticketInput;
                transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), [](unsigned char c) { return tolower(c); });
                if (lowerInput == "back" || lowerInput == "return" || lowerInput == "b") {
                    break;
                }

                int ticketNumber;
                if (!Helpers::parseTicketNumber(ticketInput, ticketNumber)) {
                    cout << "Invalid ticket number." << "\n";
                    continue;
                }

                Helpers::searchTicketByNumberAcrossDocuments(baseFolder, ticketNumber);
            }
            continue;
        }

        cout << "Invalid selection. Please try again." << "\n";
    }
}
