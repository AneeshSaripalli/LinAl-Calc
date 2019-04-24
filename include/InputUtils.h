#include <vector>
#include <sstream>

#include "Runtime.h"

#ifndef INPUT_UTILS_H
#define INPUT_UTILS_H

class InputUtils {

public:
	static std::vector<std::string> popFromStream (std::stringstream& ss, unsigned int n);

	static bool ensureTypeCoherence (std::vector<std::string> strs, std::vector<int> types, Runtime& rnt);

	static void trim (std::string &s); // Function to trim off leading or trailing spaces
	
	static bool contains (std::string findIn, std::string toFind) {
		size_t found = findIn.find (toFind);

		if (found != std::string::npos)
			return true;

		return false;
	}
};

#endif