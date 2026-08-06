#include "Helpers.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include <ctime>
#include <filesystem>
#include <cstdlib>
#include <thread>
#include <chrono>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#endif

using namespace std;

namespace Helpers {

#ifdef _WIN32
    void setCursorVisibility(bool visible) {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO cursorInfo;
        if (GetConsoleCursorInfo(consoleHandle, &cursorInfo)) {
            cursorInfo.bVisible = visible;
            SetConsoleCursorInfo(consoleHandle, &cursorInfo);
        }
    }
#endif

    unordered_map<string, string> getPartsMap() {
        return {
            {"1", "Motherboard"}, {"2", "Keyboard"}, {"3", "Touchscreen"}, {"4", "LCD"},
            {"5", "Top Cover"}, {"6", "Bottom Cover"}, {"7", "Touchpad"}, {"8", "Bezel"},
            {"9", "Hinge Cover"}, {"0", "Cable Kit"}, {"A", "LCD cable kit"}, {"B", "DC Plug"},
            {"C", "Audio Board"}, {"D", "NIC"}, {"E", "Webcam"}, {"F", "Battery"},
            {"G", "SSD/HDD"}, {"H", "Speaker"}, {"I", "Heat Sink"}, {"J", "Screw Kit"}
        };
    }

    bool promptLine(const string &prompt, string &out) {
#ifdef _WIN32
        setCursorVisibility(true);
#endif
        cout << prompt;
        out.clear();

#ifdef _WIN32
        while (true) {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0) {
                _getch();
                continue;
            }
            if (ch == 27) {
                cout << "\n";
                return false;
            }
            if (ch == '\r' || ch == '\n') {
                cout << "\n";
                return true;
            }
            if (ch == 8) {
                if (!out.empty()) {
                    out.pop_back();
                    cout << "\b \b";
                }
                continue;
            }
            if (isprint(ch)) {
                out.push_back(static_cast<char>(ch));
                cout << static_cast<char>(ch);
            }
        }
#else
        if (!getline(cin, out)) return false;
        string lower = out;
        transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return tolower(c); });
        if (lower == "esc") return false;
        return true;
#endif
    }

    bool parseTicketNumber(const string &input, int &ticket_number) {
        try {
            ticket_number = stoi(input);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool promptChar(const string &prompt, char &out) {
#ifdef _WIN32
        setCursorVisibility(true);
#endif
        cout << prompt;

#ifdef _WIN32
        while (true) {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0) {
                _getch();
                continue;
            }
            if (ch == 27) {
                cout << "\n";
                return false;
            }
            if (ch == '\r' || ch == '\n') {
                continue;
            }
            out = toupper(static_cast<unsigned char>(ch));
            cout << out << "\n";
            return true;
        }
#else
        if (!(cin >> out)) return false;
        out = toupper(out);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return true;
#endif
    }

    bool promptMenuChar(const string &prompt, char &out) {
#ifdef _WIN32
        setCursorVisibility(false);
#endif
        cout << prompt;

#ifdef _WIN32
        while (true) {
            int ch = _getch();
            if (ch == 0 || ch == 0xE0) {
                _getch();
                continue;
            }
            if (ch == 27) {
                cout << "\n";
                out = static_cast<char>(27);
                return true;
            }
            if (ch == '\r' || ch == '\n') {
                continue;
            }
            out = toupper(static_cast<unsigned char>(ch));
            cout << out << "\n";
            return true;
        }
#else
        int ch = cin.get();
        if (ch == EOF) return false;
        if (ch == 27) {
            out = static_cast<char>(27);
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return true;
        }
        while (ch == '\n' || ch == '\r') {
            ch = cin.get();
            if (ch == EOF) return false;
        }
        out = toupper(static_cast<unsigned char>(ch));
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return true;
#endif
    }

    void clearScreen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    void printCreatorHeader() {
        cout << "----------------------------------------------" << "\n";
        cout << "--------------- Ticket Creator ---------------" << "\n";
        cout << "----------------------------------------------" << "\n" << "\n";
    }

    void printViewerHeader() {
        cout << "----------------------------------------------" << "\n";
        cout << "--------------- Ticket Viewer  ---------------" << "\n";
        cout << "----------------------------------------------" << "\n" << "\n";
    }

    static void animateSearchDots(const std::string &baseMessage, int cycles = 3, int delayMs = 300) {
        const std::vector<std::string> frames = {".", "..", "..."};
        for (int cycle = 0; cycle < cycles; ++cycle) {
            for (const auto &frame : frames) {
                cout << "\r" << baseMessage << frame << flush;
                std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
            }
        }
    }

    string chooseLaptopType(char touchscreenLetter) {
        return (touchscreenLetter == 'Y') ? "LCD N08933-001" : "LCD N08935-001";
    }

    vector<string> parseParts(const string &rawInput, const unordered_map<string, string> &partsMap) {
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

    string joinPartsList(const vector<string> &parts) {
        string parts_list;
        for (size_t i = 0; i < parts.size(); ++i) {
            parts_list += parts[i];
            if (i + 1 < parts.size()) parts_list += ", ";
        }
        return parts_list;
    }

    string getCurrentDateString() {
        time_t now = time(nullptr);
        tm *localTime = localtime(&now);

        ostringstream dateStream;
        dateStream << (localTime->tm_year + 1900) << "-"
                   << setw(2) << setfill('0') << (localTime->tm_mon + 1) << "-"
                   << setw(2) << setfill('0') << localTime->tm_mday;
        return dateStream.str();
    }

    string getCurrentYearString() {
        time_t now = time(nullptr);
        tm *localTime = localtime(&now);

        ostringstream yearStream;
        yearStream << (localTime->tm_year + 1900);
        return yearStream.str();
    }

    bool isValidDateFormat(const string &date) {
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

    string normalizeDateToFileName(const string &date) {
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

    static string trim(const string &value) {
        const string whitespace = " \t\n\r";
        size_t start = value.find_first_not_of(whitespace);
        size_t end = value.find_last_not_of(whitespace);
        if (start == string::npos || end == string::npos) {
            return "";
        }
        return value.substr(start, end - start + 1);
    }

    static bool parseTicketBlock(const string &block, string &laptop_type, int &ticket_number, string &serial_number, string &parts_list) {
        vector<string> lines;
        string line;
        istringstream in(block);
        while (getline(in, line)) {
            if (!line.empty()) {
                lines.push_back(line);
            }
        }

        if (lines.size() < 2) {
            return false;
        }

        laptop_type = trim(lines[0]);
        const string &ticketLine = lines[1];
        const string prefix = "Service Ticket ";
        size_t pos = ticketLine.find(prefix);
        if (pos == string::npos) {
            return false;
        }

        size_t cursor = pos + prefix.size();
        size_t nextSpace = ticketLine.find(' ', cursor);
        if (nextSpace == string::npos) {
            return false;
        }

        string ticketString = ticketLine.substr(cursor, nextSpace - cursor);
        if (!parseTicketNumber(ticketString, ticket_number)) {
            return false;
        }

        cursor = nextSpace + 1;
        nextSpace = ticketLine.find(' ', cursor);
        if (nextSpace == string::npos) {
            return false;
        }

        serial_number = ticketLine.substr(cursor, nextSpace - cursor);

        const string needsToken = "HP Fortis G9 needs - ";
        size_t needsPos = ticketLine.find(needsToken, nextSpace);
        if (needsPos == string::npos) {
            return false;
        }

        cursor = needsPos + needsToken.size();
        const string suffix = " ADP*";
        size_t suffixPos = ticketLine.rfind(suffix);
        if (suffixPos == string::npos || suffixPos < cursor) {
            return false;
        }

        parts_list = trim(ticketLine.substr(cursor, suffixPos - cursor));
        return true;
    }

    bool manageTicketDocument(const string &filePath) {
        if (!filesystem::exists(filePath)) {
            cout << "No saved tickets found for that date." << "\n";
            return false;
        }

        while (true) {
            clearScreen();
            printViewerHeader();
            vector<string> blocks = readTicketBlocks(filePath);
            cout << "\nSaved Tickets - " << filesystem::path(filePath).filename().string() << "\n"
                 << "------------------------------" << "\n" << "\n";

            if (blocks.empty()) {
                cout << "No tickets are stored in this document." << "\n";
            } else {
                for (size_t i = 0; i < blocks.size(); ++i) {
                    cout << blocks[i] << "\n";
                    if (i + 1 < blocks.size()) {
                        cout << "\n";
                    }
                }
            }

            cout << "\n" << "------------------------------" << "\n";

            cout << "\nDocument Actions" << "\n"
                 << "----------------" << "\n"
                 << "1. Delete ticket by number" << "\n"
                 << "2. Edit ticket by number" << "\n"
                 << "3. Add a new ticket" << "\n"
                 << "4. Refresh document" << "\n"
                 << "5. Return" << "\n" << "\n";

            string action;
            if (!promptLine("Select an action: ", action)) {
                return false;
            }
            transform(action.begin(), action.end(), action.begin(), [](unsigned char c) { return tolower(c); });

            if (action == "1" || action == "delete" || action == "d") {
                string ticketInput;
                if (!promptLine("Enter ticket number to delete: ", ticketInput)) return false;
                int ticketNumber;
                if (!parseTicketNumber(ticketInput, ticketNumber)) {
                    cout << "Invalid ticket number." << "\n";
                    continue;
                }

                size_t index = blocks.size();
                for (size_t i = 0; i < blocks.size(); ++i) {
                    string laptop_type, serial_number, parts_list;
                    int existingNumber;
                    if (parseTicketBlock(blocks[i], laptop_type, existingNumber, serial_number, parts_list) && existingNumber == ticketNumber) {
                        index = i;
                        break;
                    }
                }

                if (index == blocks.size()) {
                    cout << "Ticket not found." << "\n";
                    continue;
                }

                cout << "\nFound ticket:\n" << blocks[index] << "\n";
                string confirm;
                if (!promptLine("Type 'yes' to confirm delete: ", confirm)) return false;
                transform(confirm.begin(), confirm.end(), confirm.begin(), [](unsigned char c) { return tolower(c); });
                if (confirm != "yes") {
                    cout << "Delete canceled." << "\n";
                    continue;
                }

                blocks.erase(blocks.begin() + index);
                if (!writeTicketBlocks(filePath, blocks)) {
                    return false;
                }
                cout << "Ticket deleted." << "\n";
                continue;
            }

            if (action == "2" || action == "edit" || action == "e") {
                string ticketInput;
                if (!promptLine("Enter ticket number to edit: ", ticketInput)) return false;
                int ticketNumber;
                if (!parseTicketNumber(ticketInput, ticketNumber)) {
                    cout << "Invalid ticket number." << "\n";
                    continue;
                }

                size_t index = blocks.size();
                string existingLaptopType, existingSerial, existingParts;
                int existingNumber = 0;
                for (size_t i = 0; i < blocks.size(); ++i) {
                    if (parseTicketBlock(blocks[i], existingLaptopType, existingNumber, existingSerial, existingParts) && existingNumber == ticketNumber) {
                        index = i;
                        break;
                    }
                }

                if (index == blocks.size()) {
                    cout << "Ticket not found." << "\n";
                    continue;
                }


                int newTicketNumber = existingNumber;
                string newSerial = existingSerial;
                string newLaptopType = existingLaptopType;
                string newPartsList = existingParts;

                while (true) {
                    string draftBlock = buildTicketBlock(newLaptopType, newTicketNumber, newSerial, newPartsList);
                    cout << "\nCurrent Ticket" << "\n"
                         << "----------------" << "\n"
                         << draftBlock << "\n\n";
                    cout << "1. Adjust Ticket Number" << "\n"
                         << "2. Adjust Serial Number" << "\n"
                         << "3. Adjust LCD Type" << "\n"
                         << "4. Adjust Parts Needed" << "\n"
                         << "5. Save Edited Ticket" << "\n"
                         << "6. Cancel" << "\n" << "\n";

                    string reviewChoice;
                    if (!promptLine("Select an option: ", reviewChoice)) return false;
                    transform(reviewChoice.begin(), reviewChoice.end(), reviewChoice.begin(), [](unsigned char c) { return tolower(c); });

                    if (reviewChoice == "1") {
                        if (!promptForTicketNumber(newTicketNumber)) {
                            cout << "Ticket number was not changed." << "\n";
                        }
                        continue;
                    }

                    if (reviewChoice == "2") {
                        string updatedSerial;
                        if (!promptLine("Enter Serial Number: ", updatedSerial)) {
                            cout << "Serial number was not changed." << "\n";
                        } else {
                            transform(updatedSerial.begin(), updatedSerial.end(), updatedSerial.begin(), [](unsigned char c) { return toupper(c); });
                            newSerial = updatedSerial;
                        }
                        continue;
                    }

                    if (reviewChoice == "3") {
                        char currentTouchscreen = (newLaptopType == "LCD N08933-001") ? 'Y' : 'N';
                        string touchscreenInput;
                        if (!promptLine("Is the Laptop touchscreen? (Y/N): ", touchscreenInput)) {
                            cout << "LCD type was not changed." << "\n";
                        } else {
                            char touchscreenChoice = touchscreenInput.empty() ? currentTouchscreen : toupper(static_cast<unsigned char>(touchscreenInput[0]));
                            newLaptopType = chooseLaptopType(touchscreenChoice);
                        }
                        continue;
                    }

                    if (reviewChoice == "4") {
                        cout << "\nCurrent parts: " << (newPartsList.empty() ? "(none)" : newPartsList) << "\n";
                        printPartsMenu();
                        string partSelection;
                        if (!promptLine("Enter the part(s) (leave blank to keep current): ", partSelection)) {
                            cout << "Parts were not changed." << "\n";
                        } else if (!partSelection.empty()) {
                            vector<string> newParts = parseParts(partSelection, getPartsMap());
                            newPartsList = joinPartsList(newParts);
                        }
                        continue;
                    }

                    if (reviewChoice == "5") {
                        blocks[index] = buildTicketBlock(newLaptopType, newTicketNumber, newSerial, newPartsList);
                        stable_sort(blocks.begin(), blocks.end(), [&](const string &a, const string &b) {
                            const string firstLineA = a.substr(0, a.find('\n'));
                            const string firstLineB = b.substr(0, b.find('\n'));
                            return getLaptopTypeOrder(firstLineA) < getLaptopTypeOrder(firstLineB);
                        });

                        if (!writeTicketBlocks(filePath, blocks)) {
                            return false;
                        }
                        cout << "Ticket updated successfully." << "\n";
                        string returnInput;
                        if (!promptLine("Press Enter to return to the document... ", returnInput)) {
                            return false;
                        }
                        break;
                    }

                    if (reviewChoice == "6" || reviewChoice == "cancel" || reviewChoice == "c") {
                        cout << "Edit canceled." << "\n";
                        break;
                    }

                    cout << "Invalid selection. Please try again." << "\n";
                }
                continue;
            }

            if (action == "3" || action == "add" || action == "a") {
                string ticketInput;
                if (!promptLine("Enter Ticket Number: ", ticketInput)) return false;
                int ticketNumber;
                if (!parseTicketNumber(ticketInput, ticketNumber)) {
                    cout << "Invalid ticket number." << "\n";
                    continue;
                }

                string serialNumber;
                if (!promptLine("Enter Serial Number: ", serialNumber)) return false;
                transform(serialNumber.begin(), serialNumber.end(), serialNumber.begin(), [](unsigned char c) { return toupper(c); });

                char letter;
                if (!promptChar("Is the Laptop touchscreen? (Y/N): ", letter)) return false;
                string laptopType = chooseLaptopType(letter);

                printPartsMenu();
                string partSelection;
                if (!promptLine("Enter the part(s): ", partSelection)) return false;
                vector<string> uniqueParts = parseParts(partSelection, getPartsMap());
                string partsList = joinPartsList(uniqueParts);

                while (true) {
                    string newBlock = buildTicketBlock(laptopType, ticketNumber, serialNumber, partsList);
                    cout << "\nReview New Ticket" << "\n"
                         << "-----------------" << "\n"
                         << newBlock << "\n\n";
                    cout << "1. Adjust Ticket Number" << "\n"
                         << "2. Adjust Serial Number" << "\n"
                         << "3. Adjust LCD Type" << "\n"
                         << "4. Adjust Parts Needed" << "\n"
                         << "5. Save Ticket" << "\n"
                         << "6. Cancel" << "\n" << "\n";

                    string reviewChoice;
                    if (!promptLine("Select an option: ", reviewChoice)) return false;
                    transform(reviewChoice.begin(), reviewChoice.end(), reviewChoice.begin(), [](unsigned char c) { return tolower(c); });

                    if (reviewChoice == "1") {
                        if (!promptForTicketNumber(ticketNumber)) {
                            cout << "Ticket number was not changed." << "\n";
                        }
                        continue;
                    }

                    if (reviewChoice == "2") {
                        string newSerial;
                        if (!promptLine("Enter Serial Number: ", newSerial)) {
                            cout << "Serial number was not changed." << "\n";
                        } else {
                            transform(newSerial.begin(), newSerial.end(), newSerial.begin(), [](unsigned char c) { return toupper(c); });
                            serialNumber = newSerial;
                        }
                        continue;
                    }

                    if (reviewChoice == "3") {
                        char newLetter;
                        if (!promptChar("Is the Laptop touchscreen? (Y/N): ", newLetter)) {
                            cout << "LCD type was not changed." << "\n";
                        } else {
                            laptopType = chooseLaptopType(newLetter);
                        }
                        continue;
                    }

                    if (reviewChoice == "4") {
                        printPartsMenu();
                        string newPartSelection;
                        if (!promptLine("Enter the part(s): ", newPartSelection)) {
                            cout << "Parts were not changed." << "\n";
                        } else {
                            vector<string> newParts = parseParts(newPartSelection, getPartsMap());
                            partsList = joinPartsList(newParts);
                        }
                        continue;
                    }

                    if (reviewChoice == "5") {
                        blocks.push_back(newBlock);
                        stable_sort(blocks.begin(), blocks.end(), [&](const string &a, const string &b) {
                            const string firstLineA = a.substr(0, a.find('\n'));
                            const string firstLineB = b.substr(0, b.find('\n'));
                            return getLaptopTypeOrder(firstLineA) < getLaptopTypeOrder(firstLineB);
                        });

                        if (!writeTicketBlocks(filePath, blocks)) {
                            return false;
                        }
                        cout << "Ticket added." << "\n";
                        break;
                    }

                    if (reviewChoice == "6" || reviewChoice == "cancel" || reviewChoice == "c") {
                        cout << "Add ticket canceled." << "\n";
                        break;
                    }

                    cout << "Invalid selection. Please try again." << "\n";
                }
                continue;
            }

            if (action == "4" || action == "refresh" || action == "r") {
                continue;
            }

            if (action == "5" || action == "return" || action == "q") {
                return true;
            }

            cout << "Invalid selection. Please try again." << "\n";
        }
    }

    vector<string> getSortedTicketFiles(const string &baseFolder) {
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

    vector<string> readTicketBlocks(const string &fileName) {
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

    bool writeTicketBlocks(const string &fileName, const vector<string> &blocks) {
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

    int getLaptopTypeOrder(const string &laptop_type) {
        if (laptop_type == "N08933-001") return 0;
        if (laptop_type == "N08935-001") return 1;
        return 2;
    }

    string buildTicketBlock(const string &laptop_type, int ticket_number, const string &serial_number, const string &parts_list) {
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

    bool saveTicketToFile(const string &laptop_type, int ticket_number, const string &serial_number, const string &parts_list) {
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

    void printPartsMenu() {
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

    void printTicket(int ticket_number, const string &serial_number, const string &laptop_type, const string &parts_list) {
        cout << "\n" << laptop_type << "\n"
             << "Service Ticket " << ticket_number << " " << serial_number << " HP Fortis G9 needs - ";

        if (!parts_list.empty()) {
            cout << parts_list << " ";
        } else {
            cout << "(none) ";
        }

        cout << "ADP*" << "\n" << "\n";
    }

    bool promptForTicketNumber(int &ticket_number) {
        string ticket_input;
        if (!promptLine("Enter Ticket Number: ", ticket_input)) return false;

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

    void searchTicketByNumberAcrossDocuments(const string &baseFolder, int ticketNumber) {
        vector<string> ticketFiles = getSortedTicketFiles(baseFolder);
        int matchCount = 0;

        if (ticketFiles.empty()) {
            cout << "No saved tickets found yet." << "\n";
            return;
        }

        const string searchMessage = "\nSearching all documents for ticket number " + to_string(ticketNumber);
        animateSearchDots(searchMessage, 2, 150);

        for (const auto &filePath : ticketFiles) {
            vector<string> blocks = readTicketBlocks(filePath);
            for (const auto &block : blocks) {
                string laptop_type, serial_number, parts_list;
                int existingNumber = 0;
                if (parseTicketBlock(block, laptop_type, existingNumber, serial_number, parts_list) && existingNumber == ticketNumber) {
                    if (matchCount == 0) {
                        cout << "\n\nSearch Results:" << "\n"
                             << "---------------" << "\n";
                             std::this_thread::sleep_for(std::chrono::milliseconds(300));
                            }

                    matchCount++;
                    cout << "(" << matchCount << ") " << "Found in "; 
                    cout << filesystem::path(filePath).filename().string() << ":\n\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(300));
                    cout << block << "\n\n";
                }
            }
        }

        if (matchCount == 0) {
            cout << "\n\nNo ticket found with number " << ticketNumber << "." << "\n\n";
        }
    }
}
