
// #include "bfs.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()
#include "data.cpp"

using namespace std;

namespace bfs
{
    void BFS(int s, int value_to_find, int found_value_count, bool visited[], list<int> *adj, list<int> queue, list<int> queue2, int nw)
    {
        // Mark the current node as visited and enqueue it
        visited[s] = true;
        queue.push_back(s);

        // 'i' will be used to get all adjacent
        // vertices of a vertex
        list<int>::iterator i;

        while (!queue.empty())
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
                    if (values[*i] == value_to_find)
                    {
                        found_value_count++;
                    }
                    // delay(ms);
                }
            }
        }
        cout << endl
            << "Found '" << value_to_find << "' " << found_value_count << " times." << endl;
    }
}