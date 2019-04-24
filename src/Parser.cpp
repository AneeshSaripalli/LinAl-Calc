#include "../include/Parser.h"

Parser::Parser ()
{
	std::cout << "Initializing parser" << std::endl;
}

Parser::~Parser ()
{
	std::cout << "Deleting parser" << std::endl;
}

Action Parser::process (std::string& command, Runtime&rnt, bool del)
{
	std::stringstream ss;
	ss << command;

	std::string cmd;
	ss >> cmd;

	bool recognized = false;
	for (auto it = action_map.begin (); it != action_map.end (); ++it) {
		if (it->first == cmd)
		{
			recognized = true;
			return it->second (ss, rnt, del);
		}
	}

	if (!recognized) return action_map.at ("default")(ss, rnt, del);
}


void Parser::link (std::vector<std::string>& keys, Response& rsp)
{
	for (auto it = keys.begin (); it != keys.end (); ++it)
	{
		action_map.insert (std::make_pair (*it, rsp));
	}
}

void Parser::link (std::string key, Response& rsp)
{
	action_map.insert (std::make_pair (key, rsp));
}

bool Parser::unlink (std::string key)
{
	if (action_map.find (key) != action_map.end ())
	{
		action_map.erase (key);
		return true;
	}
	else
	{
		return false;
	}
}