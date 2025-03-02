#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <vector>
#include <functional>
#include <cmath>

namespace py = pybind11;
using std::vector;
using std::function;

class LinearDE {
public:
    function<double(double, double)> ode_function;  // Renamed from `function` to `ode_function`
    double initialvaluex0;
    double initialvaluey0;
    double stepsize;
    double finalxval;

    // Constructor
    LinearDE(function<double(double, double)> func, double x0, double y0, double finalx, double step)
        : ode_function(func), initialvaluex0(x0), initialvaluey0(y0), stepsize(step), finalxval(finalx) {}

    // Euler's Method Function
    vector<double> euler_forward() {
        int size = int((finalxval - initialvaluex0) / stepsize);
        vector<double> yvals(size);

        double x = initialvaluex0;
        yvals[0] = initialvaluey0; 

        for (int iter = 0; iter < size - 1; iter++) {
            yvals[iter + 1] = yvals[iter] + stepsize * ode_function(x, yvals[iter]);  // Use `ode_function`
            x += stepsize;
        }

        return yvals;
    }

    // Runge-Kutta 2nd order method
    vector<double> rk2() {
        int size = int((finalxval - initialvaluex0) / stepsize);
        vector<double> yvals(size);

        double x = initialvaluex0;
        yvals[0] = initialvaluey0;

        for (int iter = 0; iter < size - 1; iter++) {
            double k1 = stepsize * ode_function(x, yvals[iter]);  // Use `ode_function`
            double k2 = stepsize * ode_function(x + stepsize, yvals[iter] + k1);
            yvals[iter + 1] = yvals[iter] + 0.5 * (k1 + k2);  // Fix integer division
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
        .def("euler_forward", &LinearDE::euler_forward)
        .def("rk2", &LinearDE::rk2);
}
