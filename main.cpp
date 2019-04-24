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
#include "include/Object.h"
#include "include/Matrix.h"
#include "include/Runtime.h"
#include "include/Action.h"

#define min(x,y) (x < y ? x : y);

using namespace std;
using Response = Action (*)(std::stringstream&, Runtime&, bool del);

std::map < std::string, Response> action_map;

std::vector<std::string> popFromStream (std::stringstream& ss, unsigned int n) {
	std::vector<std::string> pars;

	for (auto i = 0u; i < n; ++i)
	{
		std::string str;
		ss >> str;
		pars.push_back (str);
	}

	return pars;
}

bool ensureTypeCoherence (std::vector<std::string> strs, std::vector<int> types, Runtime& rnt)
{
	if (strs.size () < types.size ()) return false;
	const auto len = strs.size ();

	bool isCoherent = true;
	for (auto i = 0u; i < len; ++i)
	{
		isCoherent &= (rnt.savedType (strs[i]) == types[i]);
	}

	return isCoherent;
}

void trim (std::string &s) { // Function to trim off leading or trailing spaces
	for (int i = 0; i < s.length (); i++) {
		if (s[i] == ' ') {
			s.erase (i, 1);
		}
		else {
			break;
		}

		i--;
	}

	for (int i = s.length () - 1; i >= 0; i--) {
		if (s[i] == ' ') {
			s.erase (i, 1);
		}
		else {
			break;
		}
	}
}
bool contains (std::string findIn, std::string toFind) {
	size_t found = findIn.find (toFind);

	if (found != std::string::npos)
		return true;

	return false;
}
void entryMessage () {
	std::cout << "       ----- Linear Algebra -----" << std::endl;
}

bool contains (std::string findIn, std::string toFind); // Returns whether or not a substring exists within a string

void help () { // Console output to help direct users
	std::cout << "Functional Commands: Any data can be given with the name of the object or inputted after command call." << std::endl;
	std::cout << "\tEx: define matrix a ... \nrank a is equivalent to rank ... followed by input." << std::endl;
	std::cout << "-- define || def --> Alias either a matrix or a vector with a name to use later on." << std::endl;
	std::cout << "-- assign --> Alias the result of some command with a name.\n\t Ex: \"assign b inverse a\" assigns b the matrix value of a inverse." << std::endl;
	std::cout << "-- approx_sol || as --> Least squares solution to Ax = B." << std::endl;
	std::cout << "-- calculate || calc --> Calculate mathematical operations between matrices and vectors." << std::endl;
	std::cout << "-- least_squares || ls --> Accepts data for least squares regression." << std::endl;
	std::cout << "-- rank --> Returns rank of a matrix." << std::endl;
	std::cout << "-- inverse || inv --> Returns the inverse of the matrix if it exists." << std::endl;
	std::cout << "-- projection || proj --> Projects the first input vector onto the second and returns the vector projection.\n\t Ex: \"proj a b\" would return the vector projection of a onto b." << std::endl;
	std::cout << "-- dot --> Returns dot product of two vectors." << std::endl;
	std::cout << "-- determinant || det --> Returns the determinant of the matrix." << std::endl;
	std::cout << "-- basis --> Returns the linearly independent basis of both the column and row spaces." << std::endl;
	std::cout << "-- clear || cls --> Clears the command line prompt screen." << std::endl;
	std::cout << "-- exit --> Exits the command line application." << std::endl;
	std::cout << "-- help --> Prompts these options again." << std::endl;
}

Action process (std::string& command, Runtime&rnt, bool del = false)
{
	std::stringstream ss;
	ss << command;

	std::string cmd;
	ss >> cmd;

	bool recognized = false;
	for (auto it = action_map.begin (); it != action_map.end (); ++it) {
		if (it->first == cmd)
		{
			recognized = true;
			return it->second (ss, rnt, del);
		}
	}

	if (!recognized) return action_map.at ("default")(ss, rnt, del);
}


Response define{
	[] (std::stringstream& ss, Runtime& rnt, bool del = false)
	{
		auto decs = popFromStream (ss, 2);

		std::string type = decs[0];
		std::string alias = decs[1];

		if (alias.size () == 0 || type.size () == 0) {
			cout << "-> Data can not be empty strings. Type \"help\" for help." << std::endl;
			return Action (string ("define"), 0);
		}

		Object *newObj;

		if (type == "matrix") {
			newObj = new Matrix ();
		}
		else if (type == "vector") {
			newObj = new Vector ();
		}
		else if (type == "value") {
			newObj = new Value<double> ();
		}
		else {
			cout << "-> Data format not recognized.\n define <matrix/vector> <name>" << std::endl;
			return Action (string ("define"), 0);
		}

		rnt.store_In_Object_Map (alias, newObj);

		return Action (string ("define"), newObj, false);
	}
};
Response output{
	[] (std::stringstream& ss, Runtime& rnt, bool del = false) {
		auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		int type = rnt.savedType (alias);

		if (type != T_NULL) {
			return Action (string ("print"), rnt.obj_map[alias], false);
		}
		else {
			cout << "-> Variable \"" << alias << "\" not defined during this session." << std::endl;
		}

		return Action ("recall", 0);
	}
};
Response default_action{
	[] (std::stringstream& ss, Runtime& rnt, bool del = false) -> Action
	{

		std::string cmd = ss.str ();

		if (rnt.savedType (cmd) != T_NULL)
		{
			return Action (string ("default interp"), rnt.obj_map[cmd]);
		}
		else {
			std::cout << "-> Command " << cmd << " not recognized. Type \"help\" for help." << std::endl;
			return Action ("undef", 0);
		}
	}
};
Response memory{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		for (auto it = rnt.obj_map.begin (); it != rnt.obj_map.end (); it++) {
			cout << "-------------------------" << std::endl;
			cout << "Object " << it->first << " - " << Object::typeAsString (rnt.savedType (it->first)) << std::endl;
			it->second->print ();
			cout << "-------------------------" << std::endl;
		}

		return Action ("memory", 0);
	}
};
Response assign{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		std::string cmd = ss.str ();

		auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		int start_pos = cmd.find (alias, 6);
		int end_pos = start_pos + alias.length ();

		std::string rest = cmd.substr (end_pos);
		trim (rest);

		Action result = process (rest, rnt, false);

		Object *data_ptr = result.obj_ptr;

		if (data_ptr == 0) {
			cout << "-> Assignment function does not have a return type." << std::endl;
		}
		else {
			rnt.store_In_Object_Map (alias, data_ptr);
		}

		return Action ("assign", 0);
	}
};
Response projection{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
	auto vectors = popFromStream (ss, 2);
		bool wellTyped = ensureTypeCoherence (vectors, { T_VECTOR, T_VECTOR }, rnt);

		std::cout << "Data is well typed " << wellTyped << std::endl;

		if (wellTyped) {
			Object *v1_obj = rnt.obj_map[vectors[0]];
			Object *v2_obj = rnt.obj_map[vectors[1]];

			if (v1_obj->type == T_VECTOR && v2_obj->type == T_VECTOR) {
				Vector *v1 = (Vector *)v1_obj;
				Vector *v2 = (Vector *)v2_obj;

				double u_dot_v = *v1 * *v2;
				double v_dot_v = *v2 * *v2;

				Vector *proj = (*v2) * (u_dot_v / v_dot_v);

				return Action ("proj", proj, del);

			}
			else {
				cout << "Projection not supported between entered data types." << std::endl;
				return Action ("proj", nullptr, false);
			}
		}
	}
};
Response delete_from_runtime{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
	auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		if (rnt.keyInMap (rnt.obj_map, alias)) {
			Object *obj_ptr = rnt.obj_map[alias];

			if (obj_ptr->type == T_MATRIX) {
				delete (Matrix *)obj_ptr;
			}
			else if (obj_ptr->type == T_VECTOR) {
				delete (Vector *)obj_ptr;
			}
			else {
				cout << "-> Object with the name " << alias << " has not been initialized in this run time." << std::endl;
				return Action ("delete", 0);
			}

			rnt.obj_map.erase (alias);

			return Action ("delete", 0);
		}
	}
};
Response calculate{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
	auto expVec = popFromStream (ss, 3);
		string first = expVec[0], oper = expVec[1], second = expVec[2];

		if (rnt.keyInMap (rnt.obj_map, first) || rnt.keyInMap (rnt.obj_map, second)) {

			Object *obj_ptr1 = rnt.obj_map[first];
			Object *obj_ptr2 = rnt.obj_map[second];

			Object *returnObj = 0;

			if (obj_ptr1->type == T_MATRIX && obj_ptr2->type == T_MATRIX) {
				returnObj = matrix_Operations ((Matrix *)obj_ptr1, (Matrix *)obj_ptr2, oper);

				if (returnObj == 0) {
					cout << oper << " is not an acceptable operation between two matrices." << std::endl;
					return Action ("calc mmult", 0);
				}

				returnObj->type = T_MATRIX;

				return Action ("calc mmult", returnObj, del);

			}
			else if (obj_ptr1->type == T_MATRIX && obj_ptr2->type == T_VECTOR) {
				returnObj = matrix_vector_operations (obj_ptr1->get<Matrix> (), obj_ptr2->get<Vector> (), oper);
				returnObj->type = T_VECTOR;

				return Action ("calc mvmult", returnObj, del);

			}
			else if (obj_ptr1->type == T_VECTOR && obj_ptr2->type == T_VECTOR) {
				if (oper == "*") {
					string processString = "dot " + first + " " + second;
					return process (processString, rnt, del);
				}
				else {
					returnObj = vector_operations (obj_ptr1->get <Vector> (), obj_ptr2->get <Vector> (), oper);
					returnObj->type = T_VECTOR;

					return Action ("calc voper", returnObj, del);
				}
			}
		}
	}
};
Response approx_sol{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		string mat_a;
		string vec_b;

		ss >> mat_a >> vec_b;

		Matrix *m_ptr = 0;
		Vector *v_ptr = 0;

		if (rnt.savedType (mat_a) == T_MATRIX && rnt.savedType (vec_b) == T_VECTOR) {
			m_ptr = (Matrix *)rnt.obj_map[mat_a];
			v_ptr = (Vector *)rnt.obj_map[vec_b];
		}
		else {
			if (mat_a != "" || vec_b != "") {
				cout << "-> Data input format did not match the appropriate format." << std::endl;
			}

			*m_ptr = Matrix ();
			*v_ptr = Vector ();
		}

		Vector *v_sol = solveLeastSquares (*m_ptr, *v_ptr);

		return Action ("approx_sol", v_sol, del);
	}
};
Response least_squares{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		int data_points;
		cout << "-> Enter the number of data points: ";
		cin >> data_points;

		if (data_points <= 1) {
			cout << "-> Enter at least 2 data points: " << std::endl;
			return Action ("least_squares", 0);
		}

		int power;
		cout << "-> Enter the power to evaluate the data set at: ";
		cin >> power;

		if (power > 10) {
			cout << "-> Power is too great." << std::endl;
			return Action ("least_squares", 0);
		}

		Matrix *data_mat = new Matrix (data_points, power + 1);
		Vector *y_vals = new Vector (data_points);


		for (int row = 0; row < data_points; row++) {
			int x, y;
			cout << "-> Enter x, y: ";
			cin >> x >> y;

			for (int col = power; col >= 0; col--) {
				(*data_mat)[row][power - col] = pow (x, col);
			}

			(*y_vals)[row] = y;
		}

		Vector *result = solveLeastSquares (*data_mat, *y_vals);

		return Action ("least_squares", result, del);
	}
};
Response solve_rank{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		string alias;
		ss >> alias;

		if (rnt.savedType (alias) == T_MATRIX) {
			Matrix *m_ptr = (Matrix *)rnt.obj_map[alias];

			Matrix *reduced = m_ptr->gaussianReducedForm ();

			cout << "-> The rank of the matrix is " << reduced->countPivots () << "." << std::endl;

			delete reduced;
		}
		else {
			if (alias != "") {
				cout << "-> A matrix with the name of " << alias << " was not found." << std::endl;
			}

			Matrix m = Matrix ();

			m.gaussianReduce ();

			cout << "-> The rank of the matrix is " << m.countPivots () << "." << std::endl;
		}

		return Action ("rank", 0);
	}
};
Response solve_inverse{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		string alias;
		ss >> alias;

		Matrix *m;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		Matrix *inverse = m->inverse ();

		if (inverse != 0) {
			return Action ("inverse", inverse, del);
		}

		return Action ("inverse", 0);
	}
};
Response determinant{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		string alias;
		ss >> alias;

		Matrix *m = nullptr;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		const double det = Matrix::determinant (m);
		Value<double> *det_ptr = new Value<double> (det);

		return Action ("determinant", det_ptr, del);
	}
};
Response dot{
	[] (std::stringstream& ss, Runtime& rnt, bool del) {
		std::string v1, v2;
		ss >> v1 >> v2;

		Vector *v1_ptr, *v2_ptr;

		double dot = 0;

		if (rnt.savedType (v1) == T_VECTOR && rnt.savedType (v2) == T_VECTOR) {
			v1_ptr = (Vector *)rnt.obj_map[v1];
			v2_ptr = (Vector *)rnt.obj_map[v2];

		}
		else {

			*v1_ptr = Vector ();
			*v2_ptr = Vector ();
		}

		dot = (*v1_ptr) * (*v2_ptr);

		Value<double> *value_ptr = new Value<double> (dot);

		return Action ("dot", value_ptr, del);
	}
};
Response solve_basis{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		string alias;
		ss >> alias;

		Matrix *m;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		Matrix *reduced = m->gaussianReducedForm ();

		int min_dim = min (m->rows, m->cols);

		int *basisV = new int[min_dim];

		for (int i = 0; i < min_dim; i++) {
			basisV[i] = 0;
		}

		for (int i = 0; i < min_dim; i++) {
			if ((*reduced)[i][i] != 0) {
				basisV[i] = 1;
			}
		}

		cout << "-> Basis Vectors for the Row Space [Read Horizontal]: " << std::endl;

		for (int i = 0; i < min_dim; i++) {
			if (basisV[i]) {
				(*reduced)[i].print ();
			}
		}

		cout << "-> Basis Vectors for the Column Space [Read Vertical]: " << std::endl;

		for (int i = 0; i < min_dim; i++) {
			if (basisV[i]) {
				(*m)[i].print ();
			}
		}

		delete[] reduced;
		delete[] basisV;

		return Action ("basis", 0);

	}
};
Response do_exit{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		return Action ("exit", 0);
	}
};
Response do_help{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		help ();
		return Action ("help", 0);
	}
};

void link_response (std::vector<std::string>& key_list, Response& resp)
{
	for (auto it = key_list.begin (); it != key_list.end (); ++it) {
		action_map.insert (std::make_pair (*it, resp));
	}
}

void init_map () {
	link_response (std::vector<std::string>{ "default" }, default_action);

	link_response (std::vector<std::string>{ "def", "define" }, define);

	link_response (std::vector<std::string>{ "print", "recall" }, output);

	link_response (std::vector<std::string>{ "assign" }, assign);

	link_response (std::vector<std::string>{ "mem", "memory" }, memory);

	link_response (std::vector<std::string>{ "proj", "projeciton" }, projection);

	link_response (std::vector<std::string>{ "delete", "del" }, delete_from_runtime);

	link_response (std::vector<std::string>{ "calculate", "calc" }, calculate);

	link_response (std::vector<std::string>{ "approx_sol", "as" }, approx_sol);

	link_response (std::vector<std::string>{ "least_squares", "ls" }, least_squares);

	link_response (std::vector<std::string>{ "rank", "rank_of" }, solve_rank);

	link_response (std::vector<std::string>{ "inv", "inverse" }, solve_inverse);

	link_response (std::vector<std::string>{ "det", "determinant" }, determinant);

	link_response (std::vector<std::string>{ "dot" }, dot);

	link_response (std::vector<std::string>{"basis"}, solve_basis);

	link_response (std::vector<std::string>{"exit", "quit"}, do_exit);

	link_response (std::vector<std::string>{"help", "?"}, do_help);
}

int main () {
	entryMessage ();

	Runtime rnt;

	std::string cmd;

	init_map ();

	do {
		std::cout << "? "; // Prompts input from user
		getline (std::cin, cmd); // Accepts input from user

		trim (cmd);
		if (cmd.length () == 0) { continue; }

		Action action = process (cmd, rnt);
		Object *obj_ptr = action.obj_ptr;

		if (obj_ptr) { obj_ptr->print (); }

	} while (cmd != "exit");

	for (auto it = rnt.obj_map.begin (); it != rnt.obj_map.end (); it++) {
		delete it->second;
	}

	rnt.obj_map.clear ();

	return 0;
}


/*
Action process (std::string &cmd, Runtime& rnt, bool del = false) {
	std::stringstream ss;
	ss << cmd;

	std::string command;
	ss >> command;

	if (rnt.savedType (command) != T_NULL)
	{
		return Action (string ("default interp"), rnt.obj_map[command]);
	}
	else if (command == "define" || command == "def") {
		auto decs = popFromStream (ss, 2);

		std::string type = decs[0];
		std::string alias = decs[1];

		if (alias.size () == 0 || type.size () == 0) {
			cout << "-> Data can not be empty strings. Type \"help\" for help." << std::endl;
			return Action (string ("define"), 0);
		}

		Object *newObj;

		if (type == "matrix") {
			newObj = new Matrix ();
		}
		else if (type == "vector") {
			newObj = new Vector ();
		}
		else if (type == "value") {
			newObj = new Value<double> ();
		}
		else {
			cout << "-> Data format not recognized.\n define <matrix/vector> <name>" << std::endl;
			return Action (string ("define"), 0);
		}

		rnt.store_In_Object_Map (alias, newObj);

		return Action (string ("define"), newObj, false);

	}
	else if (command == "recall" || command == "print") {
		auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		int type = rnt.savedType (alias);

		if (type != T_NULL) {
			return Action (string ("print"), rnt.obj_map[alias], false);
		}
		else {
			cout << "-> Variable \"" << alias << "\" not defined during this session." << std::endl;
		}

		return Action ("recall", 0);
	}
	else if (command == "assign") {
		auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		int start_pos = cmd.find (alias, 6);
		int end_pos = start_pos + alias.length ();

		std::string rest = cmd.substr (end_pos);
		trim (rest);

		Action result = process (rest, rnt, false);

		Object *data_ptr = result.obj_ptr;

		if (data_ptr == 0) {
			cout << "-> Assignment function does not have a return type." << std::endl;
		}
		else {
			rnt.store_In_Object_Map (alias, data_ptr);
		}

		return Action ("assign", 0);
	}
	else if (command == "mem" || command == "memory") {
		for (auto it = rnt.obj_map.begin (); it != rnt.obj_map.end (); it++) {
			cout << "-------------------------" << std::endl;
			cout << "Object " << it->first << " - " << Object::typeAsString (rnt.savedType (it->first)) << std::endl;
			it->second->print ();
			cout << "-------------------------" << std::endl;
		}

		return Action ("memory", 0);
	}
	else if (command == "cross") {
		std::string v1_name, v2_name;

		auto vecs = popFromStream (ss, 2);

		v1_name = vecs[0];
		v2_name = vecs[1];

		if (rnt.keyInMap (rnt.obj_map, v1_name) && rnt.keyInMap (rnt.obj_map, v2_name)) {
			Object *v1 = rnt.obj_map[v1_name];
			Object *v2 = rnt.obj_map[v2_name];

			if (v1->type == T_VECTOR && v2->type == T_VECTOR) {

			}

			cout << "Command not currently supported." << std::endl;
		}
	}
	else if (command == "proj" || command == "projection") {
		auto vectors = popFromStream (ss, 2);
		bool wellTyped = ensureTypeCoherence (vectors, { T_VECTOR, T_VECTOR }, rnt);

		std::cout << "Data is well typed " << wellTyped << std::endl;

		if (wellTyped) {
			Object *v1_obj = rnt.obj_map[vectors[0]];
			Object *v2_obj = rnt.obj_map[vectors[1]];

			if (v1_obj->type == T_VECTOR && v2_obj->type == T_VECTOR) {
				Vector *v1 = (Vector *)v1_obj;
				Vector *v2 = (Vector *)v2_obj;

				double u_dot_v = *v1 * *v2;
				double v_dot_v = *v2 * *v2;

				Vector *proj = (*v2) * (u_dot_v / v_dot_v);

				return Action ("proj", proj, del);

			}
			else {
				cout << "Projection not supported between entered data types." << std::endl;
				return Action ("proj", nullptr, false);
			}
		}


	}
	else if (command == "delete" || command == "del") {
		auto aliasVec = popFromStream (ss, 1);
		std::string alias = aliasVec[0];

		if (rnt.keyInMap (rnt.obj_map, alias)) {
			Object *obj_ptr = rnt.obj_map[alias];

			if (obj_ptr->type == T_MATRIX) {
				delete (Matrix *)obj_ptr;
			}
			else if (obj_ptr->type == T_VECTOR) {
				delete (Vector *)obj_ptr;
			}
			else {
				cout << "-> Object with the name " << alias << " has not been initialized in this run time." << std::endl;
				return Action ("delete", 0);
			}

			rnt.obj_map.erase (alias);

			return Action ("delete", 0);
		}

	}
	else if (command == "calculate" || command == "calc") {
		auto expVec = popFromStream (ss, 3);
		string first = expVec[0], oper = expVec[1], second = expVec[2];

		if (rnt.keyInMap (rnt.obj_map, first) || rnt.keyInMap (rnt.obj_map, second)) {

			Object *obj_ptr1 = rnt.obj_map[first];
			Object *obj_ptr2 = rnt.obj_map[second];

			Object *returnObj = 0;

			if (obj_ptr1->type == T_MATRIX && obj_ptr2->type == T_MATRIX) {
				returnObj = matrix_Operations ((Matrix *)obj_ptr1, (Matrix *)obj_ptr2, oper);

				if (returnObj == 0) {
					cout << oper << " is not an acceptable operation between two matrices." << std::endl;
					return Action ("calc mmult", 0);
				}

				returnObj->type = T_MATRIX;

				return Action ("calc mmult", returnObj, del);

			}
			else if (obj_ptr1->type == T_MATRIX && obj_ptr2->type == T_VECTOR) {
				returnObj = matrix_vector_operations (obj_ptr1->get<Matrix> (), obj_ptr2->get<Vector> (), oper);
				returnObj->type = T_VECTOR;

				return Action ("calc mvmult", returnObj, del);

			}
			else if (obj_ptr1->type == T_VECTOR && obj_ptr2->type == T_VECTOR) {
				if (oper == "*") {
					string processString = "dot " + first + " " + second;
					return process (processString, rnt, del);
				}
				else {
					returnObj = vector_operations (obj_ptr1->get <Vector> (), obj_ptr2->get <Vector> (), oper);
					returnObj->type = T_VECTOR;

					return Action ("calc voper", returnObj, del);
				}
			}
		}

	}
	else if (command == "approx_sol" || command == "as") {
		string mat_a;
		string vec_b;

		ss >> mat_a >> vec_b;

		Matrix *m_ptr = 0;
		Vector *v_ptr = 0;

		if (rnt.savedType (mat_a) == T_MATRIX && rnt.savedType (vec_b) == T_VECTOR) {
			m_ptr = (Matrix *)rnt.obj_map[mat_a];
			v_ptr = (Vector *)rnt.obj_map[vec_b];

		}
		else {
			if (mat_a != "" || vec_b != "") {
				cout << "-> Data input format did not match the appropriate format." << std::endl;
			}

			*m_ptr = Matrix ();
			*v_ptr = Vector ();
		}

		Vector *v_sol = solveLeastSquares (*m_ptr, *v_ptr);

		return Action ("approx_sol", v_sol, del);

	}
	else if (command == "least_squares" || command == "ls") {
		int data_points;
		cout << "-> Enter the number of data points: ";
		cin >> data_points;

		if (data_points <= 1) {
			cout << "-> Enter at least 2 data points: " << std::endl;
			return Action ("least_squares", 0);
		}

		int power;
		cout << "-> Enter the power to evaluate the data set at: ";
		cin >> power;

		if (power > 10) {
			cout << "-> Power is too great." << std::endl;
			return Action ("least_squares", 0);
		}

		Matrix *data_mat = new Matrix (data_points, power + 1);
		Vector *y_vals = new Vector (data_points);


		for (int row = 0; row < data_points; row++) {
			int x, y;
			cout << "-> Enter x, y: ";
			cin >> x >> y;

			for (int col = power; col >= 0; col--) {
				(*data_mat)[row][power - col] = pow (x, col);
			}

			(*y_vals)[row] = y;
		}

		Vector *result = solveLeastSquares (*data_mat, *y_vals);

		return Action ("least_squares", result, del);
	}
	else if (command == "rank") {
		string alias;
		ss >> alias;

		if (rnt.savedType (alias) == T_MATRIX) {
			Matrix *m_ptr = (Matrix *)rnt.obj_map[alias];

			Matrix *reduced = m_ptr->gaussianReducedForm ();

			cout << "-> The rank of the matrix is " << reduced->countPivots () << "." << std::endl;

			delete reduced;
		}
		else {
			if (alias != "") {
				cout << "-> A matrix with the name of " << alias << " was not found." << std::endl;
			}

			Matrix m = Matrix ();

			m.gaussianReduce ();

			cout << "-> The rank of the matrix is " << m.countPivots () << "." << std::endl;
		}

		return Action ("rank", 0);
	}
	else if (command == "cls" || command == "clear") {
		cout << flush;
		system ("CLS");
		entryMessage ();

		return Action ("CLS", 0);

	}
	else if (command == "inverse" || command == "inv") {
		string alias;
		ss >> alias;

		Matrix *m;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		Matrix *inverse = m->inverse ();

		if (inverse != 0) {
			return Action ("inverse", inverse, del);
		}

		return Action ("inverse", 0);
	}
	else if (command == "dot") {
		std::string v1, v2;
		ss >> v1 >> v2;

		Vector *v1_ptr, *v2_ptr;

		double dot = 0;

		if (rnt.savedType (v1) == T_VECTOR && rnt.savedType (v2) == T_VECTOR) {
			v1_ptr = (Vector *)rnt.obj_map[v1];
			v2_ptr = (Vector *)rnt.obj_map[v2];

		}
		else {

			*v1_ptr = Vector ();
			*v2_ptr = Vector ();
		}

		dot = (*v1_ptr) * (*v2_ptr);

		Value<double> *value_ptr = new Value<double> (dot);

		return Action ("dot", value_ptr, del);
	}
	else if (command == "det" || command == "determinant") {
		string alias;
		ss >> alias;

		Matrix *m = nullptr;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		const double det = Matrix::determinant (m);
		Value<double> *det_ptr = new Value<double> (det);

		return Action ("determinant", det_ptr, del);

	}
	else if (command == "basis") {
		string alias;
		ss >> alias;

		Matrix *m;

		if (rnt.savedType (alias) == T_MATRIX) {
			m = (Matrix *)rnt.obj_map[alias];
		}
		else {
			*m = Matrix ();
		}

		Matrix *reduced = m->gaussianReducedForm ();

		int min_dim = min (m->rows, m->cols);

		int *basisV = new int[min_dim];

		for (int i = 0; i < min_dim; i++) {
			basisV[i] = 0;
		}

		for (int i = 0; i < min_dim; i++) {
			if ((*reduced)[i][i] != 0) {
				basisV[i] = 1;
			}
		}

		cout << "-> Basis Vectors for the Row Space [Read Horizontal]: " << std::endl;

		for (int i = 0; i < min_dim; i++) {
			if (basisV[i]) {
				(*reduced)[i].print ();
			}
		}

		cout << "-> Basis Vectors for the Column Space [Read Vertical]: " << std::endl;

		for (int i = 0; i < min_dim; i++) {
			if (basisV[i]) {
				(*m)[i].print ();
			}
		}

		delete[] reduced;
		delete[] basisV;

		return Action ("basis", 0);

	}
	else if (command == "help" || command == "?") {
		help ();
		return Action ("help", 0);
	}
	else if (command == "exit" || command == "quit") {
		return Action ("exit", 0);
	}
	else {
		cout << "-> Command " << cmd << " not recognized. Type \"help\" for help." << std::endl;
		return Action ("undef", 0);
	}
}
*/
