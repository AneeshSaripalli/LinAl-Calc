#ifndef VECTOR_H
#define VECTOR_H

#include "Object.h"

class Vector : public Object
{
public:
    static const Vector* v_null;

    int length;
    Vector(int cols, double* values);
    Vector(int cols);

    Vector();
    virtual ~Vector();

    double& operator[](int index);
    void scale(double scalar);
    void subtractVector(Vector& v);
    void add(double x);
    void append(Vector& a);
    void round_off();
    double evaluateAsPolynomial(double x);
    bool isZeroVector();
    Vector* unit_vector();

    Vector* operator+(Vector& other);
    Vector* operator-(Vector& other);
    Vector* operator*(double scalar);
    Vector* operator/(double scalar);
    double operator*(Vector& other);

    void print();
protected:

private:
    double* values;
};

#endif // VECTOR_H
