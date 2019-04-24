#include "../include/Runtime.h"


void Runtime::store_in_object_map (std::string key, Object *obj_ptr) {
	if (key_in_map (obj_map, key)) {

		Object *obj_ptr_old = obj_map[key];

		if (obj_ptr->type == T_MATRIX) { delete (Matrix *)obj_ptr_old; }
		else if (obj_ptr->type == T_VECTOR) { delete (Vector *)obj_ptr_old; }

		obj_map[key] = obj_ptr;

	}
	else {
		obj_map.insert (std::make_pair (key, obj_ptr));
	}
}

int Runtime::saved_type (const std::string &s) {
	return key_in_map (obj_map, s) ? obj_map[s]->type : T_NULL;
}


Runtime::Runtime () {
	parser = std::make_unique<Parser> ();
}

Runtime::~Runtime ()
{
	for (auto it = obj_map.begin (); it != obj_map.end (); it++) {
		delete it->second;
	}

	obj_map.clear ();
}


template<class Key, class Value>
bool Runtime::key_in_map (std::map<Key, Value> &m, Key key) {
	auto pos = m.find (key);
	if (pos == m.end ())
		return false;
	else
		return true;
}