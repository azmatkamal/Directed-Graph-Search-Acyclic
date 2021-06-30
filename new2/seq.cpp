#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>

#include "../data.cpp"
#include "Graph.cpp"

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

// int Graph::BFS(int s, int value_to_find)
// {
//     // std::chrono::milliseconds ms = 10ms;
//     int found_value_count = 0;
//     // Mark all the vertices as not visited
//     bool *visited = new bool[V];
//     for (int i = 0; i < V; i++)
//         visited[i] = false;

//     // Create a queue for BFS
//     list<int> queue;

//     // Mark the current node as visited and enqueue it
//     visited[s] = true;
//     queue.push_back(s);

//     // 'i' will be used to get all adjacent
//     // vertices of a vertex
//     list<int>::iterator i;

//     while (!queue.empty())
//     {
//         // Dequeue a vertex from queue and print it
//         s = queue.front();
//         cout << s << " ";
//         queue.pop_front();

//         // Get all adjacent vertices of the dequeued
//         // vertex s. If a adjacent has not been visited,
//         // then mark it visited and enqueue it
//         for (i = adj[s].begin(); i != adj[s].end(); ++i)
//         {
//             if (!visited[*i])
//             {
//                 visited[*i] = true;
//                 queue.push_back(*i);
//                 if (values[*i] == value_to_find)
//                 {
//                     found_value_count++;
//                 }
//                 // for_each(std::begin(values), std::end(values), [&value_to_find, &found_value_count, i](int(&row)[2])
//                 // {
//                 //     if (row[0] == *i && row[1] == value_to_find)
//                 //     {
//                 //         found_value_count++;
//                 //     }
//                 // });
//                 // delay(ms);
//             }
//         }
//     }
//     return found_value_count;
// }

// Driver program to test methods of graph class
int main(int argc, char *argv[])
{

    Graph_NS::Graph<int> graph(true);
    // Graph gs(1047);
    for (auto n : nodes)
    {
        graph.add_edge(n[0], n[1]);
    }

    vector<vector<int>> result = graph.bfs();
    for (vector<vector<int>>::size_type i = 0; i < result.size(); i++)
    {
        for (vector<int>::size_type j = 0; j < result[i].size(); j++)
        {
            cout << result[i][j] << '-';
        }
        cout << endl;
    }
    return 0;
}