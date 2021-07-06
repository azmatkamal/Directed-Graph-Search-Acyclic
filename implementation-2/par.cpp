#include <iostream>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>

#include "node.cpp"
#include "graph.cpp"
#include "../data.cpp"
#include "../utimer.cpp"

std::mutex q1_mtx;
std::mutex checkComplete_mtx;
std::mutex checkProcess_mtx;

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

void BFS(
    int &value_to_find,
    int &value_to_find_counts,
    vector<bool>& visited,
    Graph<int> &g,
    vector<queue<Node<int>>>& queue1,
    vector<queue<Node<int>>>& queue2,
    int num_of_workers,
    int id
)
{
    chrono::milliseconds ms = 10ms; // delay
    queue<Node<int>> q1 = queue1[id];
    queue<Node<int>> q2 = queue2[id];
    Node<int> selected_node;
    bool done = false;
    int value_to_find_counts_2 = 0;
    while(!done) {
        while(!q1.empty()) {
            delay(ms);
            q1_mtx.lock();
            selected_node = q1.front();
            q1.pop();
            if(selected_node.get_value() == value_to_find) value_to_find_counts_2++;
            q1_mtx.unlock();

            vector<Edge> child = selected_node.getChild();

            for (int i = 0; i < child.size(); i++)
            {
                int edge_id = child[i].get_edge_id();
                if (!visited[edge_id])
                {
                    checkProcess_mtx.lock();
                    visited[edge_id] = true;
                    q2.push(g.getNode(edge_id));
                    checkProcess_mtx.unlock();
                }
            }
        }

        done = true;
        q1_mtx.lock();
        for(int i = 0; i < num_of_workers; i++) {
            done &= !queue1[i].empty();
            if(!done && queue1[i].size() > 5) {
               for(int j=0; j < 3; j++) {
                   q1.push(queue1[i].front());
                   queue1[i].pop();
               }
               break;
            }
        }
        q1_mtx.unlock();

        if (done)
        {
            queue<Node<int>> t = q1;
            q1 = q2;
            q2 = t;
            if(!q1.empty()) done = false;
        }
    }

    checkComplete_mtx.lock();
    value_to_find_counts += value_to_find_counts_2;
    checkComplete_mtx.unlock();
}

void ProcessGraph(int starting_node, int value_to_find, int num_of_workers)
{
    int value_to_find_counts = 0;
    chrono::milliseconds ms = 10ms; // delay

    Graph<int> g;
    vector<thread> threads;
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

    vector<bool> visited(rows);
    Node<int> source = g.getNode(starting_node);
    vector<queue<Node<int>>> queue1(num_of_workers);
    vector<queue<Node<int>>> queue2(num_of_workers);

    visited[source.get_node_id()] = true;
    if (source.get_value() == value_to_find)
    {
        value_to_find_counts++;
    }

    vector<Edge> init_edges = source.getChild();
    for(int i = 0; i < init_edges.size(); i++) {
        int edge_id = init_edges[i].get_edge_id();
        if (!visited[edge_id])
        {
            queue1[num_of_workers > 1 ? i % num_of_workers : 0].push(g.getNode(edge_id));
            visited[edge_id] = true;
        }
    }

    for(int i=0; i < num_of_workers; i++) {
        threads.emplace_back(
            BFS,
            std::ref(value_to_find),
            std::ref(value_to_find_counts),
            std::ref(visited),
            std::ref(g),
            std::ref(queue1),
            std::ref(queue2),
            num_of_workers,
            i
        );
    }
    for(auto& thread: threads) {
        thread.join();
    }

    cout << endl
         << value_to_find << " is found " << value_to_find_counts
         << " time" << (value_to_find_counts < 2 ? "" : "s") << endl;
}

int main(int argc, char *argv[])
{
    int starting_node = atoi(argv[1]);
    int value_to_find = atoi(argv[2]);
    int num_of_workers = atoi(argv[3]);

    {
        utimer ut("PAR Part");
        ProcessGraph(starting_node, value_to_find, num_of_workers);
    }

    return 0;
}