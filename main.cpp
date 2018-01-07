#include <string>
#include <stdlib.h>
#include <sstream>
#include <map>

#include "Matrix.h"


using namespace std;

template <class Key, class Value>
bool keyInMap(map<Key, Value>&m, Key key);
string typeof(string& s);
bool contains(string findIn, string toFind);
void process(string& cmd);

map<string, string> typeof_map;
map<string, Matrix*> matrix_map;
map<string, Vector*> vector_map;


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

    sol->print();


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
    cout << "-- define || def --> Alias either a matrix or a vector with a name to use later on." << endl;
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

string typeof(string& s)
{
    return keyInMap(typeof_map, s) ? typeof_map[s] : "null";
}

void trim(string& s)
{
    for(unsigned int i = 0; i < s.length(); i++)
    {
        if(s[i] == ' ')
        {
            s.erase(i, 1);
        }
        else
        {
            break;
        }

        i--;
    }

    for(int i = s.length() - 1; i >= 0; i--)
    {
        if(s[i] == ' ')
        {
            s.erase(i, 1);
        }
        else
        {
            break;
        }
    }
}

bool contains(string findIn, string toFind)
{
    size_t found = findIn.find(toFind);

    if(found != string::npos)
        return true;

    return false;
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

template <class Key, class Value>
bool keyInMap(map<Key, Value> &m, Key key)
{
    auto pos = m.find(key);
    if(pos == m.end())
        return false;
    else
        return true;
}


void process(string& cmd)
{
    std::stringstream ss;
    ss << cmd;

    string command;
    ss >> command;


    if(command == "define" || command == "def")
    {
        string type, alias;
        ss >> type >> alias;

        if(alias == "" || type == "")
        {
            cout << "Data can not be empty strings. Type \"help\" for help." << endl;
            return;
        }

        if(type == "matrix")
        {
            Matrix* mat = new Matrix(); // Creates new user defined matrix

            string type = typeof(alias); // Gets the type of the alias

            if(type == "matrix") // If the entry was a matrix, it deletes matrix pointer
            {
                cout << alias << " was defined as a matrix. Deleting old pointer." << endl;
                delete matrix_map[alias];

                matrix_map[alias] = mat; // Sets the matrix pointer to the new matrix
                typeof_map[alias] = "matrix"; // Sets the data type to matrix
            }
            else if(type == "vector") // If the entry was a vector, it deletes the vector pointer
            {
                cout << alias << " was defined as a vector. Deleting old pointer." << endl;
                delete vector_map[alias];

                matrix_map[alias] = mat; // Sets the matrix pointer to the new matrix
                typeof_map[alias] = "matrix"; // Sets the data type to matrix
            }
            else if(type == "null")   // If the alias is new and an object with the name does not exist
            {
                cout << "Creating new matrix with type " << alias << endl;
                matrix_map.insert(std::make_pair(alias, mat));  // Links key alias and matrix pointer mat
                typeof_map.insert(std::make_pair(alias, "matrix")); // Links key alias and type "matrix"
            }
        }
        else if(type == "vector")
        {
            Vector* vec = new Vector(); // Creates new user defined vector

            string type = typeof(alias);

            if(type == "matrix") // If the entry was a matrix deletes matrix pointer
            {
                cout << alias << " was defined as a matrix. Deleting old pointer." << endl;
                delete matrix_map[alias];

                vector_map[alias] = vec; // Sets the vector pointer to the new vector
                typeof_map[alias] = "vector"; // Sets the data type to vector
            }
            else if(type == "vector") // If the entry was a vector deletes the vector pointer
            {
                cout << alias << " was defined as vector. Deleting old pointer." << endl;
                delete vector_map[alias];

                vector_map[alias] = vec; // Sets the vector pointer to the new vector
                typeof_map[alias] = "vector"; // Sets the data type to vector
            }
            else if(type == "null") // If the alias is new
            {
                vector_map.insert(std::make_pair(alias, vec));  // Links key alias and vector pointer mat
                typeof_map.insert(std::make_pair(alias, "vector")); // Links key alias and type "vector"
            }
        }
        else
        {
            cout << "Data format not recognized.\n define <matrix/vector> <name>" << endl;
            return;
        }
    }
    else if(command == "recall" || command == "print")
    {
        string alias;
        ss >> alias;

        string type = typeof(alias);

        if(type == "matrix" )
        {
            Matrix* mat = matrix_map[alias];
            mat->print();
        }
        else if(type == "vector")
        {
            Vector* vec = vector_map[alias];
            vec->print();
        }
        else if(type == "null")
        {
            cout << "Variable " << alias << " not defined during this session." << endl;
            return;
        }
    }
    else if(command == "approx_sol" || command == "as")
    {
        string mat_a;
        string vec_b;

        ss >> mat_a >> vec_b;

        if(typeof(mat_a) == "matrix" && typeof(vec_b) == "vector")
        {
            Matrix* m_ptr = matrix_map[mat_a];
            Vector* v_ptr = vector_map[vec_b];

            solveLeastSquares(*m_ptr, *v_ptr);
        }
        else
        {
            if(mat_a != "" || vec_b != "")
            {
                cout << "Data input format did not match the appropriate format." << endl;
            }

            Matrix a = Matrix();
            Vector b = Vector();

            solveLeastSquares(a, b);
        }
    }
    else if(command == "least_squares" || command == "ls")
    {
        int data_points;
        cout << "Enter the number of data points: ";
        cin >> data_points;

        if(data_points <= 1)
        {
            cout << "Enter at least 2 data points: " << endl;
            return;
        }

        int power;
        cout << "Enter the power to evaluate the plot at: ";
        cin >> power;

        if(power > 10)
        {
            cout << "Power is too great." << endl;
            return;
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
    else if(command == "rank")
    {
        string alias;
        ss >> alias;

        if(typeof(alias) == "matrix")
        {
            Matrix* m_ptr = matrix_map[alias];

            Matrix* reduced = m_ptr->gaussianReducedForm();

            cout << "The rank of the matrix is " << reduced->countPivots() << "." << endl;

            delete reduced;
        }
        else
        {
            if(alias != "")
            {
                cout << "A matrix with the name of " << alias << " was not found." << endl;
            }

            Matrix m = Matrix();

            m.gaussianReduce();

            cout << "The rank of the matrix is " << m.countPivots() << "." << endl;
        }
    }
    else if(command == "cls" || command == "clear")
    {
        cout << flush;
        system("CLS");
        entryMessage();
    }
    else if(command == "inverse" || command == "inv")
    {
        string alias;
        ss >> alias;

        Matrix* m;

        if(typeof(alias) == "matrix")
        {
            m = matrix_map[alias];
        }
        else
        {
            *m = Matrix();
        }

        Matrix* inverse = m->inverse();

        if(inverse != 0)
        {
            inverse->print();

            delete inverse;
        }
    }
    else if(command == "dot")
    {
        string v1, v2;
        ss >> v1 >> v2;

        Vector *v1_ptr, *v2_ptr;

        double dot = 0;

        if(typeof(v1) == "vector" && typeof(v2) == "vector")
        {
            v1_ptr = vector_map[v1];
            v2_ptr = vector_map[v2];

        }
        else
        {

            *v1_ptr = Vector();
            *v2_ptr = Vector();
        }

        dot = (*v1_ptr) * (*v2_ptr);

        cout << "The dot product of the two vectors is " << setprecision(5) << dot << endl;
    }
    else if(command == "det" || command == "determinant")
    {
        string alias;
        ss >> alias;

        Matrix* m;

        if(typeof(alias) == "matrix")
        {
            m = matrix_map[alias];
        }
        else
        {
            *m = Matrix();
        }

        cout << "The determinant of " << alias << " is " << Matrix::determinant(m) << endl;
    }
    else if(command == "basis")
    {
        string alias;
        ss >> alias;

        Matrix* m;

        if(typeof(alias) == "matrix")
        {
            m = matrix_map[alias];
        }
        else
        {
            *m = Matrix();
        }

        Matrix* reduced = m->gaussianReducedForm();

        int min_dim = min(m->rows, m->cols);

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
                (*m)[i].print();
            }
        }

        delete reduced;
        delete basisV;
    }
    else if(command == "help" || command == "?")
    {
        help();
    }
    else if(command == "exit" || command == "quit")
    {
        return;
    }
    else
    {
        cout << "Command " << cmd << " not recognized. Type \"help\" for help." << endl;
    }

}

int main()
{
    entryMessage();

    string cmd = "";

    do
    {
        cout << "? "; // Prompts input from user
        getline(cin, cmd); // Accepts input from user

        trim(cmd);

        if(cmd == "")
        {
            continue;
        }

        process(cmd);
    }
    while (cmd != "exit");

    typeof_map.clear();

    for(auto it = matrix_map.begin(); it != matrix_map.end(); it++)
    {
        delete it->second;
    }

    matrix_map.clear();

    for(auto it = vector_map.begin(); it != vector_map.end(); it++)
    {
        delete it->second;
    }

    vector_map.clear();

    return 0;
}
