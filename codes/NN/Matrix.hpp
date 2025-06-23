#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <cmath>

// ----------- Vector Class -----------
class Vector {
public:
    int size;
    double *data;

    // Default constructor (needed for 'new Vector[N]')
    Vector(): size(0), data(nullptr) {}

    // Construct a vector of length s, initialized to 0.0
    Vector(int s): size(s), data(new double[s]()) {
        if (s <= 0) throw std::invalid_argument("Vector size must be positive");
    }

    // Copy constructor: deep copy
    Vector(const Vector &other): size(other.size),
                                 data(new double[other.size]) {
        for (int i = 0; i < size; ++i)
            data[i] = other.data[i];
    }

    // Copy assignment: deep copy with self-assign guard
    Vector& operator=(const Vector &other) {
        if (this == &other) return *this;
        delete[] data;
        size = other.size;
        data = new double[size];
        for (int i = 0; i < size; ++i)
            data[i] = other.data[i];
        return *this;
    }

    // Destructor: free memory
    ~Vector() {
        delete[] data;
    }

    // Element access (with bounds checking)
    double& operator()(int i) {
        if (i < 0 || i >= size) throw std::out_of_range("Vector index out of range");
        return data[i];
    }
    double operator()(int i) const {
        if (i < 0 || i >= size) throw std::out_of_range("Vector index out of range");
        return data[i];
    }

    // Dot product
    double dot(const Vector &B) const {
        if (size != B.size) throw std::invalid_argument("Vector size mismatch in dot");
        double sum = 0.0;
        for (int i = 0; i < size; ++i)
            sum += data[i] * B.data[i];
        return sum;
    }

    // Norm (Euclidean)
    double norm() const {
        double s = 0.0;
        for (int i = 0; i < size; ++i)
            s += data[i] * data[i];
        return std::sqrt(s);
    }

    // Normalize
    Vector normalize() const {
        double n = norm();
        if (n == 0.0) throw std::runtime_error("Cannot normalize zero vector");
        Vector R(size);
        for (int i = 0; i < size; ++i)
            R.data[i] = data[i] / n;
        return R;
    }

    // Print with 4 decimals
    void print() const {
        std::cout << "[ ";
        for (int i = 0; i < size; ++i)
            std::cout << std::fixed << std::setprecision(4) << data[i] << " ";
        std::cout << "]\n";
    }
};

// ----------- Matrix Class -----------
class Matrix {
public:
    int rows, cols;
    Vector **colData;   // column-major: array of pointers to Vectors

    // Construct a rows×cols matrix of zeros
    Matrix(int r, int c): rows(r), cols(c), colData(new Vector*[c]) {
        if (r <= 0 || c <= 0) throw std::invalid_argument("Matrix size must be positive");
        for (int j = 0; j < cols; ++j)
            colData[j] = new Vector(rows);
    }

    // Copy constructor: deep copy
    Matrix(const Matrix &M): rows(M.rows), cols(M.cols),
                             colData(new Vector*[M.cols]) {
        for (int j = 0; j < cols; ++j)
            colData[j] = new Vector(*M.colData[j]);
    }

    // Copy assignment: deep copy with cleanup
    Matrix& operator=(const Matrix &M) {
        if (this == &M) return *this;
        for (int j = 0; j < cols; ++j) delete colData[j];
        delete[] colData;
        rows = M.rows;  cols = M.cols;
        colData = new Vector*[cols];
        for (int j = 0; j < cols; ++j)
            colData[j] = new Vector(*M.colData[j]);
        return *this;
    }

    // Destructor
    ~Matrix() {
        for (int j = 0; j < cols; ++j) delete colData[j];
        delete[] colData;
    }

    // Element access (i=row, j=col)
    double& operator()(int i, int j) {
        if (i<0||i>=rows||j<0||j>=cols) throw std::out_of_range("Matrix index out of range");
        return colData[j]->data[i];
    }
    double operator()(int i, int j) const {
        if (i<0||i>=rows||j<0||j>=cols) throw std::out_of_range("Matrix index out of range");
        return colData[j]->data[i];
    }

    // Matrix multiplication
    Matrix operator*(const Matrix &B) const {
        if (cols != B.rows) throw std::invalid_argument("Inner dims must match");
        Matrix C(rows, B.cols);
        for (int j = 0; j < B.cols; ++j)
            for (int i = 0; i < rows; ++i) {
                double sum = 0.0;
                for (int k = 0; k < cols; ++k)
                    sum += colData[k]->data[i] * B.colData[j]->data[k];
                C.colData[j]->data[i] = sum;
            }
        return C;
    }

    // Scalar multiply: A * k
    Matrix operator*(double k) const {
        Matrix C(rows, cols);
        for (int j = 0; j < cols; ++j)
            for (int i = 0; i < rows; ++i)
                C.colData[j]->data[i] = colData[j]->data[i] * k;
        return C;
    }
    // Scalar multiply: k * A
    friend Matrix operator*(double k, const Matrix &A) {
        return A * k;
    }

    // Print
    void print() const {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j)
                std::cout << std::fixed << std::setprecision(4)
                          << colData[j]->data[i] << " ";
            std::cout << "\n";
        }
    }
};

#endif // MATRIX_HPP
