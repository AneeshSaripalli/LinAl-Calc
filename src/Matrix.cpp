#include "../include/Matrix.h"

using namespace std;

Matrix::Matrix(int rows, int cols)
    : Object("matrix")
{
    this->rows = rows;
    this->cols = cols;

    matrix = new Vector*[rows];

    for(int i = 0; i < rows; i++)
    {
        Vector* newVectorPtr = new Vector(cols);

        matrix[i] = newVectorPtr;
    }
}

Matrix::Matrix()
    : Object("matrix")
{
    int rows, cols;

    cout << "Enter a row length: ";
    cin >> rows;

    cout << "Enter a col length: ";
    cin >> cols;

    cout << "Matrix has " << rows << " rows " << "and " << cols << " cols." << endl;

    this->rows = rows;
    this->cols = cols;

    matrix = new Vector*[rows];

    cout << "Enter values in left to right order by row up to down" << endl;

    int total = 0;

    cout << ">> ";

    for(int i = 0; i < rows; i++)
    {
        Vector* newVectorPtr = new Vector(cols);

        for(int j = 0; j < cols; j++)
        {
            total++;

            int val;
            cin >> val;

            char a;
            a = cin.get();

            if(total != rows * cols && a == '\n')
            {
                cout << ">> ";
            }

            (*newVectorPtr)[j] = val;
        }

        matrix[i] = newVectorPtr;
    }
}

double Matrix::determinant(Matrix* mat)
{
    if(mat->rows != mat->cols)
    {
        return 0;
    }

    Matrix* reduced = mat->gaussianReducedForm();

    int min_dim = min(reduced->rows, reduced->cols);

    double det = 1;

    for(int i = 0; i < min_dim; i++)
    {
        det *= (*reduced)[i][i];
    }

    delete reduced;

    return det;
}

Matrix* identity(int side)
{
    Matrix* iden = new Matrix(side, side);

    for(int r = 0; r < side; r++)
    {
        for(int c = 0; c < side; c++)
        {
            (*iden)[r][c] = (r == c) ? 1 : 0;
        }
    }

    return iden;
}

void Matrix::appendColumn(Vector& v)
{
    if(rows != v.length)
    {
        cout << "Can not append vector to matrix as column." << endl;
        return;
    }

    cols++;

    for(int i = 0; i < rows; i++)
    {
        (*matrix[i]).add(v[i]);
    }
}

int Matrix::countPivots()
{
    int min_dim = min(rows, cols);

    int count = 0;

    for(int i = 0; i < min_dim; i++)
    {
        if((*this)[i][i] != 0)
        {
            count++;
        }
    }

    return count;
}

Matrix* Matrix::submatrix(int tlR, int tlC, int rows, int cols)
{
    Matrix* submat = new Matrix(rows, cols);

    for(int r = tlR; r < (rows + tlR); r++)
    {
        for(int c = tlC; c < (cols + tlC); c++)
        {
            (*submat)[r - tlR][c - tlC] = (*this)[r][c];
        }
    }

    return submat;
}

/*
* Takes matrix A, appends I and reduces A->I which causes I->A^-1
*/
Matrix* Matrix::inverse()
{
    if(rows != cols)
    {
        cout << "Matrix rows and columns must be equal for an inverse to exist" << endl;
        return 0;
    }

    Matrix* appended = new Matrix(rows, cols);

    Matrix* iden = identity(rows);

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            (*appended)[r][c] = (*this)[r][c];
        }
    }

    appended->append(*iden);

    delete iden;

    appended->gaussianReduce();
    appended->solveUpperTriangular();

    Matrix* inverse = appended->submatrix(0, rows, rows, rows);

    delete appended;

    return inverse;
}

void Matrix::appendRow(Vector& v)
{
    if(cols != v.length)
    {
        cout << "Can not append vector to matrix as row." << endl;
        return;
    }

    Vector** new_matrix = new Vector*[rows + 1];

    for(int i = 0; i < rows; i++)
    {
        new_matrix[i] = matrix[i];
    }

    Vector* new_row = new Vector(cols);

    for(int i = 0; i < cols; i++)
    {
        (*new_row)[i] = v[i];
    }

    new_matrix[rows] = new_row;

    delete [] matrix;

    matrix = new_matrix;

    rows++;
}

Matrix* Matrix::transpose()
{
    Matrix* trans = new Matrix(cols, rows);

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            (*trans)[c][r] = (*this)[r][c];
        }
    }

    return trans;
}

Matrix* Matrix::add(Matrix& otherMatrix)
{
    if(!(this->rows == otherMatrix.rows && this->cols == otherMatrix.cols))
    {
        cout << "Addition undefined between these two matrices" << endl;
        return 0;
    }

    Matrix* result = new Matrix(rows, cols);

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            (*result)[r][c] = (*this)[r][c] + otherMatrix[r][c];
        }
    }

    return result;
}

Matrix* Matrix::subtract(Matrix& otherMatrix)
{
    if(!(this->rows == otherMatrix.rows && this->cols == otherMatrix.cols))
    {
        cout << "Subtraction undefined between these two matrices" << endl;
        return 0;
    }

    Matrix* result = new Matrix(rows, cols);

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            result[r][c] = (*this)[r][c] - otherMatrix[r][c];
        }
    }

    return result;
}


Matrix* Matrix::multiply(Matrix& otherMatrix)
{
    if(this->cols != otherMatrix.rows) // Checks if the columns of the left matrix are the same as the rows of the right
    {
        cout << "Multiplication undefined between these two matrices" << endl;
        return 0; // Returns a nullptr
    }

    Matrix* result = new Matrix(rows, otherMatrix.cols); // Creates a new matrix with dimensions rows and columns

    for(int rowCount = 0; rowCount < rows; rowCount++) // Loops through the rows in the left matrix
    {
        for(int colCount = 0; colCount < otherMatrix.cols; colCount++) // Loops through the columns in the right matrix
        {
            int sum = 0; // Creates a cumulative sum variable to add to

            for(int i = 0; i < cols; i++) // Loops through indices in the row and column
            {
                int a = (*this)[rowCount][i];
                int b = otherMatrix[i][colCount];

                sum += a * b; // Adds the product of the two values to the sum
            }

            (*result)[rowCount][colCount] = sum; // Sets the index in the result matrix to the total sum
        }
    }

    return result; // Returns the new product matrix
}

Vector& Matrix::operator[](int index)
{
    if(index > rows)
    {
        cout << "Index " << index << " exceeds row count " << rows << "." << endl;
    }

    return *(matrix[index]);
}

Matrix* Matrix::operator *(Matrix& other)
{
    return multiply(other);
}

Vector* Matrix::operator *(Vector& v)
{
    if(cols != v.length)
    {
        cout << "Can not multiply this matrix and vector." << endl;
        return 0;
    }

    Vector* result = new Vector(rows);

    for(int r = 0; r < rows; r++)
    {
        double dot_product = (*matrix[r]) * v;

        (*result)[r] = dot_product;
    }

    return result;
}

Matrix* Matrix::operator +(Matrix& other)
{
    return add(other);
}

Matrix* Matrix::operator -(Matrix& other)
{
    return subtract(other);
}

void Matrix::append(Matrix& a)
{
    if(rows != a.rows)
    {
        cout << "Cannot append these two matrices" << endl;
        return;
    }

    for(int i = 0; i < rows; i++)
    {
        matrix[i]->append(a[i]);
    }

    cols += a.cols;
}

bool Matrix::allDiagonalsNonzero()
{
    if(cols != rows)
        return false;

    for(int i = 0; i < rows; i++)
    {
        if((*matrix[i])[i] == 0)
            return false;
    }

    return true;
}

Vector* Matrix::solveUpperTriangular()
{
    if(allDiagonalsNonzero() && (rows == cols))
    {
        cout << "Matrix is full rank and can not be solved." << endl;
        cout << "Rows " << rows << " cols " << cols << endl;
        return 0;
    }

    bool hasNoSol = false, hasInfiSol = false;

    for(int i = rows - 1; i >= 0; i--)
    {
        Vector* row = matrix[i];

        if(row->isZeroVector())
        {
            hasInfiSol = true;
            continue;
        }

        double scale = (*row)[i];

        if(scale == 0)
        {
            hasNoSol = true;
            continue;
        }

        row->scale(1.0 / scale);

        for(int j = i - 1; j >= 0; j--)
        {
            Vector* reduc = matrix[j];

            double reducVal = (*reduc)[i];

            reduc->subtractVector(*((*row) *  reducVal));
        }
    }

    if(hasNoSol)
    {
        cout << "Matrix has no solution." << endl;
    }
    else if(hasInfiSol)
    {
        cout << "Matrix has infinite solutions." << endl;
    }

    round_off();

    Vector* values = new Vector(rows);

    for(int i = 0; i < rows; i++)
    {
        (*values)[i] = (*this)[i][cols - 1];
    }

    return values;
}

Matrix* Matrix::gaussianReducedForm()
{
    Matrix* cpy = new Matrix(rows, cols);

    for(int r = 0; r < rows; r++)
    {
        for(int c = 0; c < cols; c++)
        {
            (*cpy)[r][c] = (*this)[r][c];
        }
    }

    cpy->gaussianReduce();

    return cpy;
}

void Matrix::round_off()
{
    for(int i = 0; i < rows; i++)
    {
        matrix[i]->round_off();
    }
}

void Matrix::gaussianReduce()
{
    bool fullRank = true;

    for(int i = 0; i < rows - 1; i++)
    {
        Vector* pivotVector = matrix[i];

        for(int j = i + 1; j < rows; j++)
        {
            Vector* row = matrix[j];

            double rowValue = (*row)[i], pivotValue = (*pivotVector)[i];

            if(pivotValue == 0)
            {
                fullRank = false;
                continue;
            }

            double scale = (double) rowValue / pivotValue;

            Vector* scaled = (*pivotVector) * scale;

            row->subtractVector(*scaled);

            delete scaled;
        }
    }

    if(!fullRank)
    {
        cout << "Matrix was not full rank. There was atleast one 0 pivot." << endl;
    }
}

void Matrix::setValue(int value, int row, int col)
{
    if(row > rows || col > cols) // Checks if the row and col exist in the matrix
    {
        cout << "Out of bounds" << endl;
        return;
    }

    (*(matrix[row]))[col] = value;
}


void Matrix::print()
{
    for(int i = 0; i < rows; i++) // Loops through each of the rows
    {
        (matrix[i])->print(); // Calls print for each of the vectors (rows)
    }
    cout << endl;
}

int Matrix::valueAt(int row, int col)
{
    if(row > rows || col > cols)
    {
        cout << "Invalid element" << endl;
        return 0;
    }

    return (*(matrix[row]))[col];
}

Matrix::~Matrix()
{
    delete [] matrix;
}
