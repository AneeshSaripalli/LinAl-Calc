#include <iostream>
#include <iomanip>
#include <cmath>

#ifndef OBJECT_H
#define OBJECT_H

class Object
{
    public:
        std::string name;

        Object(std::string name);
        virtual ~Object();

        virtual void print() = 0;

        auto convert() -> Object*;

    protected:

    private:
};

#endif // OBJECT_H
