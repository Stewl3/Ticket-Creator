#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Helpers {
    std::unordered_map<std::string, std::string> getPartsMap();
    bool promptLine(const std::string &prompt, std::string &out);
    bool parseTicketNumber(const std::string &input, int &ticket_number);
    bool promptChar(const std::string &prompt, char &out);
    bool promptMenuChar(const std::string &prompt, char &out);
    void clearScreen();
    void printCreatorHeader();
    void printViewerHeader();
    std::string chooseLaptopType(char touchscreenLetter);
    std::vector<std::string> parseParts(const std::string &rawInput, const std::unordered_map<std::string, std::string> &partsMap);
    std::string joinPartsList(const std::vector<std::string> &parts);
    std::string getCurrentDateString();
    std::string getCurrentYearString();
    bool isValidDateFormat(const std::string &date);
    std::string normalizeDateToFileName(const std::string &date);
    bool manageTicketDocument(const std::string &filePath);
    std::vector<std::string> getSortedTicketFiles(const std::string &baseFolder);
    std::vector<std::string> readTicketBlocks(const std::string &fileName);
    bool writeTicketBlocks(const std::string &fileName, const std::vector<std::string> &blocks);
    int getLaptopTypeOrder(const std::string &laptop_type);
    void searchTicketByNumberAcrossDocuments(const std::string &baseFolder, int ticketNumber);
    std::string buildTicketBlock(const std::string &laptop_type, int ticket_number, const std::string &serial_number, const std::string &parts_list);
    bool saveTicketToFile(const std::string &laptop_type, int ticket_number, const std::string &serial_number, const std::string &parts_list);
    void printPartsMenu();
    void printTicket(int ticket_number, const std::string &serial_number, const std::string &laptop_type, const std::string &parts_list);
    bool promptForTicketNumber(int &ticket_number);
    std::unordered_map<std::string, int> parsePartsWithQuantities(const std::string &rawInput, const std::unordered_map<std::string, std::string> &partsMap);
    std::unordered_map<std::string, int> loadExtraPartsFromFile();
    bool removeExtraPartsFromFile(const std::vector<std::string> &partNames);
    std::vector<std::string> selectFromNumberedList(const std::vector<std::string> &items, const std::string &prompt);
}
