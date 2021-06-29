#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <chrono>
#include <ctime>
#include <thread>
#include <vector>
#include <math.h>
#include <mutex>

#include "utimer.cpp"

using namespace std;

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
    {14, 25}
};

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
    } else {
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

    auto visit_adj = [&visited, &queue, &adj, &s, &ms, &value_to_find, &found_value_count, &mtx](RANGE range) { // function to compute a chunk
        mtx.lock();
        list<int>::iterator i; // 'i' will be used to get all adjacent - vertices of a vertex
        i = std::next(adj[s].begin(), range.start);
        for (int a = range.start;  a < range.end; a++ )
        {
            if (!visited[*i])
            {
                visited[*i] = true;
                queue.push_back(*i);
                for_each(std::begin(values), std::end(values), [&value_to_find, &found_value_count, i](int(&row)[2])
                         {  
                             if(row[0] == *i && row[1] == value_to_find) {
                                found_value_count++;
                             }
                         });
                // delay(ms);
            }
            i++;
        }
        mtx.unlock();
        return;
    };

    cout << "Following is Breadth First Traversal: ";
    while(!queue.empty())
    {
        s = queue.front();
        cout << s << " ";
        queue.pop_front();

        vector<RANGE> ranges(nw); // vector to compute the ranges
        int m = adj[s].size();
        int delta = ceil(m / nw);
        vector<thread> tids;

        for (int i = 0; i < nw; i++)
        {
            ranges[i].start = i * delta;
            ranges[i].end = (i != (nw - 1) ? (i + 1) * delta : m);
        }

        for (int i = 0; i < nw; i++)
        {
            tids.push_back(thread(visit_adj, ranges[i])); // assign chuncks to threads
        }

        for (thread &t : tids)
        {
            t.join(); // await thread termination
        }
    }

    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    // and print time
    cout << endl << "Found " << value_to_find << " at " << found_value_count << " nodes." << endl;
    cout << endl << "Spent " << usec << " usecs - using parallel with " << nw << " threads" << endl;
}