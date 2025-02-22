#include <iostream>
#include <cstdlib>
#include <functional> // Needed for std::function

using namespace std;
typedef function<double(double)> funcs;  // Use std::function for lambda support

extern "C" {
funcs polyinterpol(int numpoints, double** points){
    // Matrix to store functions
    funcs funcmatrix[numpoints][numpoints];

    // Fill the function matrix using Neville's Algorithm
    for (int i = 0; i < numpoints; i++) {
        funcmatrix[i][i] = [i, points](double x) -> double {
            return points[i][1];  // f(xi)
        };
    }

    // Compute higher-order interpolations
    for (int d = 1; d < numpoints; d++) { // d is the diagonal level
        for (int i = 0; i < numpoints - d; i++) {
            int j = i + d;
            auto f1 = funcmatrix[i + 1][j];  // Copy function
            auto f2 = funcmatrix[i][j - 1];  // Copy function

            funcmatrix[i][j] = [i, j, points, f1, f2](double x) -> double {
                return ((x - points[i][0]) * f1(x) -
                        (x - points[j][0]) * f2(x)) /
                        (points[j][0] - points[i][0]);
            };
        }
    }

    return funcmatrix[0][numpoints-1];
}
}

int main() {
    int numpoints;
    cout <<"Enter the number of points: ";
    cin >> numpoints;

    // Allocate memory for points
    double** points = (double**)malloc(numpoints * sizeof(double*));
    for (int i = 0; i < numpoints; i++) {
        points[i] = (double*)malloc(2 * sizeof(double));
    }

    cout << "Enter the points in form 'x y':\n" ;

    for(int i = 0; i < numpoints; i++){
        cin >> points[i][0] >> points [i][1];
    }

    // Evaluate Neville's interpolation at x = 4
    double result;
    result = polyinterpol(numpoints, points)(4);

    cout << "Interpolated value at x = 4: " << result << endl;  // Print result

    // Free allocated memory
    for (int i = 0; i < numpoints; i++) {
        free(points[i]);
    }
    free(points);

    return 0;
}
