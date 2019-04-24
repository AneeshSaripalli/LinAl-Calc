#include <sstream>
#include <iostream>
#include <vector>
#include "Action.h"
#include "Runtime.h"
#include "InputUtils.h"

#ifndef STDMATHLIB_H
#define STDMATHLIB_H

using Response = Action (*)(std::stringstream&, Runtime&, bool del);

using namespace std;

namespace stdlib {
	Response define{
		[] (std::stringstream& ss, Runtime& rnt, bool del = false)
		{
			auto decs = InputUtils::popFromStream (ss, 2);

			std::string type = decs[0];
			std::string alias = decs[1];

			if (alias.size () == 0 || type.size () == 0) {
				cout << "-> Data can not be empty strings. Type \"help\" for help." << std::endl;
				return Action (string ("define"), 0);
			}

			Object *newObj;

			if (type == "matrix") {
				newObj = new Matrix ();
			}
			else if (type == "vector") {
				newObj = new Vector ();
			}
			else if (type == "value") {
				newObj = new Value<double> ();
			}
			else {
				cout << "-> Data format not recognized.\n define <matrix/vector> <name>" << std::endl;
				return Action (string ("define"), 0);
			}

			rnt.store_In_Object_Map (alias, newObj);

			return Action (string ("define"), newObj, false);
		}
	};
	Response memory{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
			for (auto it = rnt.obj_map.begin (); it != rnt.obj_map.end (); it++) {
				cout << "-------------------------" << std::endl;
				cout << "Object " << it->first << " - " << Object::typeAsString (rnt.savedType (it->first)) << std::endl;
				it->second->print ();
				cout << "-------------------------" << std::endl;
			}

			return Action ("memory", 0);
		}
	};
	Response delete_from_runtime{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
		auto aliasVec = InputUtils::popFromStream (ss, 1);
			std::string alias = aliasVec[0];

			if (rnt.keyInMap (rnt.obj_map, alias)) {
				Object *obj_ptr = rnt.obj_map[alias];

				if (obj_ptr->type == T_MATRIX) {
					delete (Matrix *)obj_ptr;
				}
				else if (obj_ptr->type == T_VECTOR) {
					delete (Vector *)obj_ptr;
				}
				else {
					cout << "-> Object with the name " << alias << " has not been initialized in this run time." << std::endl;
					return Action ("delete", 0);
				}

				rnt.obj_map.erase (alias);

				return Action ("delete", 0);
			}
		}
	};
}

namespace help_lib {
	Response do_test{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
			std::cout << "Testing." << std::endl;
			return Action ("Test", 0);
		}
	};


	Response do_exit{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
			return Action ("exit", 0);
		}
	};

};
#endif