//
// Created by Aneesh on 1/11/2018.
//

#include "../include/Object.h"

template<class T>
class Value : public Object {
public:

    T value;

    Value(T value) : Object("value") {
        // std::cout << "New value created with value " << value << std::endl;
        this->value = value;
    }

    Value() : Object("value") {
        std::cout << "Enter value:\n " << ">> ";
        std::cin >> value;

        char c = std::cin.get();
        if(c == '\n')
        {
            std::cout << std::endl;
        }
    }


    void print() {
        std::cout << value << std::endl;
    }

    virtual ~Value() {

    }
};
