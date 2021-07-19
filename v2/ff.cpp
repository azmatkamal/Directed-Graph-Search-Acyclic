#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <chrono>
#include <mutex>
#include <thread>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()
#include <ff/ff.hpp>
#include <ff/node.hpp>

#include "node.cpp"
#include "graph.cpp"
#include "../graph_data.cpp"
#include "../graph_values.cpp"
#include "../utimer.cpp"

using namespace ff;
using namespace std;

std::mutex q1_mtx;
std::mutex checkComplete_mtx;
std::mutex checkProcess_mtx;
std::mutex queueShift;

std::atomic<int> value_to_find_counts = 0;
int value_to_find;
int starting_node;
int num_of_workers;
Graph<int> g;
vector<bool> visited;
vector<queue<Node<int>>> queue1;
vector<queue<Node<int>>> queue2;

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
// #else
//     std::this_thread::sleep_for(m);
#endif
    return;
}

void BFS(
    int &value_to_find,
    atomic<int> &value_to_find_counts,
    vector<bool> &visited,
    Graph<int> &g,
    vector<queue<Node<int>>> &queue1,
    vector<queue<Node<int>>> &queue2,
    int num_of_workers,
    int id)
{
    chrono::milliseconds ms = 5ms; // delay
    queue<Node<int>> q1 = queue1[id];
    queue<Node<int>> q2 = queue2[id];
    Node<int> selected_node;
    bool done = false;
    int value_to_find_counts_2 = 0;
    while (!done)
    {
        while (!q1.empty())
        {
            delay(ms);
            q1_mtx.lock();
            selected_node = q1.front();
            q1.pop();
            q1_mtx.unlock();
            if (selected_node.get_value() == value_to_find)
                value_to_find_counts_2++;

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
        for (int i = 0; i < num_of_workers; i++)
        {
            if (i != id)
            {
                done &= !queue1[i].empty();
                if (!done && queue1[i].size() > 3)
                {
                    for (int j = 0; j < 2; j++)
                    {
                        q1.push(queue1[i].front());
                        queue1[i].pop();
                    }
                    break;
                }
            }
        }
        q1_mtx.unlock();

        queueShift.lock();
        if (done)
        {
            queue<Node<int>> t = q1;
            q1 = q2;
            q2 = t;
            if (!q1.empty())
                done = false;
        }
        queueShift.unlock();
    }

    checkComplete_mtx.lock();
    value_to_find_counts += value_to_find_counts_2;
    checkComplete_mtx.unlock();
}

class Worker:public ff_node {
    private:
        int t;
    public:
        Worker(int tt=-1) {
            t=tt;
        }
        int svc_init() {
            if(t > -1) set_id(t);
            return 0;
        }
        void set_id(ssize_t id) {
            ff_node::set_id(id);
        }
        int run() { return ff_node::run(); } 
        int wait() { return ff_node::wait(); } 
        void *svc(void*) {
            BFS(
                std::ref(value_to_find),
                std::ref(value_to_find_counts),
                std::ref(visited),
                std::ref(g),
                std::ref(queue1),
                std::ref(queue2),
                num_of_workers,
                get_my_id()
            );
            return 0;
        }
};

int main(int argc, char *argv[])
{
    starting_node = atoi(argv[1]);
    value_to_find = atoi(argv[2]);
    num_of_workers = atoi(argv[3]);
    cout << "Starting from: " << starting_node << endl;

    for (auto n : nodes)
    {
        g.addNode(n[0], n[1]);
    }
    for (auto e : edges)
    {
        g.addEdge(e[0], e[1]); // A -> B
        g.addEdge(e[1], e[0]); // A -> B -> A
    }
    int rows = sizeof nodes / sizeof nodes[0];

    bool found = false;
    for (int x = 0; x < rows; ++x)
        if (nodes[x][0] == starting_node)
            found = true;

    if (!found)
    {
        std::cout << "Starting node " << starting_node << " is not one of the graph edge\n";
        return 0;
    }
    cout << "Value to find: " << value_to_find << endl;
    cout << "No. of Workers: " << num_of_workers << endl;

    Node<int> source = g.getNode(starting_node);
    visited.resize(rows);
    queue1.resize(num_of_workers);
    queue2.resize(num_of_workers);

    {
        utimer ut("PAR Part");
        visited[source.get_node_id()] = true;
        if(source.get_value() == value_to_find) value_to_find_counts++;

        vector<Edge> init_edges = source.getChild();
        for (int i = 0; i < init_edges.size(); i++)
        {
            int edge_id = init_edges[i].get_edge_id();
            if (!visited[edge_id])
            {
                queue1[num_of_workers > 1 ? i % num_of_workers : 0].push(g.getNode(edge_id));
                visited[edge_id] = true;
            }
        }


        ff_farm farm;
        vector<ff_node*> w;
        for(int i=0; i < num_of_workers; i++) {
            w.push_back(new Worker(i));
        }
        farm.add_workers(w);
        if(farm.run_then_freeze() < 0) {
            cout << "Error";
            return 0;
        } else {
            farm.wait_freezing();
        }

        cout << endl
             << value_to_find << " is found " << value_to_find_counts
             << " time" << (value_to_find_counts < 2 ? "" : "s") << endl;
    }

    return 0;
}