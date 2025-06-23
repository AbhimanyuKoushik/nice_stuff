#include "Neural_net.hpp"
#include <cstdlib>
#include <ctime>

namespace ml {

// Constructor: allocate and initialize all layers
NeuralNetwork::NeuralNetwork(int in_dim, int hidden_layers,
                            int hidden_size, int out_dim) {
    // seed RNG once
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    // 1) build layer_sizes[]
    num_layers = hidden_layers + 2; // input + hidden + output
    layer_sizes = new int[num_layers];
    layer_sizes[0] = in_dim;
    for (int i = 1; i < num_layers - 1; ++i)
        layer_sizes[i] = hidden_size;
    layer_sizes[num_layers - 1] = out_dim;
    
    // 2) allocate W[] and b[]
    W = new Matrix*[num_layers - 1];
    b = new Vector*[num_layers - 1];
    
    // 3) initialize weights ∈[-1,1], biases ←0
    for (int l = 0; l < num_layers - 1; ++l) {
        int Rin = layer_sizes[l], Rout = layer_sizes[l+1];
        W[l] = new Matrix(Rin, Rout);
        b[l] = new Vector(Rout);
        
        for (int i = 0; i < Rin; ++i)
            for (int j = 0; j < Rout; ++j)
                (*W[l])(i, j) = 2.0 * (std::rand() / (double)RAND_MAX) - 1.0;
        
        for (int j = 0; j < Rout; ++j)
            (*b[l])(j) = 0.0;
    }
}

// Destructor: delete all allocated memory
NeuralNetwork::~NeuralNetwork() {
    for (int l = 0; l < num_layers - 1; ++l) {
        delete W[l];
        delete b[l];
    }
    
    delete[] W;
    delete[] b;
    delete[] layer_sizes;
}

// Forward propagate a single input
Vector NeuralNetwork::forward(const Vector &input) {
    // allocate activations per layer: use raw arrays
    Vector *A = new Vector[static_cast<size_t>(num_layers)];
    A[0] = input; // copy input
    
    for (int l = 0; l < num_layers - 1; ++l) {
        int Rout = layer_sizes[l+1];
        A[l+1] = Vector(Rout); // allocate next layer
        
        for (int j = 0; j < Rout; ++j) {
            double sum = (*b[l])(j);
            for (int i = 0; i < layer_sizes[l]; ++i)
                sum += A[l](i) * (*W[l])(i, j);
            A[l+1](j) = sigmoid(sum);
        }
    }
    
    Vector output = A[num_layers - 1]; // copy result
    delete[] A;
    return output;
}

// Train on a batch of M samples
void NeuralNetwork::train_batch(const Vector *inputs,
                               const Vector *targets,
                               int M, double lr) {
    int L = num_layers - 1;
    
    // 1) allocate gradient accumulators
    Matrix **gradW = new Matrix*[L];
    Vector **gradb = new Vector*[L];
    
    for (int l = 0; l < L; ++l) {
        gradW[l] = new Matrix(layer_sizes[l], layer_sizes[l+1]);
        gradb[l] = new Vector(layer_sizes[l+1]);
        
        // zero-init
        for (int i = 0; i < layer_sizes[l]; ++i)
            for (int j = 0; j < layer_sizes[l+1]; ++j)
                (*gradW[l])(i,j) = 0.0;
        
        for (int j = 0; j < layer_sizes[l+1]; ++j)
            (*gradb[l])(j) = 0.0;
    }
    
    // 2) loop over each sample
    for (int k = 0; k < M; ++k) {
        // a) forward on sample k
        Vector *A = new Vector[static_cast<size_t>(num_layers)];
        A[0] = inputs[k];
        
        for (int l = 0; l < L; ++l) {
            int Rout = layer_sizes[l+1];
            A[l+1] = Vector(Rout);
            
            for (int j = 0; j < Rout; ++j) {
                double sum = (*b[l])(j);
                for (int i = 0; i < layer_sizes[l]; ++i)
                    sum += A[l](i) * (*W[l])(i, j);
                A[l+1](j) = sigmoid(sum);
            }
        }
        
        // b) backward: compute deltas
        Vector *delta = new Vector[L];
        
        // output delta
        delta[L-1] = Vector(layer_sizes[L]);
        for (int j = 0; j < layer_sizes[L]; ++j) {
            double err = targets[k](j) - A[L](j);
            delta[L-1](j) = err * dsigmoid(A[L](j));
        }
        
        // hidden deltas
        for (int l = L-2; l >= 0; --l) {
            delta[l] = Vector(layer_sizes[l+1]);
            for (int j = 0; j < layer_sizes[l+1]; ++j) {
                double err = 0.0;
                for (int m = 0; m < layer_sizes[l+2]; ++m)
                    err += delta[l+1](m) * (*W[l+1])(j, m);
                delta[l](j) = err * dsigmoid(A[l+1](j));
            }
        }
        
        // c) accumulate gradients
        for (int l = 0; l < L; ++l) {
            for (int i = 0; i < layer_sizes[l]; ++i)
                for (int j = 0; j < layer_sizes[l+1]; ++j)
                    (*gradW[l])(i,j) += delta[l](j) * A[l](i);
            
            for (int j = 0; j < layer_sizes[l+1]; ++j)
                (*gradb[l])(j) += delta[l](j);
        }
        
        delete[] A;
        delete[] delta;
    }
    
    // 3) apply averaged gradients
    for (int l = 0; l < L; ++l) {
        double factor = lr / M;
        for (int i = 0; i < layer_sizes[l]; ++i)
            for (int j = 0; j < layer_sizes[l+1]; ++j)
                (*W[l])(i,j) += factor * (*gradW[l])(i,j);
        
        for (int j = 0; j < layer_sizes[l+1]; ++j)
            (*b[l])(j) += factor * (*gradb[l])(j);
    }
    
    // 4) cleanup
    for (int l = 0; l < L; ++l) {
        delete gradW[l];
        delete gradb[l];
    }
    
    delete[] gradW;
    delete[] gradb;
}

} // namespace ml
