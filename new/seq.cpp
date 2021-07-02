#include<iostream>
#include <string>
#include <chrono>
#include <ctime>

#include "node.cpp"
#include "graph.cpp"
#include "../data.cpp"
#include "../utimer.cpp"

#define ACTIVEWAIT = "";

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

void BFS(int starting_node, int value_to_find)
{
    int value_to_find_counts = 0;
    chrono::milliseconds ms = 10ms; // delay

    Graph<int> g;
    for (auto n : nodes)
    {
        g.addNode(n[0], n[1]);
    }
    for (auto e : edges)
    {
        g.addEdge(e[0], e[1]);
    }
    int rows = sizeof nodes / sizeof nodes[0];
    // int cols = sizeof nodes[0] / sizeof(int);

    bool found = false;
    for (int x = 0; x < rows; ++x)
        if (nodes[x][0] == starting_node)
            found = true;

    if (!found)
    {
        std::cout << "Starting node " << starting_node << " is not one of the graph edge\n";
        return;
    }
    cout << "Value to find: " << value_to_find << endl;

    vector<bool> visited(rows, false);
    queue<Node<int>> q;
    Node<int> current;
    q.push(g.getNode(starting_node));
    visited[starting_node] = true;

    cout << "Following is Breadth First Traversal "
         << "(starting from vertex " + to_string(starting_node) + ") \n";

    while (!q.empty())
    {
        delay(ms);
        current = q.front();
        cout << current.get_node_id() << " ";
        q.pop();

        if (current.get_value() == value_to_find)
        {
            value_to_find_counts++;
        }

        vector<Edge> child = current.getChild();

        for (int i = 0; i < child.size(); i++)
        {
            int edge_id = child[i].get_edge_id();
            if (!visited[edge_id])
            {
                q.push(g.getNode(edge_id));
                visited[edge_id] = true;
            }
        }
    }

    cout << endl
         << value_to_find << " is found " << value_to_find_counts
         << " time" << (value_to_find_counts < 2 ? "" : "s")  << endl;
}

int main(int argc, char *argv[])
{
    int starting_node = atoi(argv[1]);
    int value_to_find = atoi(argv[2]);

    {
        utimer ut("SEQ Part");
        BFS(starting_node,value_to_find);
    }

    return 0;
}