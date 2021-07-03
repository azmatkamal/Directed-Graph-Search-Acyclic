#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <thread>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()

#include "utimer.cpp"
#include "bfs.cpp"

using namespace std;

class Graph
{
    int V; // No. of vertices

    // Pointer to an array containing adjacency
    // lists
    list<int> *adj;

public:
    list<int> all_elements;
    Graph(int V); // Constructor

    // list<int> getEdges();

    // function to add an edge to graph
    void addEdge(int v, int w);

    // prints BFS traversal from a given source s
    void Execute(Graph gs, int s, int value_to_find, int nw);
    void BFS(Graph gs, int s, int value_to_find, int nw);
};

Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}

void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
    adj[w].push_back(v); // Add v to w’s list.
    all_elements.push_back(v);
    all_elements.push_back(w);
}

void Graph::Execute(Graph gs, int s, int value_to_find, int nw)
{
    int found_value_count = 0;
    // Mark all the vertices as not visited
    vector<bool> visited(V);
    for (int i = 0; i < V; i++)
        visited[i] = false;

    // Create a queue for BFS
    vector<queue<int>> queue1(nw);
    vector<queue<int>> queue2(nw);

    // Mark the current node as visited and enqueue it
    visited[s] = true;
    if (values2[s] == value_to_find)
    {
        // found_value_count++;
    }
    // queue1[0].push(s);
    list<int>::iterator i;
    // Get all adjacent vertices of the dequeued
    // vertex s. If a adjacent has not been visited,
    // then mark it visited and enqueue it
    int x = 0;
    for (i = adj[s].begin(); i != adj[s].end(); ++i)
    {
        if (!visited[*i])
        {
            visited[*i] = true;
            queue1[nw>1? x%nw : 0].push(*i);
            x++;
        }
    }
    std::vector<std::thread *> tids(nw);
    for (int j = 0; j < nw; j++)
        tids[j] = new std::thread(
            bfs::BFS, 
            std::ref(value_to_find), 
            std::ref(found_value_count), 
            std::ref(visited), 
            std::ref(adj), 
            std::ref(queue1), 
            std::ref(queue2), 
            nw, 
            j
        );

    for (int j = 0; j < nw; j++)
        tids[j]->join();

    cout << endl
         << "Found '" << value_to_find << "' " << found_value_count << " times." << endl;
}

// Driver program to test methods of graph class
int main(int argc, char *argv[])
{
    {
        utimer ut("PAR TIME");
        int rows = sizeof nodes / sizeof nodes[0];
        Graph gs((rows + 1));
        // Graph gs(3951);
        for (auto n : edges)
        {
            gs.addEdge(n[0], n[1]);
        }

        int starting_vertex;
        int nw;
        int value_to_find;
        int found_value_count = 0;
        if (argc == 4)
        {
            starting_vertex = atoi(argv[1]);
            cout << "Starting from: " << starting_vertex << endl;
            value_to_find = atoi(argv[2]);
            cout << "Value to find: " << value_to_find << endl;
            nw = atoi(argv[3]);
            cout << "No. of workers: " << nw << endl;
        }
        else
        {
            cout << "Not all params were passed";
            return 0;
        }

        bool found = (std::find(gs.all_elements.begin(), gs.all_elements.end(), starting_vertex) != gs.all_elements.end());

        if (!found)
        {
            std::cout << "Starting vertex " << starting_vertex << " is not one of the graph edge\n";
            return 0;
        }

        gs.Execute(gs, starting_vertex, value_to_find, nw);
    }
    return 0;
}
