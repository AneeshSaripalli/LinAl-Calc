#include "../include/InputUtils.h"


std::vector<std::string> InputUtils::pop_from_stream (std::stringstream& ss, unsigned int n) {
	std::vector<std::string> pars;

	for (auto i = 0u; i < n; ++i)
	{
		std::string str;
		ss >> str;
		pars.push_back (str);
	}

	return pars;
}


bool InputUtils::ensure_type_validity (std::vector<std::string> strs, std::vector<int> types, Runtime& rnt)
{
	if (strs.size () < types.size ()) return false;
	const auto len = strs.size ();

	bool isCoherent = true;
	for (auto i = 0u; i < len && isCoherent; ++i)
	{
		isCoherent &= (rnt.saved_type (strs[i]) == types[i]);
	}

	return isCoherent;
}

void InputUtils::trim (std::string &s) { // Function to trim off leading or trailing spaces
	for (int i = 0u; i < s.length (); i++) {
		if (s[i] == ' ') {
			s.erase (i, 1);
		}
		else {
			break;
		}

		i--;
	}

	for (int i = s.length () - 1; i >= 0; i--) {
		if (s[i] == ' ') {
			s.erase (i, 1);
		}
		else {
			break;
		}
	}
}