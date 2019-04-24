#include <map>
#include <vector>
#include <iostream>

#include "Action.h"
#include "Defines.h"
#include "Runtime.h"

#ifndef PARSER_H
#define PARSER_H

class Parser {
private:
	std::map < std::string, Response> action_map;

public:
	Parser ();
	virtual ~Parser ();

	Action process (std::string& command, Runtime& rnt, bool del = false);

	void link (std::vector<std::string> &keys, Response& rsp);
	void link (std::string key, Response& rsp);

	bool unlink (std::string key);
};

#endif