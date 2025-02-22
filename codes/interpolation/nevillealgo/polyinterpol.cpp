// Returns a polynomial function of lowest order which passes through a given set of points

#include <iostream>
#include <cstdlib>
#include <functional>

using namespace std;

// Define a function pointer alias for functions which takes a double as input and gives out a double
typedef double (*InterpFunc)(double);

// A global function pointer to store the function pointer to the final polynomial function 
InterpFunc global_func = nullptr;

// Extern C ensure C linkage which prevents C++ name mangling making it callable from C or python without errors
extern "C" {

    // Function which outputs the required polynomial function by implementing Neville's Algorithm
    InterpFunc polyinterpol(int numpoints, double** points) {
        // Matrix to store functions p_(i,j)
        function<double(double)> funcmatrix[numpoints][numpoints];

        // Fill the function matrix using Neville's Algorithm and Lambda functions
        for (int i = 0; i < numpoints; i++) {
            // All the initial starting diagonal elements are the y values themself
            funcmatrix[i][i] = [i, points](double x) -> double {
                return points[i][1];  // f(xi)
            };
        }

        // Using the neville's formula to compute the higher order functions
        for (int d = 1; d < numpoints; d++) {
            // First p_(i,i) is used to calculate all p_(i,i+1) which are then used to calculate p_(i,i+2) and so till p_(i,d)
            for (int i = 0; i < numpoints - d; i++) {
                int j = i + d;
                // Auto fi is just an alias to a given function, used for more readability of code
                auto f1 = funcmatrix[i + 1][j];
                auto f2 = funcmatrix[i][j - 1];
                
                // Filling the functions in the matrix
                funcmatrix[i][j] = [i, j, points, f1, f2](double x) -> double {
                    return ((x - points[i][0]) * f1(x) -
                            (x - points[j][0]) * f2(x)) /
                            (points[j][0] - points[i][0]);
                };
            }
            // Note that the lower triangular part of matrix is useless hence is not being computed
        }

        // Store the final interpolation function
        static function<double(double)> final_func = funcmatrix[0][numpoints - 1];

        // Create a wrapper function
        global_func = [](double x) -> double {
            return final_func(x);
        };

        // Returns the given function pointer
        return global_func;
    }
}

// The function matrix is in the form -->
//  p_(0,0) p_(0,1) p_(0,2) p_(0,3)
//  xxxxxxx p_(1,1) p_(1,2) p_(1,3)
//  xxxxxxx xxxxxxx p_(2,2) p_(2,3)
//  xxxxxxx xxxxxxx xxxxxxx p_(3,3)
// Here the positions with xxxxxxx are useless

