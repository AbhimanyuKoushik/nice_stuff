#ifndef NEURAL_NET_HPP
#define NEURAL_NET_HPP

#include "Matrix.hpp"
#include <cmath>
#include <cstdlib>
#include <ctime>

namespace ml {

class NeuralNetwork {
public:
    // Total number of layers (input + hidden + output)
    int num_layers;

    // Array of layer sizes: length == num_layers
    // layer_sizes[0] == input dimension
    // layer_sizes[num_layers-1] == output dimension
    int *layer_sizes;

    // Weight matrices: W[l] is a (layer_sizes[l] x layer_sizes[l+1]) matrix
    Matrix **W; // Since we don't know the size of Weight Matrices (determined by number of perceptrons in two adjacent hidden
                //  layers) and the number of Weight Matrices (determined by number of Hiddenlayers) we are using a double pointer.
                //  Now, we can decide the number of Weight Matrices and their sizes at runtime

    // Similar reasoning for Bias vector
    // Bias vectors: b[l] is a length-(layer_sizes[l+1]) vector
    Vector **b;

    /**
     * Construct a fully-connected feedforward network
     * @param in_dim        Number of input features
     * @param hidden_layers Number of hidden layers
     * @param hidden_size   Number of neurons in each hidden layer
     * @param out_dim       Number of outputs
     */
    NeuralNetwork(int in_dim,
                  int hidden_layers,
                  int hidden_size,
                  int out_dim);

    /// Free all allocated memory
    ~NeuralNetwork();

    /**
     * Single-sample forward propagation
     * @param input  Length-in_dim Vector
     * @return       Length-out_dim Vector of outputs (after sigmoid)
     */
    Vector forward(const Vector &input);

    /**
     * Mini-batch gradient descent training
     * @param inputs     Array of M input Vectors (length-in_dim)
     * @param targets    Array of M target Vectors (length-out_dim)
     * @param M          Batch size (# of samples)
     * @param lr         Learning rate
     */
    void train_batch(const Vector *inputs,
                     const Vector *targets,
                     int M,
                     double lr);

private:
    /// Sigmoid activation
    static double sigmoid(double x) {
        return 1.0 / (1.0 + std::exp(-x));
    }

    /// Derivative of sigmoid, given y = sigmoid(x)
    static double dsigmoid(double y) {
        return y * (1.0 - y);
    }
};

} // namespace ml

#endif // NEURAL_NET_HPP
