#pragma once

#include <string>
#include <map>
#include <memory>

#include "Matrix.h"
#include "Vector.h"
#include "Value.h"
#include "Parser.h"

#ifndef RUNTIME_H
#define RUNTIME_H


class Runtime
{

public:
	Runtime ();
	virtual ~Runtime ();

	template<class Key, class Value>
	bool keyInMap (std::map<Key, Value> &m, Key key); // Returns whether or not a key exists in a map

	int savedType (const std::string &name); // Returns the saved type <matrix/vector> of an object if it exists in the map, else null

	void store_In_Object_Map (std::string key, Object *obj_ptr); // Stores a key: string, value:obj_ptr pair in the obj_map map

	std::unique_ptr<Parser> parser;

	std::map < std::string, Object * > obj_map;
};

#endif