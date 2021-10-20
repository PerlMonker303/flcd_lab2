#pragma once
#include <vector>
#include <string>
#include <sstream>

class Helper
{
public:
	static std::vector<std::string> splitString(std::string str, char delim);
	static bool findInVector(std::vector<std::string> vec, std::string elem);
};

