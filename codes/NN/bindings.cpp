#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include "Neural_net.hpp" // defines ml::NeuralNetwork
#include "Matrix.hpp" // defines global ::Vector

namespace py = pybind11;

PYBIND11_MODULE(neural_net, m) {
    m.doc() = "C++ Neural Network with pybind11";

    // Translate std::runtime_error → NNError
    py::register_exception<std::runtime_error>(m, "NNError");

    py::class_<ml::NeuralNetwork>(m, "NeuralNetwork")
        .def(py::init<int, int, int, int>(),
            py::arg("in_dim"),
            py::arg("hidden_layers"),
            py::arg("hidden_size"),
            py::arg("out_dim"),
            "Create a feedforward neural network")
        .def("forward",
            [](ml::NeuralNetwork &nn, py::array_t<double> input) {
                auto buf = input.unchecked<1>();
                ::Vector v(static_cast<int>(buf.shape(0)));
                for (ssize_t i = 0; i < buf.shape(0); ++i)
                    v(i) = buf(i);
                ::Vector out = nn.forward(v);
                py::array_t<double> result(out.size);
                auto rbuf = result.mutable_unchecked<1>();
                for (int i = 0; i < out.size; ++i)
                    rbuf(i) = out(i);
                return result;
            },
            py::arg("input"),
            "Run forward propagation on a single sample")
        .def("train_batch",
            [](ml::NeuralNetwork &nn,
               py::array_t<double> X,
               py::array_t<double> Y,
               int batch_size,
               double lr) {
                auto xb = X.unchecked<2>();
                auto yb = Y.unchecked<2>();
                
                // Suppress alloc-size-larger-than warning: batch_size > 0 guaranteed
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Walloc-size-larger-than="
                ::Vector *inputs = new ::Vector[static_cast<size_t>(batch_size)];
                ::Vector *targets = new ::Vector[static_cast<size_t>(batch_size)];
                #pragma GCC diagnostic pop
                
                int in_dim = static_cast<int>(xb.shape(1));
                int out_dim = static_cast<int>(yb.shape(1));
                
                for (int k = 0; k < batch_size; ++k) {
                    inputs[k] = ::Vector(in_dim);
                    targets[k] = ::Vector(out_dim);
                    for (int j = 0; j < in_dim; ++j)
                        inputs[k](j) = xb(k, j);
                    for (int j = 0; j < out_dim; ++j)
                        targets[k](j) = yb(k, j);
                }
                
                nn.train_batch(inputs, targets, batch_size, lr);
                delete[] inputs;
                delete[] targets;
            },
            py::arg("X"),
            py::arg("Y"),
            py::arg("batch_size"),
            py::arg("lr") = 0.01,
            "Train on a mini-batch of samples");
}
