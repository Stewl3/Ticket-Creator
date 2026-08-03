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
#include <filesystem>

using namespace std;

static unordered_map<string, string> getPartsMap() {
    return {
        {"1", "Motherboard"}, {"2", "Keyboard"}, {"3", "Touchscreen"}, {"4", "LCD"},
        {"5", "Top Cover"}, {"6", "Bottom Cover"}, {"7", "Touchpad"}, {"8", "Bezel"},
        {"9", "Hinge Cover"}, {"0", "Cable Kit"}, {"A", "LCD cable kit"}, {"B", "DC Plug"},
        {"C", "Audio Board"}, {"D", "NIC"}, {"E", "Webcam"}, {"F", "Battery"},
        {"G", "SSD/HDD"}, {"H", "Speaker"}, {"I", "Heat Sink"}, {"J", "Screw Kit"}
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

static string getCurrentDateString() {
    time_t now = time(nullptr);
    tm *localTime = localtime(&now);

    ostringstream dateStream;
    dateStream << (localTime->tm_year + 1900) << "-"
               << setw(2) << setfill('0') << (localTime->tm_mon + 1) << "-"
               << setw(2) << setfill('0') << localTime->tm_mday;
    return dateStream.str();
}

static string getCurrentYearString() {
    time_t now = time(nullptr);
    tm *localTime = localtime(&now);

    ostringstream yearStream;
    yearStream << (localTime->tm_year + 1900);
    return yearStream.str();
}

static bool isValidDateFormat(const string &date) {
    if (date.empty()) return false;

    string normalized = date;
    replace(normalized.begin(), normalized.end(), '/', '-');

    if (normalized.size() == 5) {
        if (normalized[2] != '-') return false;
        for (size_t i = 0; i < normalized.size(); ++i) {
            if (i == 2) continue;
            if (!isdigit(static_cast<unsigned char>(normalized[i]))) return false;
        }
        return true;
    }

    if (normalized.size() == 10) {
        if (normalized[2] != '-' || normalized[5] != '-') return false;
        for (size_t i = 0; i < normalized.size(); ++i) {
            if (i == 2 || i == 5) continue;
            if (!isdigit(static_cast<unsigned char>(normalized[i]))) return false;
        }
        return true;
    }

    return false;
}

static string normalizeDateToFileName(const string &date) {
    string normalized = date;
    replace(normalized.begin(), normalized.end(), '/', '-');

    if (normalized.size() == 5) {
        return getCurrentYearString() + "-" + normalized.substr(0, 2) + "-" + normalized.substr(3, 2) + ".txt";
    }

    if (normalized.size() == 10) {
        return normalized.substr(6, 4) + "-" + normalized.substr(0, 2) + "-" + normalized.substr(3, 2) + ".txt";
    }

    return normalized + ".txt";
}

static void printTicketFileByPath(const string &filePath) {
    if (!filesystem::exists(filePath)) {
        cout << "No saved tickets found for that date." << "\n";
        return;
    }

    ifstream in(filePath);
    if (!in.is_open()) {
        cout << "Unable to open saved tickets for that date." << "\n";
        return;
    }

    cout << "\nSaved Tickets - " << filesystem::path(filePath).filename().string() << "\n"
         << "------------------------------" << "\n";

    string line;
    while (getline(in, line)) {
        cout << line << "\n";
    }
    cout << "\n";
}

static vector<string> getSortedTicketFiles(const string &baseFolder) {
    vector<string> files;

    if (!filesystem::exists(baseFolder)) {
        return files;
    }

    for (const auto &entry : filesystem::directory_iterator(baseFolder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            files.push_back(entry.path().string());
        }
    }

    sort(files.begin(), files.end(), [](const string &a, const string &b) {
        return filesystem::path(a).filename().string() < filesystem::path(b).filename().string();
    });

    return files;
}

static void viewAllSavedTickets(const string &baseFolder) {
    vector<string> ticketFiles = getSortedTicketFiles(baseFolder);

    if (ticketFiles.empty()) {
        cout << "No saved tickets found yet." << "\n";
        return;
    }

    cout << "\nSaved Ticket Files" << "\n"
         << "------------------" << "\n";

    for (size_t i = 0; i < ticketFiles.size(); ++i) {
        cout << (i + 1) << ". " << filesystem::path(ticketFiles[i]).filename().string() << "\n";
    }

    const int cancelChoice = static_cast<int>(ticketFiles.size()) + 1;
    cout << cancelChoice << ". Cancel" << "\n" << "\n";

    while (true) {
        string choiceInput;
        if (!promptLine("Select a file to open: ", choiceInput)) return;

        if (choiceInput.empty()) {
            cout << "Please enter a selection." << "\n";
            continue;
        }

        transform(choiceInput.begin(), choiceInput.end(), choiceInput.begin(), [](unsigned char c) { return tolower(c); });

        if (choiceInput == "cancel" || choiceInput == "c") {
            cout << "View canceled." << "\n";
            return;
        }

        try {
            int choice = stoi(choiceInput);
            if (choice == cancelChoice) {
                cout << "View canceled." << "\n";
                return;
            }
            if (choice >= 1 && choice <= static_cast<int>(ticketFiles.size())) {
                printTicketFileByPath(ticketFiles[choice - 1]);
                return;
            }
        } catch (...) {
        }

        cout << "Invalid selection. Please try again." << "\n";
    }
}

static int getLaptopTypeOrder(const string &laptop_type) {
    if (laptop_type == "N08933-001") return 0;
    if (laptop_type == "N08935-001") return 1;
    return 2;
}

static string buildTicketBlock(const string &laptop_type, int ticket_number, const string &serial_number, const string &parts_list) {
    ostringstream block;
    block << laptop_type << "\n";
    block << "Service Ticket " << ticket_number << " " << serial_number << " HP Fortis G9 needs - ";

    if (!parts_list.empty()) {
        block << parts_list << " ";
    } else {
        block << "(none) ";
    }

    block << "ADP*";
    return block.str();
}

static vector<string> readTicketBlocks(const string &fileName) {
    vector<string> blocks;
    ifstream in(fileName);

    if (!in.is_open()) {
        return blocks;
    }

    string line;
    ostringstream currentBlock;

    while (getline(in, line)) {
        if (line.empty()) {
            if (!currentBlock.str().empty()) {
                blocks.push_back(currentBlock.str());
                currentBlock.str("");
                currentBlock.clear();
            }
        } else {
            if (!currentBlock.str().empty()) {
                currentBlock << "\n";
            }
            currentBlock << line;
        }
    }

    if (!currentBlock.str().empty()) {
        blocks.push_back(currentBlock.str());
    }

    return blocks;
}

static bool writeTicketBlocks(const string &fileName, const vector<string> &blocks) {
    ofstream out(fileName, ios::trunc);

    if (!out.is_open()) {
        cout << "Unable to save ticket to " << fileName << "." << "\n";
        return false;
    }

    for (size_t i = 0; i < blocks.size(); ++i) {
        if (i > 0) {
            out << "\n\n";
        }
        out << blocks[i];
    }

    out.close();
    return true;
}

static bool saveTicketToFile(const string &laptop_type, int ticket_number, const string &serial_number, const string &parts_list) {
    string folderName = "output/Tickets";
    string fileName = folderName + "/" + getCurrentDateString() + ".txt";

    if (!filesystem::exists(folderName)) {
        filesystem::create_directories(folderName);
    }

    vector<string> blocks = readTicketBlocks(fileName);
    blocks.push_back(buildTicketBlock(laptop_type, ticket_number, serial_number, parts_list));

    stable_sort(blocks.begin(), blocks.end(), [&](const string &a, const string &b) {
        const string firstLineA = a.substr(0, a.find('\n'));
        const string firstLineB = b.substr(0, b.find('\n'));
        return getLaptopTypeOrder(firstLineA) < getLaptopTypeOrder(firstLineB);
    });

    if (!writeTicketBlocks(fileName, blocks)) {
        return false;
    }

    cout << "\nTicket saved to " << fileName << "\n";
    return true;
}

static void viewSavedTickets() {
    const string baseFolder = "output/Tickets";

    if (!filesystem::exists(baseFolder)) {
        cout << "No saved tickets found yet." << "\n";
        return;
    }

    cout << "\nView Saved Tickets" << "\n"
         << "------------------" << "\n"
         << "1. View today's tickets" << "\n"
         << "2. Search by date" << "\n"
         << "3. View all saved tickets" << "\n"
         << "4. Cancel" << "\n" << "\n";

    string choiceInput;
    if (!promptLine("Select an option: ", choiceInput)) return;
    transform(choiceInput.begin(), choiceInput.end(), choiceInput.begin(), [](unsigned char c) { return tolower(c); });

    if (choiceInput == "1" || choiceInput == "today" || choiceInput == "t") {
        printTicketFileByPath(baseFolder + "/" + getCurrentDateString() + ".txt");
        return;
    }

    if (choiceInput == "2" || choiceInput == "date" || choiceInput == "d" || choiceInput == "search") {
        string dateInput;
        while (true) {
            cout << "Enter date (MM-DD or MM-DD-YYYY): ";
            if (!getline(cin, dateInput)) return;

            if (dateInput.empty()) {
                cout << "Date cannot be empty." << "\n";
                continue;
            }

            if (!isValidDateFormat(dateInput)) {
                cout << "Invalid date format. Please use MM-DD or MM-DD-YYYY." << "\n";
                continue;
            }

            string fileName = normalizeDateToFileName(dateInput);
            printTicketFileByPath(baseFolder + "/" + fileName);
            return;
        }
    }

    if (choiceInput == "3" || choiceInput == "all" || choiceInput == "a") {
        viewAllSavedTickets(baseFolder);
        return;
    }

    cout << "View canceled." << "\n";
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
         << "A. LCD cable kit \n"
         << "B. DC Plug \n"
         << "C. Audio Board \n"
         << "D. NIC \n"
         << "E. Webcam \n"
         << "F. Battery \n"
         << "G. SSD/HDD \n"
         << "H. Speaker \n"
         << "I. Heat Sink \n"
         << "J. Screw Kit \n" << "\n";
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

static void reviewTicketDetails(int &ticket_number, string &serial_number, string &laptop_type, string &parts_list, const unordered_map<string, string> &partsMap) {
    char choice;
    char letter;

    while (true) {
        cout << "Final Check" << "\n"
             << "-----------" << "\n"
             << "1. Adjust Ticket Number" << "\n"
             << "2. Adjust Serial Number" << "\n"
             << "3. Adjust LCD Type" << "\n"
             << "4. Adjust Parts Needed" << "\n"
             << "5. Save Ticket To File" << "\n"
             << "6. View Saved Tickets" << "\n" << "\n";

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
            case '3': {
                if (!promptChar("Is the Laptop touchscreen? (Y/N): ", letter)) {
                    cout << "LCD type was not changed." << "\n";
                    break;
                }
                laptop_type = chooseLaptopType(letter);
                break;
            }
            case '4':
                promptForPartsSelection(partsMap, parts_list);
                break;
            case '5':
                saveTicketToFile(laptop_type, ticket_number, serial_number, parts_list);
                return;
            case '6':
                viewSavedTickets();
                break;
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
        if (!promptLine("\nEnter Ticket Number ('View' to see saved tickets): ", ticket_input)) break;

        if (isExitCommand(ticket_input)) {
            cout << "Exiting ticket creator." << "\n";
            break;
        }

        if (ticket_input.empty()) {
            cout << "Ticket number cannot be empty. Please try again." << "\n" << "\n";
            continue;
        }

        if (ticket_input == "view" || ticket_input == "View" || ticket_input == "VIEW" || ticket_input == "show") {
            viewSavedTickets();
            cout << "\n";
            continue;
        }

        if (!parseTicketNumber(ticket_input, ticket_number)) {
            cout << "Invalid ticket number. Please enter a numeric ticket number or type 'View' To view tickets." << "\n" << "\n";
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
