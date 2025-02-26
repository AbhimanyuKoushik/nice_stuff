// Finds the shortest path from one point to another point in a graph using Dijkstra's Algorithm

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define INF 1e10 // Large value representing "infinity"

namespace py = pybind11;
using namespace std;

// Node class representing a point in the graph
class Node {
public:
    double coordinates[2]; // Stores x and y coordinates of the node
    Node* previousNode; // Pointer to the previous node in the shortest path
    double cost; // Cost to reach this node from the starting node

    // Constructor initializes the node with given coordinates and default weight
    Node(double x, double y, double weight = 1.0) {
        coordinates[0] = x;
        coordinates[1] = y;
        cost = weight;
        previousNode = nullptr;
    }

    // Computes the Euclidean distance from another node
    double getDistance(const Node& other) const {
        return sqrt(pow(coordinates[0] - other.coordinates[0], 2) + pow(coordinates[1] - other.coordinates[1], 2));
    }

    // Overload equality operator to compare nodes based on coordinates
    bool operator==(const Node& other) const {
        return coordinates[0] == other.coordinates[0] && coordinates[1] == other.coordinates[1];
    }
};

// Edge class representing a connection between two nodes
class Edge {
public:
    Node start;
    Node end;
    double weight;

    // Constructor initializes an edge between two nodes with a given weight
    Edge(Node startNode, Node endNode, double length) : start(startNode), end(endNode), weight(length) {}
};

// Graph class representing the map with nodes and edges
class Graph {
public:
    vector<Node> nodes; // List of all nodes in the graph
    vector<Edge> edges; // List of all edges in the graph
    Node startNode; // Starting node of the path
    Node targetNode; // Target node (destination)

    // Constructor initializes the graph with nodes, edges, start, and end nodes
    Graph(vector<Node> allNodes, vector<Edge> allEdges, Node start, Node end)
        : nodes(allNodes), edges(allEdges), startNode(start), targetNode(end) {}

    // Dijkstra's Algorithm implementation
    vector<Node> findShortestPath() {
        vector<double> shortestDistances(nodes.size(), INF); // Distance from start to each node, initially set to infinity
        vector<Node*> previousNodes(nodes.size(), nullptr); // Stores previous node in the shortest path
        
        // Find the index of the start node
        int startIdx = distance(nodes.begin(), find(nodes.begin(), nodes.end(), startNode));
        shortestDistances[startIdx] = 0; // Distance to start node is 0

        // Iterate over all nodes, size_t for python wrapper purposes
        for (size_t j = 0; j < nodes.size(); j++) {
            double minDistance = INF;
            int minIndex = -1;

            // Find the node with the shortest known distance, size_t for python wrapper purposes
            for (size_t i = 0; i < nodes.size(); i++) {
                if (shortestDistances[i] < minDistance) {
                    minDistance = shortestDistances[i];
                    minIndex = i;
                }
            }

            if (minIndex == -1) break; // If no valid node is found, stop the search

            Node* currentNode = &nodes[minIndex];

            // Update distances for neighboring nodes
            for (Edge& edge : edges) {
                if (edge.start == *currentNode) {
                    int neighborIdx = distance(nodes.begin(), find(nodes.begin(), nodes.end(), edge.end));
                    if (shortestDistances[minIndex] + edge.weight < shortestDistances[neighborIdx]) {
                        shortestDistances[neighborIdx] = shortestDistances[minIndex] + edge.weight;
                        previousNodes[neighborIdx] = currentNode;
                    }
                }
            }
            shortestDistances[minIndex] = INF; // Mark node as visited
        }

        // Reconstruct the shortest path from target to start
        vector<Node> shortestPath;
        Node* step = &targetNode;
        while (step) {
            shortestPath.push_back(*step);
            if (step == &startNode) break;
            step = previousNodes[distance(nodes.begin(), find(nodes.begin(), nodes.end(), *step))];
        }
        reverse(shortestPath.begin(), shortestPath.end());
        return shortestPath;
    }
};

/*
int main() {
    // Create nodes representing locations
    Node nodeA(0, 0);
    Node nodeB(1, 1);
    Node nodeC(2, 2);
    Node nodeD(3, 3);

    // Create edges representing paths between nodes with distances
    Edge edgeAB(nodeA, nodeB, 1.5);
    Edge edgeBC(nodeB, nodeC, 2.0);
    Edge edgeCD(nodeC, nodeD, 2.5);
    Edge edgeAC(nodeA, nodeC, 5.5); // Alternative direct path

    // Initialize graph
    vector<Node> nodes = {nodeA, nodeB, nodeC, nodeD};
    vector<Edge> edges = {edgeAB, edgeBC, edgeCD, edgeAC};
    Graph myGraph(nodes, edges, nodeA, nodeD);

    // Find and print shortest path
    vector<Node> shortestPath = myGraph.findShortestPath();
    cout << "Shortest path:" << endl;
    for (const Node& n : shortestPath) {
        cout << "(" << n.coordinates[0] << ", " << n.coordinates[1] << ") -> ";
    }
    cout << "END" << endl;
    return 0;
}*/


// **PYBIND11 Wrapper**
PYBIND11_MODULE(dijkstra, m) {
    py::class_<Node>(m, "Node")
        .def(py::init<double, double, double>())
        .def_property("x", 
            [](const Node& n) { return n.coordinates[0]; }, 
            [](Node& n, double value) { n.coordinates[0] = value; })
        .def_property("y", 
            [](const Node& n) { return n.coordinates[1]; }, 
            [](Node& n, double value) { n.coordinates[1] = value; })
        .def_readwrite("cost", &Node::cost);

    py::class_<Edge>(m, "Edge")
        .def(py::init<Node, Node, double>())
        .def_readwrite("start", &Edge::start)
        .def_readwrite("end", &Edge::end)
        .def_readwrite("weight", &Edge::weight);

    py::class_<Graph>(m, "Graph")
        .def(py::init<vector<Node>, vector<Edge>, Node, Node>())
        .def("findShortestPath", &Graph::findShortestPath, py::return_value_policy::move);
}