#include "TicketCreator.h"
#include "Helpers.h"
#include "TicketViewer.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>

using namespace std;

void TicketCreator::run() {
    int ticket_number;
    string ticket_input;
    string serial_number;
    string laptop_type;
    char letter;
    string part_selection;

    auto partsMap = Helpers::getPartsMap();

    while (true) {
        Helpers::clearScreen();
        Helpers::printCreatorHeader();

        if (!Helpers::promptLine("\nEnter Ticket Number ('View' for saved tickets or 'Esc' for Main Menu): ", ticket_input)) break;

        if (Helpers::isExitCommand(ticket_input)) {
            cout << "Exiting ticket creator." << "\n";
            break;
        }

        if (ticket_input.empty()) {
            cout << "Ticket number cannot be empty. Please try again." << "\n" << "\n";
            continue;
        }

        string lower = ticket_input;
        transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
        if (lower == "view" || lower == "show") {
            if (!Helpers::manageTicketDocument("output/Tickets/" + Helpers::getCurrentDateString() + ".txt")) {
                return;
            }
            cout << "\n";
            continue;
        }

        if (!Helpers::parseTicketNumber(ticket_input, ticket_number)) {
            cout << "Invalid ticket number. Please enter a numeric ticket number or type 'View' To view tickets." << "\n" << "\n";
            continue;
        }

        cout << "\n";
        if (!Helpers::promptLine("Enter Serial Number: ", serial_number)) break;
        cout << "\n";

        transform(serial_number.begin(), serial_number.end(), serial_number.begin(), [](unsigned char c) { return toupper(c); });

        string touchscreenInput;
        if (!Helpers::promptLine("Is the Laptop touchscreen? (Y/N): ", touchscreenInput)) break;
        if (!touchscreenInput.empty()) {
            letter = toupper(static_cast<unsigned char>(touchscreenInput[0]));
        } else {
            letter = 'N';
        }

        laptop_type = Helpers::chooseLaptopType(letter);

        Helpers::printPartsMenu();

        if (!Helpers::promptLine("Enter the part(s): ", part_selection)) break;

        vector<string> uniqueParts = Helpers::parseParts(part_selection, partsMap);
        string parts_list = Helpers::joinPartsList(uniqueParts);

        Helpers::printTicket(ticket_number, serial_number, laptop_type, parts_list);

        // Review loop
        while (true) {
            cout << "Final Check" << "\n"
                 << "-----------" << "\n"
                 << "1. Adjust Ticket Number" << "\n"
                 << "2. Adjust Serial Number" << "\n"
                 << "3. Adjust LCD Type" << "\n"
                 << "4. Adjust Parts Needed" << "\n"
                 << "5. Save Ticket To File" << "\n"
                 << "6. Do Not Save Ticket" << "\n" << "\n";

            string choiceInput;
            if (!Helpers::promptLine("Select an option: ", choiceInput)) return;
            if (choiceInput.empty()) {
                cout << "Invalid selection. Please try again." << "\n";
                continue;
            }
            char choice = toupper(static_cast<unsigned char>(choiceInput[0]));

            switch (choice) {
                case '1': {
                    if (!Helpers::promptForTicketNumber(ticket_number)) {
                        cout << "Ticket number was not changed." << "\n";
                    }
                    break;
                }
                case '2': {
                    if (!Helpers::promptLine("Enter Serial Number: ", serial_number)) {
                        serial_number.clear();
                        return;
                    }
                    transform(serial_number.begin(), serial_number.end(), serial_number.begin(), [](unsigned char c) { return toupper(c); });
                    break;
                }
                case '3': {
                    string touchscreenInput;
                    if (!Helpers::promptLine("Is the Laptop touchscreen? (Y/N): ", touchscreenInput)) {
                        cout << "LCD type was not changed." << "\n";
                        break;
                    }
                    if (!touchscreenInput.empty()) {
                        letter = toupper(static_cast<unsigned char>(touchscreenInput[0]));
                    }
                    laptop_type = Helpers::chooseLaptopType(letter);
                    break;
                }
                case '4': {
                    string new_selection;
                    cout << "\n" << "Current parts: " << (parts_list.empty() ? "(none)" : parts_list) << "\n";
                    Helpers::printPartsMenu();
                    if (!Helpers::promptLine("Enter the part(s) (leave blank to set to (none)): ", new_selection)) break;
                    if (new_selection.empty()) {
                        // set to none
                        parts_list.clear();
                    } else {
                        vector<string> newParts = Helpers::parseParts(new_selection, partsMap);
                        parts_list = Helpers::joinPartsList(newParts);
                    }
                    break;
                }
                case '5':
                    Helpers::saveTicketToFile(laptop_type, ticket_number, serial_number, parts_list);
                    goto next_ticket;
                case '6': {
                    cout << "Ticket will not be saved." << "\n";
                    goto next_ticket;
                }
                default:
                    cout << "Invalid selection. Please try again." << "\n";
                    break;
            }

            Helpers::printTicket(ticket_number, serial_number, laptop_type, parts_list);
        }

        next_ticket: ;
    }
}
