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
    cout << "Functional Commands: " << endl;
    cout << "-- approx_sol || as --> Least squares solution to Ax = B." << endl;
    cout << "-- least_squares || ls --> Accepts data for least squares regression." << endl;
    cout << "-- rank --> Returns rank of a matrix" << endl;
    cout << "-- inverse || inv --> Returns the inverse of the matrix if it exists." << endl;
    cout << "-- dot --> Returns dot product of two vectors." << endl;
    cout << "-- determinant || det --> Returns the determinant of the matrix." << endl;
    cout << "-- basis --> Returns the linearly independent basis of both the column and row spaces." << endl;
    cout << "-- clear || cls --> Clears the command line prompt screen." << endl;
    cout << "-- exit --> Exits the command line application." << endl;
    cout << "-- help --> Prompts these options again." << endl;
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

void entryMessage()
{
    cout << "       ----- Linear Algebra -----" << endl;
}


int main()
{
    entryMessage();

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
            entryMessage();
        }
        else if(input == "inverse" || input == "inv")
        {
            Matrix m = Matrix();

            Matrix* inverse = m.inverse();

            if(inverse != 0)
            {
                inverse->print();

                delete inverse;
            }
        }
        else if(input == "dot")
        {
            Vector v1 = Vector();
            Vector v2 = Vector();

            double dot = v1 * v2;

            cout << "Vector 1 dot Vector 2 = " << setw(10) << setprecision(5) << dot << endl;
        }
        else if(input == "det" || input == "determinant")
        {
            Matrix m = Matrix();

            cout << "The determinant of the matrix is " << Matrix::determinant(&m) << "." << endl;
        }
        else if(input == "basis")
        {
            Matrix m = Matrix();

            Matrix* reduced = m.gaussianReducedForm();

            int min_dim = min(m.rows, m.cols);

            int* basisV = new int[min_dim];

            for(int i = 0; i < min_dim; i++)
            {
                basisV[i] = 0;
            }

            for(int i = 0; i < min_dim; i++)
            {
                if((*reduced)[i][i] != 0)
                {
                    basisV[i] = 1;
                }
            }

            cout << "Basis Vectors for the Row Space [Read Horizontal]: " << endl;

            for(int i = 0; i < min_dim; i++)
            {
                if(basisV[i])
                {
                    (*reduced)[i].print();
                }
            }

            cout << "Basis Vectors for the Column Space [Read Vertical]: " << endl;

            for(int i = 0; i < min_dim; i++)
            {
                if(basisV[i])
                {
                    m[i].print();
                }
            }

            delete reduced;
            delete basisV;
        }
        else if(input == "help" || input == "?")
        {
            help();
        }
        else
        {
            cout << "Command not recognized. Type \"help\" for help." << endl;
        }

    }
    while (input != "exit");
    return 0;
}
