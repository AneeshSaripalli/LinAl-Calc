#include <sstream>

#ifndef DEFINES_H
#define DEFINES_H

class Action;
class Runtime;


using Response = Action (*)(std::stringstream&, Runtime&, bool del);

#endif