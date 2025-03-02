// Finds the shortest path from one point to another point in a graph

// Importing necessary libraries
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>

// Taking infinity to be 10^10
#define INF 1e10

using namespace std;

// Creating a node class 
class node{
public:

    // Each node has a location, the pointer node nearest to it and a weight
    double location[2];
    node* nodewithshortestdist;
    double weight;

    // Building a Constructor for node. Initially, the xcoordinate and ycoordinate are chosen with weight being default 1.0 and node nearest to it being NULL
    node(double xcoord, double ycoord, double w = 1.0) 
        : location{xcoord, ycoord}, weight(w), nodewithshortestdist(nullptr) {}

    // Calculates distance from another node
    double distfromnode(node& othernode){
        double distsqre = (location[0] - othernode.location[0])*(location[0] - othernode.location[0]) + (location[1] - othernode.location[1])*(location[1] - othernode.location[1]);
        return sqrt(distsqre);
    }

    // Defining what node1 == node2, it basically means location of both the nodes is same
    bool operator==(const node& other) const {
        return location[0] == other.location[0] && location[1] == other.location[1];
    }

    // Prints location of node
    void printnode(){
        cout << "Node location: (" << location[0] << "," << location[1] << ")" << endl;
    }
};

// Creating an edge class
class edge{
    public:
        node startnode;
        node endnode;
        double edgelength;
    
        edge(node strnd, node endnd, double edgelen)
            : startnode(strnd), endnode(endnd), edgelength(edgelen) {}

        // Prints properties of Edge
        void printedge(){
            cout << "Start Node: (" << startnode.location[0] << "," << startnode.location[1] << ")" << endl;
            cout << "End Node: (" << endnode.location[0] << "," << endnode.location[1] << ")" << endl;
            cout << "Edge Length: " << edgelength << endl;
        }       
    };

// Building a Map Class
class Map{
public:

    // Each map has a list of pointers to all nodes, pointers to all edges, the starting point and the goal
    vector<node> allnodes;
    vector<edge> allEdges;
    node startpoint;
    node endpoint;

    // Building a constructor for Map Class
    Map(vector<node> allnd, vector<edge> alledg, node strpt, node endpt)
        : allnodes(allnd), allEdges(alledg), startpoint(strpt), endpoint(endpt) {}

     // Defining Dijkstra's function with output as an array of nodes where (i+1)th element tell the location at ith step

    // Rules of Dijkstra -
    // 1. From the starting node, visit the node with the shortest distance
    // 2. Once reached there, check the neighbouring nodes
    // 3. Calculate the distance for the neighbouring nodes by summing the cost of the edges leading from the starting node
    // 4. If the distance to a vertex we are checking is less than a known distance, update the shortest distance from starting point for that vertex
    vector<node> dijkstra_directed(){
        // Initializing things

        // Assigning startIdx as the index of the starting node in the allnodes list
        int startIdx = distance(allnodes.begin(), find(allnodes.begin(), allnodes.end(), startpoint));

        // Since we dont know the distance of any node from the start, we will assume them as INF but we will keep the shortest distance for starting point as 0
        vector<double> shortestdistfromstart(allnodes.size(), INF);
        vector<node*> previousNodes(allnodes.size(), nullptr);
        vector<bool> visited(allnodes.size(), false);

        // Shortest distance of starting point from itself is 0
        shortestdistfromstart[startIdx] = 0;

        for(int j = 0; j < allnodes.size(); j++){
            // Find the unvisited node with the smallest known distance

            // For jth node, the node with minimum distance doesnt exist (initialzing like this)
            double mindist = INF;
            int minIndex = -1;

            // Iterating through all the nodes to find the node with minimum distance from the starting node
            for (int i = 0; i < allnodes.size(); i++) {
                if (!visited[i] && shortestdistfromstart[i] < mindist) {
                    mindist = shortestdistfromstart[i];
                    minIndex = i;
                }
            }
            // Once found, we set the shortst distance for that node and set minIndex to the Index of that element


            // If no valid node is found, stop the search
            if (minIndex == -1) break;

            // Else the node right now which has minimum distance from starting node is the current node
            visited[minIndex] = true; // Mark node as visited
            node* currentNode = &allnodes[minIndex];

             // Iterating through all edges to check which edges have starting node as the node with minIndex
            for (int i = 0; i < allEdges.size(); i++) {
                if (allEdges[i].startnode == *currentNode) {
                    // Once found, we check through all of them to see which node has the shortest distance from the particular node
                    int neighborIdx = distance(allnodes.begin(), find(allnodes.begin(), allnodes.end(), allEdges[i].endnode));

                    // If the distance from starting point following from the shortest node path is shorter, we will update the distance of the node from starting point
                    // Otherwise we will leave it as it is    
                    if (shortestdistfromstart[minIndex] + allEdges[i].edgelength < shortestdistfromstart[neighborIdx]) {
                        shortestdistfromstart[neighborIdx] = shortestdistfromstart[minIndex] + allEdges[i].edgelength;
                        previousNodes[neighborIdx] = currentNode;
                    }
                }
            }
        }

        // Reconstruct the shortest path from endpoint to startpoint by Back tracking
        vector<node> shortestPath;
        node* step = &endpoint;
        while (step) {
            // Stops when null pointer is reached
            shortestPath.push_back(*step);
            if (step == &startpoint) break;
            step = previousNodes[distance(allnodes.begin(), find(allnodes.begin(), allnodes.end(), *step))];
        }
        reverse(shortestPath.begin(), shortestPath.end());
        return shortestPath;
    }

    // To find the shortest distance if edges are not directed, we can the direction of each edge, add it into the list of edges and run directed dijkstra

    // Function to add a reversed edge for undirected graphs
    vector<edge> edgeNondirectional(const edge edgetoChange,const vector<edge>& edgesListtoAdd) {
        vector<edge> edgeNondirectionallist = edgesListtoAdd;
        edge edgeReverse(edgetoChange.endnode, edgetoChange.startnode, edgetoChange.edgelength);
        edgeNondirectionallist.push_back(edgeReverse);
        return edgeNondirectionallist;
    }

    vector<node> dijkstra_undirected() {
        // Create a new edges list with reversed edges added
        vector<edge> allEdgesUndirected = allEdges;
        
        for (int i = 0; i < allEdges.size(); i++) {
            allEdgesUndirected = edgeNondirectional(allEdges[i], allEdgesUndirected);
        }

        // Create a temporary map with the updated edges list
        Map undirectedMap(allnodes, allEdgesUndirected, startpoint, endpoint);
        
        // Run Dijkstra on the undirected graph
        return undirectedMap.dijkstra_directed();
    }

};

int main() {
    node n1(0, 0);
    node n2(1, 1);
    node n3(2, 2);
    node n4(3, 3);

    edge e1(n1, n2, 1.5);
    edge e2(n2, n3, 2.0);
    edge e3(n3, n4, 2.5);
    edge e4(n3, n1, 0.5);

    vector<node> nodes = {n1, n2, n3, n4};
    vector<edge> edges = {e1, e2, e3, e4};

    Map myGraph(nodes, edges, n1, n4);
    vector<node> shortestPath = myGraph.dijkstra_directed();

    cout << "Shortest path:" << endl;
    for (int i = 0; i < shortestPath.size(); i++) {
        cout << "(" << shortestPath[i].location[0] << ", " << shortestPath[i].location[1] << ") -> ";
    }
    cout << "END" << endl;

    return 0;
}
