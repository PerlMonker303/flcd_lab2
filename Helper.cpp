#include "Helper.h"

std::vector<std::string> Helper::splitString(std::string str, char delim) {
    std::vector<std::string> result;
    std::istringstream iss(str);
    std::string item;
    while (std::getline(iss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

bool Helper::findInVector(std::vector<std::string> vec, std::string elem) {
    for (auto v : vec) {
        if (v == elem) {
            return true;
        }
    }
    return false;
}