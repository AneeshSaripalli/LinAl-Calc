#include "../include/Runtime.h"


void Runtime::store_In_Object_Map (std::string key, Object *obj_ptr) {
	if (keyInMap (obj_map, key)) {

		Object *obj_ptr_old = obj_map[key];

		if (obj_ptr->type == T_MATRIX) { delete (Matrix *)obj_ptr_old; }
		else if (obj_ptr->type == T_VECTOR) { delete (Vector *)obj_ptr_old; }

		obj_map[key] = obj_ptr;

	}
	else {
		obj_map.insert (std::make_pair (key, obj_ptr));
	}
}

int Runtime::savedType (const std::string &s) {
	return keyInMap (obj_map, s) ? obj_map[s]->type : T_NULL;
}



template<class Key, class Value>
bool Runtime::keyInMap (std::map<Key, Value> &m, Key key) {
	auto pos = m.find (key);
	if (pos == m.end ())
		return false;
	else
		return true;
}