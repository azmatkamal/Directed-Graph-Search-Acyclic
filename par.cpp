#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <list>
#include <pthread.h>

#include "node.cpp"
#include "graph.cpp"
#include "utimer.cpp"
#include "barrier.cpp"

std::mutex q1_mtx;
std::mutex checkComplete_mtx;
std::mutex checkProcess_mtx;

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

void BFS(
    int &value_to_find,
    atomic<int> &value_to_find_counts,
    vector<bool> &visited,
    Graph<int> &g,
    vector<queue<Node<int>>> &queue1,
    vector<queue<Node<int>>> &queue2,
    int num_of_workers,
    int id,
    BarrierWait &br_lock
)
{
    chrono::microseconds us = 500us; // delay
    queue<Node<int>> q1 = queue1[id];
    queue<Node<int>> q2 = queue2[id];
    Node<int> selected_node;
    bool done = false;
    std::atomic<int> value_to_find_counts_2 = 0;
    while(!done) {
        while(!q1.empty()) {
            delay(us);
            #ifdef WITHTIME1
            {
                utimer ut("Time to POP Queue - Mutex");
            #endif
                q1_mtx.lock();
                selected_node = q1.front();
                q1.pop();
                if(selected_node.get_value() == value_to_find) value_to_find_counts_2++;
                q1_mtx.unlock();
            #ifdef WITHTIME1
            }
            #endif

            vector<Edge> child = selected_node.getChild();

            #ifdef WITHTIME
            {
                utimer ut("Time to process Edges - Mutex");
            #endif
                for (int i = 0; i < child.size(); i++)
                {
                    int edge_id = child[i].get_edge_id();
                    if (!visited[edge_id])
                    {
                        checkProcess_mtx.lock();
                        visited[edge_id] = true;
                        checkProcess_mtx.unlock();
                        q2.push(g.getNode(edge_id));
                    }
                }
            #ifdef WITHTIME
            }
            #endif
        }

        done = true;
        for(int i = 0; i < num_of_workers; i++) {
            done &= !queue1[i].empty();
            if(!done && queue1[i].size() > 5) {
                for(int j=0; j < 3; j++)
                {
                    q1_mtx.lock();
                    q1.push(queue1[i].front());
                    queue1[i].pop();
                    q1_mtx.unlock();
                }
                break;
            }
        }

        #ifdef WITHTIME2
        {
            utimer ut("Time to SWAP Queues and Barrier");
        #endif
            if (done)
            {
                br_lock.count_down_and_wait();
                queue<Node<int>> t = q1;
                q1 = q2;
                q2 = t;
                br_lock.count_down_and_wait();
                if(!q1.empty()) done = false;
            }
        #ifdef WITHTIME2
        }
        #endif
    }
    
    if(done) br_lock.decrease_active_threads();
    #ifdef WITHTIME3
    {
        utimer ut("Time to update Counts - Mutex");
    #endif
        checkComplete_mtx.lock();
        value_to_find_counts += value_to_find_counts_2;
        checkComplete_mtx.unlock();
    #ifdef WITHTIME3
    }
    #endif
}

void ProcessGraph(int starting_node, int value_to_find, int num_of_workers, int total_nodes, int min_edges_per_node, int max_edges_per_node)
{
    std::atomic<int> value_to_find_counts = 0;

    vector<thread> threads;
    BarrierWait br_lock(num_of_workers);

    Graph<int> g;
    {
        utimer ut("Graph processing");
        auto filename = "graph_data/graph_data_" + to_string(total_nodes) + "_" + to_string(min_edges_per_node) + "_" + to_string(max_edges_per_node) + ".txt";
        ifstream file(filename);
        int nodeCount;
        file >> nodeCount;

        for (int i = 0; i < nodeCount; ++i)
        {
            int nodeID, val;
            file >> nodeID >> val;
            g.addNode(nodeID, val);
        }
        while (file.peek() != EOF)
        {
            int src, dest;
            file >> src >> dest;
            g.addEdge(src, dest);
            g.addEdge(dest, src);
        }
    }

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
    cout << "No. of Workers: " << num_of_workers << endl;

    vector<bool> visited(total_nodes);
    Node<int> source = g.getNode(starting_node);
    vector<queue<Node<int>>> queue1(num_of_workers);
    vector<queue<Node<int>>> queue2(num_of_workers);

    visited[source.get_node_id()] = true;
    if (source.get_value() == value_to_find)
    {
        value_to_find_counts++;
    }

    {
        utimer ut("Assigning nodes");
        vector<Edge> init_edges = source.getChild();
        for(int i = 0; i < init_edges.size(); i++) {
            int edge_id = init_edges[i].get_edge_id();
            if (!visited[edge_id])
            {
                queue1[num_of_workers > 1 ? i % num_of_workers : 0].push(g.getNode(edge_id));
                visited[edge_id] = true;
            }
        }
    }
    {
        utimer ut("PAR Part");
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
                i,
                std::ref(br_lock)
            );
        }
        for(auto& thread: threads) {
            thread.join();
        }
    }

    cout << value_to_find << " is found " << value_to_find_counts
         << " time" << (value_to_find_counts < 2 ? "" : "s") << endl;
}

int main(int argc, char *argv[])
{
    int starting_node = atoi(argv[1]);
    int value_to_find = atoi(argv[2]);
    int num_of_workers = atoi(argv[3]);
    int total_nodes = atoi(argv[4]);
    int min_edges_per_node = atoi(argv[5]);
    int max_edges_per_node = atoi(argv[6]);
  
    ProcessGraph(starting_node, value_to_find, num_of_workers, total_nodes, min_edges_per_node, max_edges_per_node);

    return 0;
}