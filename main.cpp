#include <stdlib.h>
#include <sstream>
#include <map>
#include <math.h>
#include <typeinfo>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>

#include "src/Value.cpp"
#include "include/MathHelpers.h"
#include "include/TextualHelpers.h"
#include "include/StdMathLib.h"
#include "include/Object.h"
#include "include/Matrix.h"
#include "include/Runtime.h"
#include "include/Action.h"
#include "include/InputUtils.h"
#include "include/Defines.h"
#include "include/Parser.h"


using namespace std;

std::map < std::string, Response> action_map;


void entryMessage () {
	std::cout << "       ----- Linear Algebra -----" << std::endl;
}



int main () {
	entryMessage ();

	Runtime rnt;

	stdlib::init_map (rnt);
	test::init_map (rnt);
	help_lib::init_map (rnt);
	mathlib::init_map (rnt);


	std::string cmd;

	do {
		std::cout << "? "; // Prompts input from user
		getline (std::cin, cmd); // Accepts input from user

		InputUtils::trim (cmd);
		if (!cmd.length ()) { continue; }

		Action action = rnt.parser->process (cmd, rnt);
		Object *obj_ptr = action.obj_ptr;

		if (obj_ptr) { obj_ptr->print (); }

	} while (cmd != "exit");

	return 0;
}
