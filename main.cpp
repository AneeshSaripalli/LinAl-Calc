#include <stdlib.h>
#include <sstream>
#include <map>
#include <typeinfo>

#include "include/Object.h"
#include "include/Matrix.h"


using namespace std;


struct Action { // Simple class to store an obj_ptr and name of the action
    string name;
    Object *obj_ptr;

    bool del;

    Action(string name, Object *obj_ptr) {
        this->name = name;
        this->obj_ptr = obj_ptr;

        del = false;
    }

    Action(string name, Object *object, bool del) {
        this->name = name;
        this->obj_ptr = object;
        this->del = del;
    }

    ~Action() {
        if (del) {
            if (obj_ptr != 0) {
                if (obj_ptr->name == "matrix") {
                    Matrix *m_ptr = (Matrix *) obj_ptr;

                    delete m_ptr;

                } else if (obj_ptr->name == "vector") {
                    Vector *v_ptr = (Vector *) obj_ptr;

                    delete v_ptr;
                }

            }
        }
    }
};


template<class Key, class Value>
bool keyInMap(map<Key, Value> &m, Key key); // Returns whether or not a key exists in a map

string savedType(string &s); // Returns the saved type <matrix/vector> of an object if it exists in the map, else null

bool contains(string findIn, string toFind); // Returns whether or not a substring exists within a string

Action process(string &cmd, bool del = false); // Conglomerate of all string parsing

void store_In_Object_Map(string key, Object *obj_ptr); // Stores a key: string, value:obj_ptr pair in the obj_map map

map<string, Object *> obj_map;


/*
* Ax = b -> has no solution
* A^TAx = A^Tb may have solution
*/
Vector *solveLeastSquares(Matrix &a, Vector &b) {
    Matrix *a_trans = a.transpose(); // Finds A^T

    Matrix *at_a = *a_trans * a; // Calculates A^T * A

    Vector *at_b = *a_trans * b; // Calculates A^T * b

    at_a->appendColumn(*at_b); // Appends A^T * b onto the Matrix for Gaussian Elimination

    at_a->gaussianReduce(); // Does Gaussian Elimination on the Matrix

    Vector *sol = at_a->solveUpperTriangular(); // Does Upper Triangular Substitution

    Vector *a_x = a * (*sol); // Calculates Ax, which is the project of b onto the column space of A

    double sigma = 0; // Variable to store the sum of differences squared over expected

    Vector *error = b - *a_x;

    for (int i = 0; i < error->length; i++) {
        sigma += pow((*error)[i], 2) / b[i];
    }

    cout << "-> Sum of residuals: " << sigma << endl;
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


void help() { // Console output to help direct users
    cout << "Functional Commands: Any data can be given with the name of the object or inputted after command call." << endl;
    cout << "\tEx: define matrix a ... \nrank a is equivalent to rank ... followed by input." << endl;
    cout << "-- define || def --> Alias either a matrix or a vector with a name to use later on." << endl;
    cout << "-- assign --> Alias the result of some command with a name.\n\t Ex: \"assign b inverse a\" assigns b the matrix value of a inverse." << endl;
    cout << "-- approx_sol || as --> Least squares solution to Ax = B." << endl;
    cout << "-- least_squares || ls --> Accepts data for least squares regression." << endl;
    cout << "-- rank --> Returns rank of a matrix." << endl;
    cout << "-- inverse || inv --> Returns the inverse of the matrix if it exists." << endl;
    cout << "-- dot --> Returns dot product of two vectors." << endl;
    cout << "-- determinant || det --> Returns the determinant of the matrix." << endl;
    cout << "-- basis --> Returns the linearly independent basis of both the column and row spaces." << endl;
    cout << "-- clear || cls --> Clears the command line prompt screen." << endl;
    cout << "-- exit --> Exits the command line application." << endl;
    cout << "-- help --> Prompts these options again." << endl;
}

void store_In_Object_Map(string key, Object *obj_ptr) {
    if (keyInMap(obj_map, key)) {

        Object *obj_ptr_old = obj_map[key];

        if (obj_ptr->name == "matrix") { delete (Matrix *) obj_ptr_old; }
        else if (obj_ptr->name == "vector") { delete (Vector *) obj_ptr_old; }

        obj_map[key] = obj_ptr;

    } else {
        obj_map.insert(std::make_pair(key, obj_ptr));
    }
}

string savedType(string &s) {
    return keyInMap(obj_map, s) ? obj_map[s]->name : "null";
}

void trim(string &s) { // Function to trim off leading or trailing spaces
    for (unsigned int i = 0; i < s.length(); i++) {
        if (s[i] == ' ') {
            s.erase(i, 1);
        } else {
            break;
        }

        i--;
    }

    for (unsigned i = s.length() - 1; i >= 0; i--) {
        if (s[i] == ' ') {
            s.erase(i, 1);
        } else {
            break;
        }
    }
}

bool contains(string findIn, string toFind) {
    size_t found = findIn.find(toFind);

    if (found != string::npos)
        return true;

    return false;
}

int pow(int base, int power) { // Simple utility function to evaluate powers
    int result = 1;
    for (int i = 0; i < power; i++) {
        result *= base;
    }

    return result;
}

void entryMessage() {
    cout << "       ----- Linear Algebra -----" << endl;
}

template<class Key, class Value>
bool keyInMap(map<Key, Value> &m, Key key) {
    auto pos = m.find(key);
    if (pos == m.end())
        return false;
    else
        return true;
}

Matrix *matrix_Operations(Matrix *m1, Matrix *m2, string oper) {
    if (oper == "*") { return *m1 * *m2; }
    else if (oper == "+") { return *m1 + *m2; }
    else if (oper == "-") { return *m1 - *m2; }
    else {
        cout << "Returning 0 as matrix pointer" << endl;
        return 0;
    }
}

Vector *matrix_vector_Operations(Matrix *m1, Vector *v1, string oper) {
    if (oper == "*") { return (*m1) * (*v1); }
    else { return 0; }
}


Action process(string &cmd, bool del) {
    std::stringstream ss;
    ss << cmd;

    string command;
    ss >> command;


    if (command == "define" || command == "def") {
        string type, alias;
        ss >> type >> alias;

        if (alias == "" || type == "") {
            cout << "-> Data can not be empty strings. Type \"help\" for help." << endl;
            return Action(string("define"), 0);
        }

        Object *newObj;

        if (type == "matrix") {
            newObj = new Matrix();
        } else if (type == "vector") {
            newObj = new Vector();
        } else {
            cout << "-> Data format not recognized.\n define <matrix/vector> <name>" << endl;
            return Action(string("define"), 0);
        }

        store_In_Object_Map(alias, newObj);

        return Action(string("define"), newObj);

    } else if (command == "recall" || command == "print") {
        string alias;
        ss >> alias;

        string type = savedType(alias);

        if (type != "null") {
            obj_map[alias]->print();
        } else {
            cout << "-> Variable \"" << alias << "\" not defined during this session." << endl;
        }

        return Action("recall", 0);
    } else if (command == "assign") {
        string alias;
        ss >> alias;

        int start_pos = cmd.find(alias, 6);
        int end_pos = start_pos + alias.length();

        string rest = cmd.substr(end_pos);
        trim(rest);


        Action result = process(rest, false);

        Object *data_ptr = result.obj_ptr;

        if (data_ptr == 0) {
            cout << "-> Assignment function does not have a return type." << endl;
        } else {
            store_In_Object_Map(alias, data_ptr);
        }


        return Action("assign", 0);

    } else if (command == "delete" || command == "del") {
        string alias;
        ss >> alias;

        if (keyInMap(obj_map, alias)) {
            Object *obj_ptr = obj_map[alias];

            if (obj_ptr->name == "matrix") {
                delete (Matrix *) obj_ptr;
            } else if (obj_ptr->name == "vector") {
                delete (Vector *) obj_ptr;
            } else {
                cout << "-> Object with the name " << alias << " has not been initialized in this run time." << endl;
                return Action("delete", 0);
            }

            obj_map.erase(alias);

            return Action("delete", 0);
        }

    } else if (command == "calculate" || command == "calc") {
        string first, oper, second;
        ss >> first;
        ss >> oper;
        ss >> second;

        if (keyInMap(obj_map, first) || keyInMap(obj_map, second)) {

            Object *obj_ptr1 = obj_map[first];
            Object *obj_ptr2 = obj_map[second];

            Object *returnObj = 0;

            if (obj_ptr1->name == "matrix" && obj_ptr2->name == "matrix") {
                returnObj = matrix_Operations((Matrix *) obj_ptr1, (Matrix *) obj_ptr2, oper);

                if(returnObj == 0)
                {
                    cout << oper << " is not an acceptable operation between two matrices." << endl;
                    return Action("calc mmult", 0);
                }

                returnObj->name = "matrix";

                return Action("calc mmult", returnObj, del);

            } else if (obj_ptr1->name == "matrix" && obj_ptr2->name == "vector") {
                returnObj = matrix_vector_Operations(obj_ptr1->get(Matrix::m_null), obj_ptr2->get(Vector::v_null), oper);
                returnObj->name = "vector";

                return Action("calc mvmult", returnObj, del);

            } else if (obj_ptr1->name == "vector" && obj_ptr2->name == "vector") {
                string processString = "dot " + first + " " + second;
                process(processString, true);
                return Action("calc dot", 0);
            }
        }

    } else if (command == "approx_sol" || command == "as") {
        string mat_a;
        string vec_b;

        ss >> mat_a >> vec_b;

        Matrix *m_ptr = 0;
        Vector *v_ptr = 0;

        if (savedType(mat_a) == "matrix" && savedType(vec_b) == "vector") {
            m_ptr = (Matrix *) obj_map[mat_a];
            v_ptr = (Vector *) obj_map[vec_b];

        } else {
            if (mat_a != "" || vec_b != "") {
                cout << "-> Data input format did not match the appropriate format." << endl;
            }

            *m_ptr = Matrix();
            *v_ptr = Vector();
        }

        Vector *v_sol = solveLeastSquares(*m_ptr, *v_ptr);

        return Action("approx_sol", v_sol, del);

    } else if (command == "least_squares" || command == "ls") {
        int data_points;
        cout << "-> Enter the number of data points: ";
        cin >> data_points;

        if (data_points <= 1) {
            cout << "-> Enter at least 2 data points: " << endl;
            return Action("least_squares", 0);
        }

        int power;
        cout << "-> Enter the power to evaluate the data set at: ";
        cin >> power;

        if (power > 10) {
            cout << "-> Power is too great." << endl;
            return Action("least_squares", 0);
        }

        Matrix *data_mat = new Matrix(data_points, power + 1);
        Vector *y_vals = new Vector(data_points);


        for (int row = 0; row < data_points; row++) {
            int x, y;
            cout << "-> Enter x, y: ";
            cin >> x >> y;

            for (int col = power; col >= 0; col--) {
                (*data_mat)[row][power - col] = pow(x, col);
            }

            (*y_vals)[row] = y;
        }

        Vector *result = solveLeastSquares(*data_mat, *y_vals);

        return Action("least_squares", result, del);
    } else if (command == "rank") {
        string alias;
        ss >> alias;

        if (savedType(alias) == "matrix") {
            Matrix *m_ptr = (Matrix *) obj_map[alias];

            Matrix *reduced = m_ptr->gaussianReducedForm();

            cout << "-> The rank of the matrix is " << reduced->countPivots() << "." << endl;

            delete reduced;
        } else {
            if (alias != "") {
                cout << "-> A matrix with the name of " << alias << " was not found." << endl;
            }

            Matrix m = Matrix();

            m.gaussianReduce();

            cout << "-> The rank of the matrix is " << m.countPivots() << "." << endl;
        }

        return Action("rank", 0);
    } else if (command == "cls" || command == "clear") {
        cout << flush;
        system("CLS");
        entryMessage();

        return Action("CLS", 0);

    } else if (command == "inverse" || command == "inv") {
        string alias;
        ss >> alias;

        Matrix *m;

        if (savedType(alias) == "matrix") {
            m = (Matrix *) obj_map[alias];
        } else {
            *m = Matrix();
        }

        Matrix *inverse = m->inverse();

        if (inverse != 0) {
            return Action("inverse", inverse, del);
        }

        return Action("inverse", 0);
    } else if (command == "dot") {
        string v1, v2;
        ss >> v1 >> v2;

        Vector *v1_ptr, *v2_ptr;

        double dot = 0;

        if (savedType(v1) == "vector" && savedType(v2) == "vector") {
            v1_ptr = (Vector *) obj_map[v1];
            v2_ptr = (Vector *) obj_map[v2];

        } else {

            *v1_ptr = Vector();
            *v2_ptr = Vector();
        }

        dot = (*v1_ptr) * (*v2_ptr);

        cout << "-> The dot product of the two vectors is " << setprecision(5) << dot << endl;

        return Action("dot", 0);

    } else if (command == "det" || command == "determinant") {
        string alias;
        ss >> alias;

        Matrix *m;

        if (savedType(alias) == "matrix") {
            m = (Matrix *) obj_map[alias];
        } else {
            *m = Matrix();
        }

        cout << "-> The determinant of " << alias << " is " << Matrix::determinant(m) << endl;

        return Action("determinant", 0);

    } else if (command == "basis") {
        string alias;
        ss >> alias;

        Matrix *m;

        if (savedType(alias) == "matrix") {
            m = (Matrix *) obj_map[alias];
        } else {
            *m = Matrix();
        }

        Matrix *reduced = m->gaussianReducedForm();

        int min_dim = min(m->rows, m->cols);

        int *basisV = new int[min_dim];

        for (int i = 0; i < min_dim; i++) {
            basisV[i] = 0;
        }

        for (int i = 0; i < min_dim; i++) {
            if ((*reduced)[i][i] != 0) {
                basisV[i] = 1;
            }
        }

        cout << "-> Basis Vectors for the Row Space [Read Horizontal]: " << endl;

        for (int i = 0; i < min_dim; i++) {
            if (basisV[i]) {
                (*reduced)[i].print();
            }
        }

        cout << "-> Basis Vectors for the Column Space [Read Vertical]: " << endl;

        for (int i = 0; i < min_dim; i++) {
            if (basisV[i]) {
                (*m)[i].print();
            }
        }

        delete[] reduced;
        delete[] basisV;

        return Action("basis", 0);

    } else if (command == "help" || command == "?") {
        help();
        return Action("help", 0);
    } else if (command == "exit" || command == "quit") {
        return Action("exit", 0);
    } else {
        cout << "-> Command " << cmd << " not recognized. Type \"help\" for help." << endl;
        return Action("undef", 0);
    }
}

int main() {
    entryMessage();

    string cmd = "";

    do {
        cout << "? "; // Prompts input from user
        getline(cin, cmd); // Accepts input from user

        trim(cmd);

        if (cmd == "") {
            continue;
        }

        Action action = process(cmd);

        Object *obj_ptr = action.obj_ptr;

        if (obj_ptr == 0) {
            //cout << "Non-returning activity" << endl;
        } else {
            //cout << "Returning activity " << action.name << endl;

            obj_ptr->print();
        }

    } while (cmd != "exit");

    for (auto it = obj_map.begin(); it != obj_map.end(); it++) {
        delete it->second;
    }

    obj_map.clear();

    return 0;
}
