#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <typeinfo>


#ifndef OBJECT_H
#define OBJECT_H

#define T_MATRIX 0
#define T_VECTOR 1
#define T_VALUE 2
#define T_NULL -1

class Object {
public:
	int name;

	Object (int name);

	virtual ~Object ();
	virtual void print () = 0;

	static std::string Object::typeAsString (int type);

	template<class Type>
	Type *get () {
		return (Type *)(this);
	}

protected:

private:
};

#endif // OBJECT_H
