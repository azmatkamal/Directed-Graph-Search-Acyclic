#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>

#include "node.cpp"
#include "graph.cpp"
#include "utimer.cpp"

using namespace std;

void delay(std::chrono::microseconds m)
{
    #ifdef ACTIVEWAIT
        auto active_wait = [](std::chrono::microseconds us)
        {
            long usecs = us.count();
            auto start = std::chrono::high_resolution_clock::now();
            auto end = false;
            while (!end)
            {
                auto elapsed = std::chrono::high_resolution_clock::now() - start;
                auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
                if (usec > usecs)
                    end = true;
            }
            return;
        };
        active_wait(m);
    // #else
    //     std::this_thread::sleep_for(m);
    #endif
        return;
}

void BFS(int starting_node, int value_to_find, int total_nodes, int min_edges_per_node, int max_edges_per_node)
{
    int value_to_find_counts = 0;
    chrono::microseconds us = 500us; // delay

    Graph<int> g;
    {
        utimer ut("Graph processing");
        auto filename = "graph_data/graph_data_" + to_string(total_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
        ifstream file(filename);
        int nodeCount;
        file >> nodeCount;
        
        for (int i = 0; i < nodeCount; ++i) {
            int nodeID, val;
            file >> nodeID >> val;
            g.addNode(nodeID, val);
        }
        while( file.peek() != EOF ) {
            int src, dest;
            file >> src >> dest;
            g.addEdge(src, dest);
            g.addEdge(dest, src);
        }
    }
    {
        utimer ut("SEQ Part");
        bool found = false;
        for (int x = 0; x < total_nodes; ++x)
            if (x == starting_node)
                found = true;

        if (!found)
        {
            std::cout << "Starting node " << starting_node << " is not one of the graph edge\n";
            return;
        }
        cout << "Starting from: " << starting_node << endl;
        cout << "Value to find: " << value_to_find << endl;

        vector<bool> visited(total_nodes, false);
        queue<Node<int>> q;
        Node<int> current;
        q.push(g.getNode(starting_node));
        visited[starting_node] = true;

        while (!q.empty())
        {
            #ifdef WITHTIME
            {
                utimer ut("Time for a single loop");
            #endif

                delay(us);
                current = q.front();
                q.pop();

                #ifdef WITHTIME
                {
                    utimer ut("Time to update counts");
                #endif
                    if (current.get_value() == value_to_find)
                    {
                        value_to_find_counts++;
                    }
                #ifdef WITHTIME
                }
                #endif

                vector<Edge> child = current.getChild();
                #ifdef WITHTIME
                {
                    utimer ut("Time for Edges");
                #endif
                    for (int i = 0; i < child.size(); i++)
                    {
                        int edge_id = child[i].get_edge_id();
                        if (!visited[edge_id])
                        {
                            q.push(g.getNode(edge_id));
                            visited[edge_id] = true;
                        }
                    }
                #ifdef WITHTIME
                }
                #endif
            #ifdef WITHTIME
            }
            #endif
        }

    }
    cout << value_to_find << " is found " << value_to_find_counts
        << " time" << (value_to_find_counts < 2 ? "" : "s")  << endl;
}

int main(int argc, char *argv[])
{
    int starting_node = atoi(argv[1]);
    int value_to_find = atoi(argv[2]);
    int total_nodes = atoi(argv[3]);
    int min_edges_per_node = atoi(argv[4]);
    int max_edges_per_node = atoi(argv[5]);

    BFS(starting_node, value_to_find, total_nodes, min_edges_per_node, max_edges_per_node);

    return 0;
}