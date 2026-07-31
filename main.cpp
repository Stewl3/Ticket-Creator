#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <limits>
#include <ctime>

using namespace std;

static unordered_map<string, string> getPartsMap() {
    return {
        {"1", "Motherboard"}, {"2", "Keyboard"}, {"3", "Touchscreen"}, {"4", "LCD"},
        {"5", "Top Cover"}, {"6", "Bottom Cover"}, {"7", "Touchpad"}, {"8", "Bezel"},
        {"9", "Hinge Cover"}, {"0", "Cable Kit"}, {"A", "DC Plug"}, {"B", "Audio Board"},
        {"C", "NIC"}, {"D", "Webcam"}, {"E", "Battery"}, {"F", "SSD/HDD"},
        {"G", "Speaker"}, {"H", "Heat Sink"}
    };
}

static void printHeader() {
    cout << "--------------------------------------------" << "\n";
    cout << "-------------- Ticket Creator --------------" << "\n";
    cout << "--------------------------------------------" << "\n" << "\n";
}

static bool promptLine(const string &prompt, string &out) {
    cout << prompt;
    if (!getline(cin, out)) return false;
    return true;
}

static bool isExitCommand(const string &input) {
    string lower = input;
    transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
    return (lower == "exit" || lower == "quit");
}

static bool parseTicketNumber(const string &input, int &ticket_number) {
    try {
        ticket_number = stoi(input);
        return true;
    } catch (...) {
        return false;
    }
}

static bool promptChar(const string &prompt, char &out) {
    cout << prompt;
    if (!(cin >> out)) return false;
    out = toupper(out);
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return true;
}

static string chooseLaptopType(char touchscreenLetter) {
    return (touchscreenLetter == 'Y') ? "N08933-001" : "N08935-001";
}

static vector<string> parseParts(const string &rawInput, const unordered_map<string, string> &partsMap) {
    string s = rawInput;
    for (char &c : s) {
        if (c == ',') c = ' ';
        c = toupper((unsigned char)c);
    }

    vector<string> selectedParts;
    string token;
    stringstream ss(s);
    while (ss >> token) {
        if (token.size() == 1) {
            string key(1, token[0]);
            auto it = partsMap.find(key);
            if (it != partsMap.end()) selectedParts.push_back(it->second);
        } else {
            for (char c : token) {
                string key(1, c);
                auto it = partsMap.find(key);
                if (it != partsMap.end()) selectedParts.push_back(it->second);
            }
        }
    }

    vector<string> uniqueParts;
    for (const auto &p : selectedParts) {
        if (find(uniqueParts.begin(), uniqueParts.end(), p) == uniqueParts.end())
            uniqueParts.push_back(p);
    }
    return uniqueParts;
}

static string joinPartsList(const vector<string> &parts) {
    string parts_list;
    for (size_t i = 0; i < parts.size(); ++i) {
        parts_list += parts[i];
        if (i + 1 < parts.size()) parts_list += ", ";
    }
    return parts_list;
}

static void printPartsMenu() {
    cout << "\nWhat parts are needed?\n"
         << "----------------------\n";

    cout << "1. Motherboard \n"
         << "2. Keyboard \n"
         << "3. Touchscreen \n"
         << "4. LCD \n"
         << "5. Top Cover \n"
         << "6. Bottom Cover \n"
         << "7. Touchpad \n"
         << "8. Bezel \n"
         << "9. Hinge Cover \n"
         << "0. Cable Kit \n"
         << "A. DC Plug \n"
         << "B. Audio Board \n"
         << "C. NIC \n"
         << "D. Webcam \n"
         << "E. Battery \n"
         << "F. SSD/HDD \n"
         << "G. Speaker \n"
         << "H. Heat Sink \n" << "\n";
}

static void printTicket(int ticket_number, const string &serial_number, const string &laptop_type, const string &parts_list) {
    cout << "\n" << laptop_type << "\n"
         << "Service Ticket " << ticket_number << " " << serial_number << " HP Fortis G9 needs - ";

    if (!parts_list.empty()) {
        cout << parts_list << " ";
    } else {
        cout << "(none) ";
    }

    cout << "ADP*" << "\n" << "\n";
}

static bool promptForTicketNumber(int &ticket_number) {
    string ticket_input;
    cout << "Enter Ticket Number: ";
    if (!getline(cin, ticket_input)) return false;

    if (ticket_input.empty()) {
        cout << "Ticket number cannot be empty. Please try again." << "\n" << "\n";
        return false;
    }

    if (!parseTicketNumber(ticket_input, ticket_number)) {
        cout << "Invalid ticket number. Please enter a numeric ticket number." << "\n" << "\n";
        return false;
    }

    return true;
}

static void promptForPartsSelection(const unordered_map<string, string> &partsMap, string &parts_list) {
    string part_selection;
    printPartsMenu();

    cout << "Enter the part(s): ";
    if (!getline(cin, part_selection)) {
        parts_list.clear();
        return;
    }

    vector<string> uniqueParts = parseParts(part_selection, partsMap);
    parts_list = joinPartsList(uniqueParts);
}

static void reviewTicketDetails(int &ticket_number, string &serial_number, const string &laptop_type, string &parts_list, const unordered_map<string, string> &partsMap) {
    char choice;

    while (true) {
        cout << "Final Check" << "\n"
             << "-----------" << "\n"
             << "1. Adjust Ticket Number" << "\n"
             << "2. Adjust Serial Number" << "\n"
             << "3. Adjust Parts Needed" << "\n"
             << "4. Continue" << "\n" << "\n";

        if (!promptChar("Select an option: ", choice)) return;

        switch (choice) {
            case '1': {
                if (!promptForTicketNumber(ticket_number)) {
                    cout << "Ticket number was not changed." << "\n";
                }
                break;
            }
            case '2': {
                cout << "Enter Serial Number: ";
                if (!getline(cin, serial_number)) {
                    serial_number.clear();
                    return;
                }
                transform(serial_number.begin(), serial_number.end(), serial_number.begin(), [](unsigned char c) { return toupper(c); });
                break;
            }
            case '3':
                promptForPartsSelection(partsMap, parts_list);
                break;
            case '4':
                return;
            default:
                cout << "Invalid selection. Please try again." << "\n";
                break;
        }

        printTicket(ticket_number, serial_number, laptop_type, parts_list);
    }
}

int main() {
    int ticket_number;
    string ticket_input;
    string serial_number;
    string laptop_type;
    char letter;
    string part_selection;

    auto partsMap = getPartsMap();

    printHeader();

    while (true) {
        if (!promptLine("Enter Ticket Number (Exit/Quit to end): ", ticket_input)) break;

        if (isExitCommand(ticket_input)) {
            cout << "Exiting ticket creator." << "\n";
            break;
        }

        if (ticket_input.empty()) {
            cout << "Ticket number cannot be empty. Please try again." << "\n" << "\n";
            continue;
        }

        if (!parseTicketNumber(ticket_input, ticket_number)) {
            cout << "Invalid ticket number. Please enter a numeric ticket number or type exit/quit." << "\n" << "\n";
            continue;
        }

        cout << "\n";
        if (!promptLine("Enter Serial Number: ", serial_number)) break;
        cout << "\n";

        transform(serial_number.begin(), serial_number.end(), serial_number.begin(), [](unsigned char c) { return toupper(c); });

        if (!promptChar("Is the Laptop touchscreen? (Y/N): ", letter)) break;

        laptop_type = chooseLaptopType(letter);

        printPartsMenu();

        cout << "Enter the part(s): ";
        if (!getline(cin, part_selection)) break;

        vector<string> uniqueParts = parseParts(part_selection, partsMap);
        string parts_list = joinPartsList(uniqueParts);

        printTicket(ticket_number, serial_number, laptop_type, parts_list);
        reviewTicketDetails(ticket_number, serial_number, laptop_type, parts_list, partsMap);
    }

    return 0;
}

// output to txt file stored in the appropriate folder
// make sure the txt file is named with the proper date