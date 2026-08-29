#pragma once

#include <string>
#include <unordered_map>

class ExtraPartsTracker {
public:
    void run();

private:
    std::unordered_map<std::string, int> parts;  // Part name -> quantity
    const std::string partsFilePath = "output/ExtraParts.txt";
    
    void loadParts();
    void saveParts();
    bool displayMenu();  // Returns false when ESC is pressed
    void addParts();
    void editParts();
    void removeParts();
    void viewExtraParts();
};
