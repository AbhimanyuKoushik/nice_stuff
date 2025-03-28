#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <limits>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace std;
namespace py = pybind11;

class Cluster {
public:
    vector<vector<double>> vectors;

    void addVector(const vector<double>& VectortoAdd) {
        if (VectortoAdd.empty()) return;
        vectors.push_back(VectortoAdd);
    }

    vector<double> meanVector() {
        if (vectors.empty()) return {};

        int dimension = vectors[0].size();
        vector<double> MeanVector(dimension, 0.0);

        for (int i = 0; i < vectors.size(); i++) {
            for (int j = 0; j < dimension; j++) {
                MeanVector[j] += vectors[i][j];
            }
        }

        for (int i = 0; i < dimension; i++) {
            MeanVector[i] /= vectors.size();
        }

        return MeanVector;
    }

    void clear() {
        vectors.clear();
    }

    size_t size() const {
        return vectors.size();
    }
};

double EuclideanDistance(const vector<double>& vector1, const vector<double>& vector2) {
    double distance = 0.0;
    for (int i = 0; i < vector1.size(); i++) {
        distance += pow(vector1[i] - vector2[i], 2);
    }
    return sqrt(distance);
}

vector<vector<double>> RandCentroids(const vector<vector<double>>& Data, int NumberofClusters) {
    int dimension = Data[0].size();
    vector<vector<double>> Centroids(NumberofClusters, vector<double>(dimension));

    srand(time(nullptr));

    for (int i = 0; i < NumberofClusters; i++) {
        for (int j = 0; j < dimension; j++) {
            double minVal = numeric_limits<double>::max();
            double maxVal = numeric_limits<double>::lowest();

            for (int k = 0; k < Data.size(); k++) {
                minVal = min(minVal, Data[k][j]);
                maxVal = max(maxVal, Data[k][j]);
            }

            Centroids[i][j] = minVal + (rand() / (double)RAND_MAX) * (maxVal - minVal);
        }
    }

    return Centroids;
}

vector<vector<vector<double>>> Kmeans(int dimensions, vector<vector<double>>& Data, int NumberofClusters, int iterations = 100) {
    vector<vector<double>> Centroids = RandCentroids(Data, NumberofClusters);
    int dataLength = Data.size();

    vector<Cluster> Clusters(NumberofClusters);

    for (int iter = 0; iter < iterations; iter++) {

        for (int i = 0; i < Clusters.size(); i++) {
            Clusters[i].clear();
        }

        for (int i = 0; i < dataLength; i++) {
            int closestCluster = 0;
            double minDistance = EuclideanDistance(Centroids[0], Data[i]);

            for (int j = 1; j < NumberofClusters; j++) {
                double distance = EuclideanDistance(Centroids[j], Data[i]);
                if (distance < minDistance) {
                    minDistance = distance;
                    closestCluster = j;
                }
            }

            Clusters[closestCluster].addVector(Data[i]);
        }

        for (int i = 0; i < NumberofClusters; i++) {
            if (!Clusters[i].vectors.empty()) {
                Centroids[i] = Clusters[i].meanVector();
            }
        }
    }

    vector<vector<vector<double>>> finalClusters;
    for (int i = 0; i < Clusters.size(); i++) {
        finalClusters.push_back(Clusters[i].vectors);
    }

    return finalClusters;
}

// Pybind11 wrapper
PYBIND11_MODULE(kmeans, m) {
    m.def("kmeans", &Kmeans, "Run K-means clustering",
          py::arg("dimensions"),
          py::arg("Data"),
          py::arg("NumberofClusters"),
          py::arg("iterations") = 100);
}
