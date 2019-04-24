#include "Vector.h"
#include "Object.h"
#include "Value.h"
#include "Matrix.h"

#ifndef ACTION_H
#define ACTION_H
class Action { // Simple class to store an obj_ptr and name of the action
public:
	std::string name; // Serves to identify where the Action object came from
	Object *obj_ptr; // Either is a true obj_ptr or 0, depending on whether the command needs to return something

	bool del; // bool to designate whether to delete the obj_ptr on destruction

	Action (std::string name, Object *object, bool del = false);

	virtual ~Action ();
};

#endif