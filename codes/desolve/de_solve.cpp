#include <iostream>
using namespace std;

class linear_de {
public:
    double (*function)(double, double);
    double initialvaluey0;
    double initialvaluex0;
    double stepsize;
    double finalxval;

    // Correct constructor
    linear_de(double (*func)(double, double), double x0, double y0, double finalx, double step) {
        function = func;
        initialvaluey0 = y0;
        initialvaluex0 = x0;
        finalxval = finalx;
        stepsize = step;
    }

    // Euler's Method Function
    double* eulerforward() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        double* yvals = new double[size];

        double x = initialvaluex0;
        yvals[0] = initialvaluey0; // Set initial value

        for (int iter = 0; iter < size - 1; iter++) {
            yvals[iter + 1] = yvals[iter] + stepsize * function(x, yvals[iter]); // Euler's formula
            x += stepsize;
        }

        return yvals; // Return pointer to dynamically allocated array
    }

    double* eulerbackward() {

    }

    double* RK2(){

    }

    double* RK4(){

    }

    double* trapezoidal(){

    }

};

class equation {
// The equation is in the form f(x) = 0
public:
    double (*function)(double);
    
    equation(double (*func)(double)){
        function = func;
    }

    double solveeq() {
        y = 10;
        while(function)
    }
}

// Sample function dy/dx = y
double sampleFunction(double x, double y) {
    return y;
}

int main() {
    linear_de equation(sampleFunction, 0, 1, 2, 0.1);

    double* results = equation.eulerforward();

    // Print the computed values
    int size = int((equation.finalxval - equation.initialvaluex0) / equation.stepsize) + 1;
    for (int i = 0; i < size; i++) {
        cout << "y[" << i << "] = " << results[i] << endl;
    }

    delete[] results; // Free allocated memory

    return 0;
}

