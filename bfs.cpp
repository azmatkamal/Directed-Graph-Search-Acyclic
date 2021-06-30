
// #include "bfs.h"
#include <iostream>
#include <list>
#include <algorithm>
#include <string>
#include <queue>
#include <chrono>
#include <mutex>
#include <vector>
#include <thread>
#include <ctime>   // For time()
#include <cstdlib> // For srand() and rand()
#include "data.cpp"

using namespace std;

mutex QLock;
mutex PLock;
mutex SQueue;

namespace bfs
{
    void BFS(int &value_to_find, int &found_value_count, vector<bool> &visited, list<int> *adj, vector<queue<int>>& queue1, vector<queue<int>>& queue2, int nw, int j)
    {
        // queue<int> q1 = queue1[j];
        // queue<int> queue2[j] = queue2[j];
        bool completed = false;
        int a = 0;

        // while(!completed) {
            while (!queue1[j].empty())
            {
                QLock.lock();
                // Dequeue a vertex from queue and print it
                a = queue1[j].front();
                // cout << a << " ";
                queue1[j].pop();
                QLock.unlock();

                if (values[a] == value_to_find)
                {
                    found_value_count++;
                }
                list<int>::iterator i;
                // cout << endl << a << " ====== ";
                // Get all adjacent vertices of the dequeued
                // vertex s. If a adjacent has not been visited,
                // then mark it visited and enqueue it
                for (i= adj[a].begin(); i != adj[a].end(); ++i)
                {
                    // cout << *i << " ";
                    if (!visited[*i])
                    {
                        PLock.lock();
                        visited[*i] = true;
                        PLock.unlock();
                        queue2[j].push(*i);
                        // delay(ms);
                    }
                }
                completed = true;
                SQueue.lock();
                for (int i = 0; i < nw; ++i)
                {
                    completed &= !queue1[i].empty();
                    // cout << endl << "Completed " << completed;
                    if(!completed) {
                        // for(int n=0; n<3; n++) {
                            queue1[j].push(queue1[i].front());
                            queue1[i].pop();
                        // }
                        break;
                    }
                }
                SQueue.unlock();

                if(completed) {
                    queue<int> qq = queue1[j];
                    queue1[j] = queue2[j];
                    queue2[j] = qq;
                    cout << endl << "Q size " << a << " " << queue1[j].size() << " " << queue2[j].size();
                    if(!queue1[j].empty()) completed = false;
                }
            }
        // }
        // cout << endl
        //     << "Found '" << value_to_find << "' " << found_value_count << " times." << endl;
    }
}