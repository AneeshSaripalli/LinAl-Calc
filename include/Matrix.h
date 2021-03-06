#ifndef MATRIX_H
#define MATRIX_H

#include "Vector.h"


class Matrix : public Object
{
public:
    static const Matrix* m_null ;

    int rows, cols;

    static double determinant(Matrix* mat);

    Matrix(int rows, int cols);
    Matrix();
    virtual ~Matrix();

    Matrix* multiply(Matrix& otherMatrix);
    Matrix* add(Matrix& otherMatrix);
    Matrix* subtract(Matrix& otherMatrix);
    Matrix* gaussianReducedForm();
    Matrix* transpose();
    Matrix* inverse();
    Matrix* submatrix(int tlR, int tlC, int rows, int cols);

    Vector* solveUpperTriangular();

	Matrix* Matrix::identity (int size);

    int valueAt(int row, int col);
    int countPivots();

    bool allDiagonalsNonzero();
    void setValue(int value, int row, int col);
    void print();
    void gaussianReduce();
    void append(Matrix& a);
    void appendColumn(Vector& v);
    void appendRow(Vector& v);
    void round_off();

    Vector& operator [](int index);
    Matrix* operator *(Matrix& other);
    Vector* operator *(Vector& v);
    Matrix* operator +(Matrix& other);
    Matrix* operator -(Matrix& other);

protected:

private:
    //int *values;
    Vector **matrix;
};

#endif // MATRIX_H
