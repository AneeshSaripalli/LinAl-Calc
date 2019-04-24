#ifndef VALUE_H
#define VALUE_H

#include "../include/Object.h"

template<class T>
class Value : public Object {
public:
	T value;

	Value (T value) : Object (T_VALUE) {
		// std::cout << "New value created with value " << value << std::endl;
		this->value = value;
	}

	Value () : Object (T_VALUE) {
		std::cout << "Enter value:\n " << ">> ";
		std::cin >> value;

		char c = std::cin.get ();
		if (c == '\n')
		{
			std::cout << std::endl;
		}
	}


	void print () {
		std::cout << value << std::endl;
	}

	virtual ~Value () {

	}
};

#endif
