#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <ctime>
#include <cstddef>
#include <list>
#include <math.h>
#include <string>
#include <functional>
#include <algorithm>

#include <chrono>
#include <thread>
// using namespace std::chrono_literals;

using namespace std;

#include "myqueue.cpp"
#include "utimer.cpp"
//
// needed something to represent the EOS
// here we use null
//
#define EOS NULL

// #define PROGRESSPRINT

const int TOTAL_NO_OF_ADJACENCY = 22;
const int ADJACENCY_ELEMENTS[TOTAL_NO_OF_ADJACENCY][2] = {
    {0, 1},
    {0, 6},
    {0, 9},
    {0, 11},
    {0, 13},
    {0, 14},
    {1, 2},
    {1, 8},
    {2, 4},
    {3, 5},
    {4, 3},
    {5, 7},
    {8, 3},
    {8, 4},
    {9, 10},
    {9, 11},
    {9, 12},
    {11, 12},
    {11, 14},
    {12, 13},
    {13, 14},
};

int values[][2] = {
    {0, 20},
    {1, 20},
    {2, 30},
    {3, 10},
    {4, 50},
    {5, 30},
    {6, 30},
    {7, 60},
    {8, 25},
    {9, 10},
    {10, 30},
    {11, 30},
    {12, 50},
    {13, 30},
    {14, 25}};

typedef struct
{
    int start;
    int end;
} RANGE;

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

void workers_stage(myqueue<int> &queue_worker, myqueue<int> &queue_drain, std::chrono::milliseconds ms)
{
    auto e = queue_worker.pop();

    while (e != EOS)
    {
        queue_drain.push(e);
        e = queue_worker.pop();
        
    }
    queue_drain.push(EOS);
    return;
};

int main(int argc, char *argv[])
{
    std::chrono::milliseconds ms = 40ms;
    std::mutex mtx;
    int starting_vertex;
    int nw;
    int value_to_find;
    int found_value_count = 0;
    if (argc == 4)
    {
        starting_vertex = atoi(argv[1]);
        cout << "Starting from: " << starting_vertex << endl;
        nw = atoi(argv[2]);
        cout << "No. of workers: " << nw << endl;
        value_to_find = atoi(argv[3]);
        cout << "Value to find: " << value_to_find << endl;
    }
    else
    {
        cout << "Not all params were passed";
        return 0;
    }

    list<int> *adj;         // Lost of Pointer to an array containing adjacency
    list<int> all_elements; // list of elements that are on adjacency

    int V = TOTAL_NO_OF_ADJACENCY;
    adj = new list<int>[V];

    bool *visited = new bool[V]; // visited adjacency
    for (int i = 0; i < V; i++)
        visited[i] = false; // default visited to false

    list<int> queue; // queue of all the adjacency

    for (int i = 0; i < TOTAL_NO_OF_ADJACENCY; ++i)
    {
        adj[ADJACENCY_ELEMENTS[i][0]].push_back(ADJACENCY_ELEMENTS[i][1]); // Add w to v’s list.
        all_elements.push_back(ADJACENCY_ELEMENTS[i][0]);
        all_elements.push_back(ADJACENCY_ELEMENTS[i][1]);
    }

    bool found = (find(all_elements.begin(), all_elements.end(), starting_vertex) != all_elements.end());

    if (!found)
    {
        cout << "Starting vertex " << starting_vertex << " is not one of the graph edge\n";
        return 0;
    }

    auto start = std::chrono::high_resolution_clock::now();
    int s = starting_vertex;
    visited[s] = true;
    queue.push_back(s);

    // MAIN PART START ================================================================
    myqueue<int> queue_drain;                   // input queue for the drain
    std::vector<myqueue<int>> queue_worker(nw); // input queues for the workers
    cout << "Following is Breadth First Traversal: ";
       
    // now start the farm stages threads

    auto source_stage = [&queue_worker, &s, &nw, &adj, &queue, &visited, & ms]()
    {
        while (queue.front() != EOS)
        {
            s = queue.front();
            cout << s << " ";
            queue.pop_front();

            vector<RANGE> ranges(nw); // vector to compute the ranges
            int m = adj[s].size();
            int delta = ceil(m / nw);
            vector<thread> tids;

            int turn = 0;
            list<int>::iterator i;
            for (i = adj[s].begin(); i != adj[s].end(); i++)
            {
                queue_worker[turn].push(*i);
                turn = (turn + 1) % nw;
                if (!visited[*i])
                {
                    // cout << " ========= " << visited[e] << " " << e << endl;
                    visited[*i] = true;
                    queue.push_back(*i);
                    
                }
                #ifdef PROGRESSPRINT
                    std::cout << "Source emitted " << i << std::endl;
                #endif
            }

            #ifdef PROGRESSPRINT
            std::cout << "sent EOS" << std::endl;
            #endif
        }

        for (int i = 0; i < nw; i++)
            queue_worker[i].push(EOS);
    };


    auto drain_stage = [&queue_drain, &queue, &nw, &value_to_find, &found_value_count, &visited, &ms]()
    {
        #ifdef PROGRESSPRINT
            std::cout << "Drain started" << std::endl;
        #endif
        auto e = queue_drain.pop();
        int countEos = 0; 

        do
        {
            if(e == EOS) {
                countEos++;
                if(countEos == nw) {
                    queue.push_back(EOS);
                    break;         // got last EOS: exit from loop
                }
                else
                {                    
                    continue;      // cot first EOS, look for the second one
                }
            }
            for_each(std::begin(values), std::end(values), [&value_to_find, &found_value_count, &e](int(&row)[2])
            {
                // cout << endl << " >>> " << row[0] << row[1] << endl;
                if (row[0] == e && row[1] == value_to_find)
                {
                    found_value_count++;
                }
            });
            #ifdef PROGRESSPRINT
                std::cout << "received " << e << std::endl;
            #endif
            e = queue_drain.pop();
        } while (true);

        return;
    };


    std::thread s0(source_stage); // this is the emitter, source of the stream of tasks  ;

    std::vector<std::thread *> tids(nw);

    for (int i = 0; i < nw; i++)
        tids[i] = new std::thread(workers_stage, std::ref(queue_worker[i]), std::ref(queue_drain), ms);

    std::thread s1(drain_stage); 

    s0.join();

    for (int i = 0; i < nw; i++)
        tids[i]->join();

    s1.join();

    // MAIN PART END ==================================================================

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    // and print time
    cout << endl
         << "Found " << value_to_find << " at " << found_value_count << " nodes." << endl;
    cout << endl
         << "Spent " << usec << " usecs - using parallel with " << nw << " threads" << endl;
}