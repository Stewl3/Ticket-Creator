#include "ExtraPartsTracker.h"
#include "Helpers.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <thread>

using namespace std;

void ExtraPartsTracker::run() {
    loadParts();

    while (true) {
        if (!displayMenu()) {
            break;  // ESC was pressed, return to main menu
        }
    }
}

bool ExtraPartsTracker::displayMenu() {
    Helpers::clearScreen();
    cout << "----------------------------------------------" << "\n";
    cout << "----------  Extra Parts Tracker  -----------" << "\n";
    cout << "----------------------------------------------" << "\n\n";

    cout << "                   Menu    \n"
         << "             -------------------\n";

    cout << "              1. Add Parts\n";
    cout << "              2. Edit Parts\n";
    cout << "              3. Remove Parts\n";
    cout << "              4. View Extra Parts\n";

    char sel;
    if (!Helpers::promptMenuChar("", sel)) {
        this_thread::sleep_for(chrono::milliseconds(500));
        return true;  // Continue the menu
    }
    if (sel == 27) {
        return false;  // ESC pressed, exit to main menu
    }

    switch (sel) {
        case '1': {
            addParts();
            break;
        }
        case '2': {
            editParts();
            break;
        }
        case '3': {
            removeParts();
            break;
        }
        case '4': {
            viewExtraParts();
            break;
        }
        default: {
            cout << "Invalid selection." << "\n";
            this_thread::sleep_for(chrono::milliseconds(500));
            break;
        }
    }

    cout << "\n";
    return true;  // Continue the menu
}

void ExtraPartsTracker::loadParts() {
    parts.clear();

    if (!filesystem::exists(partsFilePath)) {
        return;
    }

    ifstream file(partsFilePath);
    string line;

    while (getline(file, line)) {
        if (line.empty()) continue;

        size_t colonPos = line.find(':');
        if (colonPos == string::npos) continue;

        string partName = line.substr(0, colonPos);
        string quantityStr = line.substr(colonPos + 1);

        try {
            int quantity = stoi(quantityStr);
            parts[partName] = quantity;
        } catch (...) {
            continue;
        }
    }

    file.close();
}

void ExtraPartsTracker::saveParts() {
    // Create output directory if it doesn't exist
    if (!filesystem::exists("output")) {
        filesystem::create_directories("output");
    }

    ofstream file(partsFilePath);

    for (const auto &entry : parts) {
        file << entry.first << ":" << entry.second << "\n";
    }

    file.close();
}

void ExtraPartsTracker::addParts() {
    Helpers::clearScreen();
    cout << "----------------------------------------------" << "\n";
    cout << "----------  Extra Parts Tracker  -----------" << "\n";
    cout << "----------------------------------------------" << "\n\n";
    cout << "Add Parts\n"
         << "---------\n\n";

    auto partsMap = Helpers::getPartsMap();

    Helpers::printPartsMenu();

    string part_selection;
    if (!Helpers::promptLine("Enter the part(s) (e.g., '1 2 3' or '1x2 2x5'): ", part_selection)) {
        return;
    }

    if (part_selection.empty()) {
        cout << "No parts entered." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Parse parts with quantities
    unordered_map<string, int> selectedParts = Helpers::parsePartsWithQuantities(part_selection, partsMap);

    if (selectedParts.empty()) {
        cout << "Invalid part codes. Please try again." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Display what will be added
    cout << "\n" << "Parts to add:\n";
    cout << "-----------\n";
    for (const auto &entry : selectedParts) {
        cout << entry.first << ": " << entry.second << "\n";
    }

    // Confirm before adding
    string confirmInput;
    if (!Helpers::promptLine("\nAdd these parts? (Y/N): ", confirmInput)) {
        return;
    }

    if (confirmInput.empty()) {
        confirmInput = "N";
    }

    char confirm = toupper(static_cast<unsigned char>(confirmInput[0]));
    if (confirm != 'Y') {
        cout << "Parts not added." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Add parts to tracker
    for (const auto &entry : selectedParts) {
        if (parts.find(entry.first) != parts.end()) {
            parts[entry.first] += entry.second;
            cout << entry.first << " quantity updated to " << parts[entry.first] << "." << "\n";
        } else {
            parts[entry.first] = entry.second;
            cout << entry.first << " added with quantity " << entry.second << "." << "\n";
        }
    }

    saveParts();
    this_thread::sleep_for(chrono::milliseconds(500));
}

void ExtraPartsTracker::editParts() {
    if (parts.empty()) {
        Helpers::clearScreen();
        cout << "----------------------------------------------" << "\n";
        cout << "----------  Extra Parts Tracker  -----------" << "\n";
        cout << "----------------------------------------------" << "\n\n";
        cout << "No parts available to edit." << "\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
        return;
    }

    Helpers::clearScreen();
    cout << "----------------------------------------------" << "\n";
    cout << "----------  Extra Parts Tracker  -----------" << "\n";
    cout << "----------------------------------------------" << "\n\n";

        
         auto partsMap = Helpers::getPartsMap();
         
         cout << "\n";
         Helpers::printPartsMenu();
         
         cout << "Current Extra Parts:\n"
              << "-------------------\n";
         for (const auto &entry : parts) {
             cout << entry.first << ": " << entry.second << "\n";
         }
    string part_selection;
    if (!Helpers::promptLine("Enter the part(s) to edit with new quantities (e.g., '1x5 2x3'): ", part_selection)) {
        return;
    }

    if (part_selection.empty()) {
        cout << "No parts entered." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Parse parts with quantities
    unordered_map<string, int> selectedParts = Helpers::parsePartsWithQuantities(part_selection, partsMap);

    if (selectedParts.empty()) {
        cout << "Invalid part codes. Please try again." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Check if all parts exist
    bool allExist = true;
    for (const auto &entry : selectedParts) {
        if (parts.find(entry.first) == parts.end()) {
            cout << "Part '" << entry.first << "' not found. Cannot edit non-existent parts." << "\n";
            allExist = false;
        }
    }

    if (!allExist) {
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Display what will be changed
    cout << "\nParts to update:\n";
    cout << "---------------\n";
    for (const auto &entry : selectedParts) {
        cout << entry.first << ": " << parts[entry.first] << " -> " << entry.second << "\n";
    }

    // Confirm before updating
    string confirmInput;
    if (!Helpers::promptLine("\nUpdate these parts? (Y/N): ", confirmInput)) {
        return;
    }

    if (confirmInput.empty()) {
        confirmInput = "N";
    }

    char confirm = toupper(static_cast<unsigned char>(confirmInput[0]));
    if (confirm != 'Y') {
        cout << "Parts not updated." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Update parts
    for (const auto &entry : selectedParts) {
        parts[entry.first] = entry.second;
    }

    saveParts();
    cout << "Parts updated successfully." << "\n";
    this_thread::sleep_for(chrono::milliseconds(500));
}

void ExtraPartsTracker::removeParts() {
    if (parts.empty()) {
        Helpers::clearScreen();
        cout << "----------------------------------------------" << "\n";
        cout << "----------  Extra Parts Tracker  -----------" << "\n";
        cout << "----------------------------------------------" << "\n\n";
        cout << "No parts available to remove." << "\n";
        this_thread::sleep_for(chrono::milliseconds(1000));
        return;
    }

    Helpers::clearScreen();
    cout << "----------------------------------------------" << "\n";
    cout << "----------  Extra Parts Tracker  -----------" << "\n";
    cout << "----------------------------------------------" << "\n\n";
         
         auto partsMap = Helpers::getPartsMap();
         
         cout << "\n";
         Helpers::printPartsMenu();
         
         cout << "Current Extra Parts:\n"
              << "-------------------\n";
         for (const auto &entry : parts) {
             cout << entry.first << ": " << entry.second << "\n";
         }
         string part_selection;
    if (!Helpers::promptLine("Enter the part(s) to remove (e.g., '1 2 3'): ", part_selection)) {
        return;
    }

    if (part_selection.empty()) {
        cout << "No parts entered." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Parse parts (without quantities - just get the part names)
    string s = part_selection;
    for (char &c : s) {
        if (c == ',') c = ' ';
        c = toupper((unsigned char)c);
    }

    vector<string> partsToRemove;
    string token;
    stringstream ss(s);

    while (ss >> token) {
        if (token.size() == 1) {
            auto it = partsMap.find(token);
            if (it != partsMap.end()) {
                partsToRemove.push_back(it->second);
            }
        } else {
            for (char c : token) {
                string key(1, c);
                auto it = partsMap.find(key);
                if (it != partsMap.end()) {
                    partsToRemove.push_back(it->second);
                }
            }
        }
    }

    // Remove duplicates
    sort(partsToRemove.begin(), partsToRemove.end());
    partsToRemove.erase(unique(partsToRemove.begin(), partsToRemove.end()), partsToRemove.end());

    if (partsToRemove.empty()) {
        cout << "Invalid part codes. Please try again." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Check if all parts exist
    bool allExist = true;
    for (const auto &partName : partsToRemove) {
        if (parts.find(partName) == parts.end()) {
            cout << "Part '" << partName << "' not found." << "\n";
            allExist = false;
        }
    }

    if (!allExist) {
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Display what will be removed
    cout << "\nParts to remove:\n";
    cout << "---------------\n";
    for (const auto &partName : partsToRemove) {
        cout << partName << " (Quantity: " << parts[partName] << ")" << "\n";
    }

    // Confirm before removing
    string confirmInput;
    if (!Helpers::promptLine("\nRemove these parts? (Y/N): ", confirmInput)) {
        return;
    }

    if (confirmInput.empty()) {
        confirmInput = "N";
    }

    char confirm = toupper(static_cast<unsigned char>(confirmInput[0]));
    if (confirm != 'Y') {
        cout << "Parts not removed." << "\n";
        this_thread::sleep_for(chrono::milliseconds(500));
        return;
    }

    // Remove parts
    for (const auto &partName : partsToRemove) {
        parts.erase(partName);
    }

    saveParts();
    cout << "Parts removed successfully." << "\n";
    this_thread::sleep_for(chrono::milliseconds(500));
}

void ExtraPartsTracker::viewExtraParts() {
    Helpers::clearScreen();
    cout << "----------------------------------------------" << "\n";
    cout << "----------  Extra Parts Tracker  -----------" << "\n";
    cout << "----------------------------------------------" << "\n\n";

    if (parts.empty()) {
        cout << "No extra parts currently tracked." << "\n";
        string continueInput;
        Helpers::promptLine("\nPress Enter to return to menu... ", continueInput);
        return;
    }

    cout << "Current Extra Parts:" << "\n"
         << "-------------------\n\n";

    int totalQuantity = 0;
    for (const auto &entry : parts) {
        cout << entry.first << ": " << entry.second << "\n";
        totalQuantity += entry.second;
    }

    cout << "\n-------------------\n";
    cout << "Total Quantity: " << totalQuantity << "\n";

    string continueInput;
    Helpers::promptLine("\nPress Enter to return to menu... ", continueInput);
}
