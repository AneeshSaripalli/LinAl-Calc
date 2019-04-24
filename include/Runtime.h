#pragma once

#include <string>
#include <map>
#include "Matrix.h"
#include "Vector.h"

#include "Value.h"

struct Action { // Simple class to store an obj_ptr and name of the action
	std::string name; // Serves to identify where the Action object came from
	Object *obj_ptr; // Either is a true obj_ptr or 0, depending on whether the command needs to return something

	bool del; // bool to designate whether to delete the obj_ptr on destruction

	Action (std::string name, Object *object, bool del = false) {
		this->name = name;
		this->obj_ptr = object;
		this->del = del;
	}

	~Action () {
		if (del) { // Checks if the ptr should be deleted
			if (obj_ptr != 0) {
				/*
				 * Checks all possible name types and casts to the appropriate one before releasing the memory
				 */

				if (obj_ptr->type == T_MATRIX) {
					Matrix *m_ptr = (Matrix *)obj_ptr;
					delete m_ptr;

				}
				else if (obj_ptr->type == T_VECTOR) {
					Vector *v_ptr = (Vector *)obj_ptr;
					delete v_ptr;
				}
				else if (obj_ptr->type == T_VALUE) {
					Value<double> *value_ptr = (Value<double> *) obj_ptr; // Casts pointer to Value<double> because that is the only used one
					delete value_ptr;
				}

			}
		}
	}
};
class Runtime
{

public:

	template<class Key, class Value>
	bool keyInMap (std::map<Key, Value> &m, Key key); // Returns whether or not a key exists in a map

	int savedType (const std::string &name); // Returns the saved type <matrix/vector> of an object if it exists in the map, else null

	void store_In_Object_Map (std::string key, Object *obj_ptr); // Stores a key: string, value:obj_ptr pair in the obj_map map

	std::map < std::string, Object * > obj_map;
};