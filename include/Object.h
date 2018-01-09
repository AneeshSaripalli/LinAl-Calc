#include <iostream>
#include <iomanip>
#include <cmath>
#include <typeinfo>

#ifndef OBJECT_H
#define OBJECT_H

class Object {
public:
    std::string name;

    Object(std::string name);

    virtual ~Object();

    virtual void print() = 0;

    template<class Type>
    Type *get(Type const *t) {
        return (Type *) (this);
    }

protected:

private:
};

#endif // OBJECT_H
