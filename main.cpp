#include<iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

#include "utimer.cpp"

using namespace std;

void activewait(std::chrono::milliseconds ms)
{
    long msecs = ms.count();
    auto start = std::chrono::high_resolution_clock::now();
    auto end = false;
    while (!end)
    {
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
        if (msec > msecs)
            end = true;
    }
    return;
}

int f(int x)
{
    activewait(1ms);
    return (x + x);
}

// This class represents a directed graph using
// adjacency list representation
class Graph
{
    int V;    // No. of vertices
 
    // Pointer to an array containing adjacency
    // lists
    list<int> *adj;

public:
    list<int> all_elements;
    Graph(int V);  // Constructor

    // list<int> getEdges();

    // function to add an edge to graph
    void addEdge(int v, int w);

    // prints BFS traversal from a given source s
    void BFS(int s); 
};
 
Graph::Graph(int V)
{
    this->V = V;
    adj = new list<int>[V];
}
 
void Graph::addEdge(int v, int w)
{
    adj[v].push_back(w); // Add w to v’s list.
    all_elements.push_back(v);
    all_elements.push_back(w);
}
 
void Graph::BFS(int s)
{
    // Mark all the vertices as not visited
    bool *visited = new bool[V];
    for(int i = 0; i < V; i++)
        visited[i] = false;
 
    // Create a queue for BFS
    list<int> queue;
 
    // Mark the current node as visited and enqueue it
    visited[s] = true;
    queue.push_back(s);
 
    // 'i' will be used to get all adjacent
    // vertices of a vertex
    list<int>::iterator i;
 
    while(!queue.empty())
    {
        // Dequeue a vertex from queue and print it
        s = queue.front();
        cout << s << " ";
        queue.pop_front();
 
        // Get all adjacent vertices of the dequeued
        // vertex s. If a adjacent has not been visited,
        // then mark it visited and enqueue it
        for (i = adj[s].begin(); i != adj[s].end(); ++i)
        {
            if (!visited[*i])
            {
                visited[*i] = true;
                queue.push_back(*i);
            }
        }
    }
}
 
// Driver program to test methods of graph class
int main(int argc, char *argv[])
{
    // Create a graph given in the above diagram
    Graph gs(9);
    gs.addEdge(0, 1);
    gs.addEdge(0, 6);
    gs.addEdge(2, 4);
    gs.addEdge(4, 3);
    gs.addEdge(3, 5);
    gs.addEdge(6, 7);
    gs.addEdge(1, 2);
    gs.addEdge(1, 8);


    int starting_vertex = atoi(argv[1]);
    bool found = (std::find(gs.all_elements.begin(), gs.all_elements.end(), starting_vertex) != gs.all_elements.end());

    if (!found)
    {
        std::cout << "Starting vertex " << starting_vertex << " is not one of the graph edge\n";
        return 0;
    }

    cout << "Following is Breadth First Traversal "
         << "(starting from vertex " + to_string(starting_vertex) + ") \n";
    gs.BFS(starting_vertex);

    return 0;
}