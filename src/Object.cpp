#include "../include/Object.h"


Object::Object (int name) {
	this->name = name;
}

std::string Object::typeAsString (int type)
{
	switch (type) {
	case T_NULL: return "null";
	case T_VECTOR: return "vector";
	case T_MATRIX: return "matrix";
	case T_VALUE: return "value";
	default: return "null";
	};
}

Object::~Object () {

}

