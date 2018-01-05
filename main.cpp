#include <string>
#include <stdlib.h>
#include "Matrix.h"

using namespace std;

/*
* Ax = b -> has no solution
* A^TAx = A^Tb may have solution
*/
void solveLeastSquares(Matrix& a, Vector& b)
{
    Matrix* a_trans = a.transpose(); // Finds A^T

    Matrix* at_a = *a_trans * a; // Calculates A^T * A

    Vector* at_b = *a_trans * b; // Calculates A^T * b

    at_a->appendColumn(*at_b); // Appends A^T * b onto the Matrix for Gaussian Elimination

    at_a->gaussianReduce(); // Does Gaussian Elimination on the Matrix

    Vector* sol = at_a->solveUpperTriangular(); // Does Upper Triangular Substitution

    Vector* a_x = a * (*sol); // Calculates Ax, which is the project of b onto the column space of A

    double sigma = 0; // Variable to store the sum of differences squared over expected

    Vector* error = b - *a_x;

    for(int i = 0; i < error->length; i++)
    {
        sigma += pow((*error)[i], 2) / b[i];
    }


    cout << "Sum of residuals: " << sigma << endl;
    /*
    * Clears up all pointers
    */

    delete a_trans;
    delete at_a;
    delete at_b;
    delete sol;
    delete a_x;
    delete error;
}


void help()
{
    cout << "HELP IS ON ITS WAY" << endl;
}


/*
* Simple power utility function to evaluate powers
*/
int pow(int base, int power)
{
    int result = 1;
    for(int i = 0; i < power; i++)
    {
        result *= base;
    }

    return result;
}

int main()
{
    string input = "";

    do
    {
        cout << "Enter next command: "; // Prompts input from user
        cin >> input; // Accepts input from user

        if(input == "approx_sol" || input == "as")
        {
            Matrix a = Matrix();
            Vector b = Vector();

            solveLeastSquares(a, b);
        }
        else if(input == "least_squares" || input == "ls")
        {
            int data_points;
            cout << "Enter the number of data points: ";
            cin >> data_points;

            if(data_points <= 1)
            {
                cout << "Enter at least 2 data points: " << endl;
                continue;
            }

            int power;
            cout << "Enter the power to evaluate the plot at: ";
            cin >> power;

            if(power > 10)
            {
                cout << "Power is too great." << endl;
                continue;
            }

            Matrix* data_mat = new Matrix(data_points, power + 1);
            Vector* y_vals = new Vector(data_points);


            for(int row = 0; row < data_points; row++)
            {
                int x, y;
                cout << "Enter x and y: ";
                cin >> x >> y;

                for(int col = power; col >= 0; col--)
                {
                    (*data_mat)[row][power - col] = pow(x, col);
                }

                (*y_vals)[row] = y;
            }

            solveLeastSquares(*data_mat, *y_vals);
        }
        else if(input == "rank")
        {
            Matrix m = Matrix();

            m.gaussianReduce();

            cout << "The rank of the matrix is " << m.countPivots() << endl;
        }
        else if(input == "cls" || input == "clear")
        {
            cout << flush;
            system("CLS");
        }
        else if(input == "inverse" || input == "inv")
        {
            Matrix m = Matrix();

            m.inverse()->print();
        }
        else
        {
            help();
        }

    }
    while (input != "exit");
    return 0;
}
