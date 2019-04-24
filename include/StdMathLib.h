#include <sstream>
#include <iostream>
#include <vector>

#include "Action.h"
#include "Runtime.h"
#include "InputUtils.h"
#include "Defines.h"

#define min(x,y) (x < y ? x : y);

#ifndef STDMATHLIB_H
#define STDMATHLIB_H

namespace stdlib {
	Response define{
		[] (std::stringstream& ss, Runtime& rnt, bool del = false)
		{
			auto decs = InputUtils::pop_from_stream (ss, 2);

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

			rnt.store_in_object_map (alias, newObj);

			return Action (string ("define"), newObj, false);
		}
	};
	Response memory{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
			for (auto it = rnt.obj_map.begin (); it != rnt.obj_map.end (); it++) {
				cout << "-------------------------" << std::endl;
				cout << "Object " << it->first << " - " << Object::typeAsString (rnt.saved_type (it->first)) << std::endl;
				it->second->print ();
				cout << "-------------------------" << std::endl;
			}

			return Action ("memory", 0);
		}
	};
	Response delete_from_runtime{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
		auto aliasVec = InputUtils::pop_from_stream (ss, 1);
			std::string alias = aliasVec[0];

			if (rnt.key_in_map (rnt.obj_map, alias)) {
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
	Response default_action{
	[] (std::stringstream& ss, Runtime& rnt, bool del = false) -> Action
	{

		std::string cmd = ss.str ();

		if (rnt.saved_type (cmd) != T_NULL)
		{
			return Action (string ("default interp"), rnt.obj_map[cmd]);
		}
		else {
			std::cout << "-> Command " << cmd << " not recognized. Type \"help\" for help." << std::endl;
			return Action ("undef", 0);
		}
	}
	};
	Response assign{
		[] (std::stringstream& ss, Runtime& rnt, bool del = false)
		{
			std::string cmd = ss.str ();

			auto aliasVec = InputUtils::pop_from_stream (ss, 1);
			std::string alias = aliasVec[0];

			int start_pos = cmd.find (alias, 6);
			int end_pos = start_pos + alias.length ();

			std::string rest = cmd.substr (end_pos);
			InputUtils::trim (rest);

			Action result = rnt.parser->process (rest, rnt, false);

			Object *data_ptr = result.obj_ptr;

			if (data_ptr == 0) {
				cout << "-> Assignment function does not have a return type." << std::endl;
			}
			else {
				rnt.store_in_object_map (alias, data_ptr);
			}

			return Action ("assign", 0);
		}
	};
	Response output{
	[] (std::stringstream& ss, Runtime& rnt, bool del = false) {
		auto aliasVec = InputUtils::pop_from_stream (ss, 1);
		std::string alias = aliasVec[0];

		int type = rnt.saved_type (alias);

		if (type != T_NULL) {
			return Action (string ("print"), rnt.obj_map[alias], false);
		}
		else {
			cout << "-> Variable \"" << alias << "\" not defined during this session." << std::endl;
		}

		return Action ("recall", 0);
	}
	};

	void init_map (Runtime& rnt)
	{
		rnt.parser->link (std::vector<std::string>{ "def", "define" }, define);
		rnt.parser->link (std::vector<std::string>{ "default" }, default_action);
		rnt.parser->link (std::vector<std::string>{ "delete", "del" }, delete_from_runtime);
		rnt.parser->link (std::vector<std::string>{ "mem", "memory" }, memory);
		rnt.parser->link (std::vector<std::string>{ "assign" }, assign);
		rnt.parser->link (std::vector<std::string>{ "print", "recall" }, output);

	}
}

namespace test {
	Response do_test{
	[] (std::stringstream& ss, Runtime& rnt, bool del)
	{
		std::cout << "Testing." << std::endl;
		return Action ("Test", 0);
	}
	};

	void init_map (Runtime& rnt)
	{
		rnt.parser->link (std::vector<std::string>{"test"}, do_test);
	}
}

namespace help_lib {


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

	void init_map (Runtime& rnt)
	{
		rnt.parser->link (std::vector<std::string>{"exit", "quit"}, do_exit);
		rnt.parser->link (std::vector<std::string>{"help", "?"}, do_help);
	}
};

namespace mathlib {


	Response projection{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
		auto vectors = InputUtils::pop_from_stream (ss, 2);
			bool wellTyped = InputUtils::ensure_type_validity (vectors, { T_VECTOR, T_VECTOR }, rnt);

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

	Response calculate{
		[] (std::stringstream& ss, Runtime& rnt, bool del)
		{
		auto expVec = InputUtils::pop_from_stream (ss, 3);
			string first = expVec[0], oper = expVec[1], second = expVec[2];

			if (rnt.key_in_map (rnt.obj_map, first) || rnt.key_in_map (rnt.obj_map, second)) {

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
						return rnt.parser->process (processString, rnt, del);
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

			if (rnt.saved_type (mat_a) == T_MATRIX && rnt.saved_type (vec_b) == T_VECTOR) {
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

			if (rnt.saved_type (alias) == T_MATRIX) {
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

			if (rnt.saved_type (alias) == T_MATRIX) {
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

			if (rnt.saved_type (alias) == T_MATRIX) {
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

			if (rnt.saved_type (v1) == T_VECTOR && rnt.saved_type (v2) == T_VECTOR) {
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

			if (rnt.saved_type (alias) == T_MATRIX) {
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



	void init_map (Runtime& rnt)
	{
		rnt.parser->link (std::vector<std::string>{ "proj", "projeciton" }, projection);
		rnt.parser->link (std::vector<std::string>{ "calculate", "calc" }, calculate);
		rnt.parser->link (std::vector<std::string>{ "approx_sol", "as" }, approx_sol);
		rnt.parser->link (std::vector<std::string>{ "least_squares", "ls" }, least_squares);
		rnt.parser->link (std::vector<std::string>{ "rank", "rank_of" }, solve_rank);
		rnt.parser->link (std::vector<std::string>{ "inv", "inverse" }, solve_inverse);
		rnt.parser->link (std::vector<std::string>{ "det", "determinant" }, determinant);
		rnt.parser->link (std::vector<std::string>{ "dot" }, dot);
		rnt.parser->link (std::vector<std::string>{"basis"}, solve_basis);
	}

}
#endif