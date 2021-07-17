#include<iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

#include "utimer.cpp"
#include "data.cpp"

using namespace std;

void delay(std::chrono::milliseconds m)
{
#ifdef ACTIVEWAIT
    auto active_wait = [](std::chrono::milliseconds ms)
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
    };
    active_wait(m);
#else
    std::this_thread::sleep_for(m);
#endif
    return;
}

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
    int BFS(int s, int value_to_find);
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

int Graph::BFS(int s, int value_to_find)
{
    std::chrono::milliseconds ms = 5ms;
    int found_value_count = 0;
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
        delay(ms);
        // Dequeue a vertex from queue and print it
        s = queue.front();
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
                if(values2[*i] == value_to_find)
                {
                    found_value_count++;
                }
            }
        }
    }
    return found_value_count;
}

// Driver program to test methods of graph class
int main(int argc, char *argv[])
{

    {
        utimer ut("SEQ TIME");
        // auto start = std::chrono::high_resolution_clock::now();
        // Create a graph given in the above diagram

        int rows = sizeof nodes / sizeof nodes[0];
        Graph gs((rows+1));
        // Graph gs(1047);
        for (auto n : edges)
        {
            gs.addEdge(n[0], n[1]);
        }

        int starting_vertex = atoi(argv[1]);
        bool found = (std::find(gs.all_elements.begin(), gs.all_elements.end(), starting_vertex) != gs.all_elements.end());

        if (!found)
        {
            std::cout << "Starting vertex " << starting_vertex << " is not one of the graph edge\n";
            return 0;
        }
        int value_to_find = atoi(argv[2]);
        cout << "Value to find: " << value_to_find << endl;

        int found_value_count = gs.BFS(starting_vertex, value_to_find);
        cout << endl << "Found '" << value_to_find << "' " << found_value_count << " times." << endl;

        // auto elapsed = std::chrono::high_resolution_clock::now() - start;
        // auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
        // cout << endl << "Spent " << usec << " usecs" << endl;
    }
    return 0;
}