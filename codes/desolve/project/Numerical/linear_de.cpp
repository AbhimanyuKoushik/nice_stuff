#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <functional>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>

namespace py = pybind11;
using std::vector;
using std::function;
using namespace std;

class equation {
public:
    function<double(double)> f; // The equation is f(x) = 0

    // Constructor function
    equation(function<double(double)> func) : f(func) {}

    double newtonsolve() {
        // Set up a random number generator
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(20.0, 40.0); // Adjust range if needed

        double guess = dist(gen);  // Random initial guess
        double limh = 1e-3;
        double tolerance = 1e-3;
        int max_iter = 100;
        int iter = 0;

        while (fabs(f(guess)) > tolerance && iter < max_iter) {
            double df = (f(guess + limh) - f(guess)) / limh;
            if (df == 0) {
                throw std::runtime_error("Derivative is zero, Newton's method fails.");
            }
            guess = guess - f(guess) / df;
            iter++;
        }

        return guess;
    }
};

class LinearDE {
public:
    function<double(double, double)> ode_function;
    double initialvaluex0;
    double initialvaluey0;
    double stepsize;
    double finalxval;

    // Constructor
    LinearDE(function<double(double, double)> func, double x0, double y0, double finalx, double step)
        : ode_function(func), initialvaluex0(x0), initialvaluey0(y0), stepsize(step), finalxval(finalx) {}

    // Euler's Method Function
    vector<double> EulerForward() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        vector<double> yvals(size);
        double x = initialvaluex0;
        yvals[0] = initialvaluey0; 

        for (int iter = 0; iter < size - 1; iter++) {
            yvals[iter + 1] = yvals[iter] + stepsize * ode_function(x, yvals[iter]);
            x += stepsize;
        }

        return yvals;
    }

    // Runge-Kutta 2nd order method
    vector<double> RK2() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        vector<double> yvals(size);
        double x = initialvaluex0;
        yvals[0] = initialvaluey0;

        double k1, k2;

        for (int iter = 0; iter < size - 1; iter++) {
            k1 = stepsize * ode_function(x, yvals[iter]);
            k2 = stepsize * ode_function(x + stepsize, yvals[iter] + k1);
            yvals[iter + 1] = yvals[iter] + 0.5 * (k1 + k2);
            x += stepsize;
        }

        return yvals;
    }

    vector<double> RK4() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        vector<double> yvals(size);
        double x = initialvaluex0;
        yvals[0] = initialvaluey0;

        double k1, k2, k3, k4;

        for (int iter = 0; iter < size - 1; iter++) {
            k1 = stepsize * ode_function(x, yvals[iter]);
            k2 = stepsize * ode_function(x + (stepsize / 2), yvals[iter] + (k1 / 2));
            k3 = stepsize * ode_function(x + (stepsize / 2), yvals[iter] + (k2 / 2));
            k4 = stepsize * ode_function(x + stepsize, yvals[iter] + k3);
            yvals[iter + 1] = yvals[iter] + (1.0 / 6.0) * (k1 + 2 * k2 + 2 * k3 + k4);
            x += stepsize;
        }

        return yvals;
    }

    vector<double> EulerBackward() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        vector<double> yvals(size);
        double x = initialvaluex0;
        yvals[0] = initialvaluey0; 

        for (int iter = 0; iter < size - 1; iter++) {
            auto update_function = [&](double yn) {
                return yn - yvals[iter] - stepsize * ode_function(x + stepsize, yn);
            };
            equation update_eq(update_function);
            yvals[iter + 1] = update_eq.newtonsolve();
            x += stepsize;
        }

        return yvals;
    }

    vector<double> Trapezoidal() {
        int size = int((finalxval - initialvaluex0) / stepsize) + 1;
        vector<double> yvals(size);
        double x = initialvaluex0;
        yvals[0] = initialvaluey0; 

        for (int iter = 0; iter < size - 1; iter++) {
            auto update_function = [&](double yn) {
                return yn - yvals[iter] - stepsize * (ode_function(x + stepsize, yn) + ode_function(x, yn)) * 0.5;
            };
            equation update_eq(update_function);
            yvals[iter + 1] = update_eq.newtonsolve();
            x += stepsize;
        }

        return yvals;
    }
};

// Pybind11 module definition
PYBIND11_MODULE(linear_de, m) {
    py::class_<LinearDE>(m, "LinearDE")
        .def(py::init<
                 function<double(double, double)>,
                 double, double, double, double>(),
             py::arg("func"), py::arg("x0"), py::arg("y0"),
             py::arg("finalx"), py::arg("stepsize"))
        .def("EulerForward", &LinearDE::EulerForward)
        .def("RK2", &LinearDE::RK2)
        .def("RK4", &LinearDE::RK4)
        .def("EulerBackward", &LinearDE::EulerBackward)
        .def("Trapezoidal", &LinearDE::Trapezoidal);

    py::class_<equation>(m, "equation")
        .def(py::init<function<double(double)>>())
        .def("newtonsolve", &equation::newtonsolve);
}