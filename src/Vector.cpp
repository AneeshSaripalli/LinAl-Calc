#include "../include/Vector.h"

using namespace std;

Vector* null_vector = new Vector(0, 0);

Vector::Vector(int length, double* values)
    : Object("vector")
{
    this->length = length;
    this->values = new double[length];

    for(int i = 0; i < length; i++)
    {
        this->values[i] = values[i];
    }
}

Vector::Vector()
    : Object("vector")
{
    int length;
    cout << "Enter the length of the vector: ";
    cin >> length;

    this->length = length;

    values = new double[length];

    cout << ">> ";

    int total = 0;

    for(int i = 0; i < length; i++)
    {
        int val;
        cin >> val;

        values[i] = val;

        total++;

        char a;
        a = cin.get();


        if(total != length && a == '\n')
        {
            cout << ">> ";
        }
    }
}

double Vector::evaluateAsPolynomial(double x)
{
    double result = 0;

    for(int i = 0; i < length; i++)
    {
        int power = length - i - 1;

        double contribution = values[i] * pow(x, power);

        result += contribution;
    }

    return result;
}

void Vector::add(double x)
{
    double* newList = new double[length + 1];

    for(int i = 0; i < length; i++)
    {
        newList[i] = values[i];
    }

    newList[length] = x;

    length += 1;

    delete [] values;

    values = newList;
}

void Vector::append(Vector& a)
{
    for(int i = 0; i < a.length; i++)
    {
        add(a[i]);
    }
}

bool Vector::isZeroVector()
{
    for(int i = 0; i < length; i++)
    {
        if(values[i] != 0)
            return false;
    }

    return true;
}

void Vector::scale(double scalar)
{
    for(int i = 0; i < length; i++)
    {
        values[i] *= scalar;
    }
}

Vector::Vector(int length)
    : Object("vector")
{
    this->length = length;
    values = new double[length];

    if(values == NULL)
    {
        cout << "Memory not allocated for values" << endl;
    }
}

double& Vector::operator[](int index)
{
    return values[index];
}

void Vector::subtractVector(Vector &v)
{
    if(length != v.length)
    {
        cout << "These two vectors can not be subtracted." << endl;
        return;
    }

    for(int i = 0; i < length; i++)
    {
        (*this)[i] -= v[i];
    }
}

Vector* Vector::operator+(Vector& other)
{
    if(length != other.length)
    {
        cout << "These two vectors can not be added." << endl;
        return 0;
    }

    Vector* sum = new Vector(length);

    for(int i = 0; i < length; i++)
    {
        (*sum)[i] = other[i] + (*this)[i];
    }

    return sum;
}

Vector* Vector::operator-(Vector& other)
{
    if(length != other.length)
    {
        cout << "These two vectors can not be subtracted." << endl;
        return 0;
    }

    Vector* diff = new Vector(length);

    for(int i = 0; i < length; i++)
    {
        (*diff)[i] = (*this)[i] - other[i];
    }

    return diff;
}

Vector* Vector::operator / (double scalar)
{
    return (*this) * (1.0 / scalar);
}

double Vector::operator*(Vector& other)
{
    if(length != other.length)
    {
        cout << "These two vectors can not be dotted." << endl;
        return -1;
    }

    double sum = 0;

    for(int i = 0; i < length; i++)
    {
        sum += (*this)[i] * other[i];
    }

    return sum;
}

void Vector::round_off()
{
    for(int i = 0; i < length; i++)
    {
        double value = (*this)[i];

        value = abs(value);

        if(value < 1e-9)
        {
            (*this)[i] = 0;
        }
    }
}

Vector* Vector::unit_vector()
{
    double mag = 0;

    for(int i = 0; i < length; i++)
    {
        mag += values[i] * values[i];
    }

    Vector* scaled_down = (*this) / mag;

    return scaled_down;
}

Vector* Vector::operator*(double scalar)
{
    Vector* scale = new Vector(length);

    for(int i = 0; i < length; i++)
    {
        (*scale)[i] = scalar * (*this)[i];
    }

    return scale;
}

void Vector::print()
{
    cout << "[ ";
    for(int i = 0; i < length; i++)
    {
        cout << setw(10) << setprecision(5) << values[i] << " ";
    }
    cout << " ]" << endl;
}

Vector::~Vector()
{
    delete [] values;
}
