
#ifndef MATH_HELP_H
#define MATH_HELP_H

#include <iostream>

#include "Matrix.h"
#include "Vector.h"
/*
* Ax = b -> has no solution
* A^TAx = A^Tb may have solution
*/
Vector *solveLeastSquares (Matrix &a, Vector &b) {
	Matrix *a_trans = a.transpose (); // Finds A^T

	Matrix *at_a = *a_trans * a; // Calculates A^T * A

	Vector *at_b = *a_trans * b; // Calculates A^T * b

	at_a->appendColumn (*at_b); // Appends A^T * b onto the Matrix for Gaussian Elimination

	at_a->gaussianReduce (); // Does Gaussian Elimination on the Matrix

	Vector *sol = at_a->solveUpperTriangular (); // Does Upper Triangular Substitution

	Vector *a_x = a * (*sol); // Calculates Ax, which is the project of b onto the column space of A

	double sigma = 0; // Variable to store the sum of differences squared over expected

	Vector *error = b - *a_x;

	for (int i = 0; i < error->length; i++) {
		sigma += pow ((*error)[i], 2) / b[i];
	}

	std::cout << "-> Sum of residuals: " << sigma << std::endl;
	/*
	* Clears up all pointers
	*/

	delete a_trans;
	delete at_a;
	delete at_b;
	delete error;
	delete a_x;

	return sol;
}



Matrix *matrix_Operations (Matrix *m1, Matrix *m2, std::string oper) {
	if (oper == "*") { return *m1 * *m2; }
	else if (oper == "+") { return *m1 + *m2; }
	else if (oper == "-") { return *m1 - *m2; }
	else {
		std::cout << "Returning 0 as matrix pointer" << std::endl;
		return 0;
	}
}

Vector *matrix_vector_operations (Matrix *m1, Vector *v1, std::string oper) {
	if (oper == "*") { return (*m1) * (*v1); }
	else { return 0; }
}

Vector *vector_operations (Vector *v1, Vector *v2, std::string oper) {
	if (oper == "+") { return (*v1) + (*v2); }
	else if (oper == "-") { return (*v1) - (*v2); }
	else { return 0; }
}

// Simple utility function to evaluate powers
int pow (int base, int power) {
	int result = 1;
	for (int i = 0; i < power; i++) {
		result *= base;
	}

	return result;
}
#endif