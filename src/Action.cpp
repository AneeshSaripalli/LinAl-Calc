#pragma once

#include "../include/Action.h"

Action::Action (std::string name, Object *object, bool del) {
	this->name = name;
	this->obj_ptr = object;
	this->del = del;
}

Action::~Action () {
	if (!del) return;

	// Checks if the ptr should be deleted
	if (obj_ptr != 0) {
		/*
		 * Checks all possible name types and casts to the appropriate one before releasing the memory
		 */

		switch (obj_ptr->type)
		{
		case T_MATRIX:
		{
			Matrix *m_ptr = (Matrix *)obj_ptr;
			delete m_ptr;
			break;
		}
		case T_VECTOR:
		{
			Vector *v_ptr = (Vector *)obj_ptr;
			delete v_ptr;
			break;
		}
		case T_VALUE:
		{
			Value<double> *value_ptr = (Value<double> *) obj_ptr; // Casts pointer to Value<double> because that is the only used one
			delete value_ptr;
			break;
		}
		}
	}
}