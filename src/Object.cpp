#include "../include/Object.h"
#include "../include/Matrix.h"
#include "../include/Vector.h"

Object::Object(std::string name)
{
    this->name = name;
}

Object::~Object()
{

}

auto Object::convert()
{
    if(name == "matrix") {return (Matrix*) this;}
    else if(name == "vector") {return (Vector*) this;}
}

