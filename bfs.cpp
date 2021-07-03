
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

    void BFS(int &value_to_find, int &found_value_count, vector<bool> &visited, list<int> *adj, vector<queue<int>>& queue1, vector<queue<int>>& queue2, int nw, int j)
    {
        queue<int> q1 = queue1[j];
        queue<int> q2 = queue2[j];
        bool completed = false;
        int a = 0;
        std::chrono::milliseconds ms = 10ms;

        while(!completed) {
            while (!q1.empty())
            {
                delay(ms);
                QLock.lock();
                a = q1.front();
                q1.pop();
                QLock.unlock();

                if (values2[a] == value_to_find)
                {
                    found_value_count++;
                }
                list<int>::iterator i;
                for (i= adj[a].begin(); i != adj[a].end(); ++i)
                {
                    if (!visited[*i])
                    {
                        PLock.lock();
                        visited[*i] = true;
                        q2.push(*i);
                        PLock.unlock();
                    }
                }
            }
            completed = true;
            SQueue.lock();
            for (int i = 0; i < nw; ++i)
            {
                completed &= !queue1[i].empty();
                if(!completed && queue1[i].size() > 5) {
                    for(int n=0; n<3; n++) {
                        q1.push(queue1[i].front());
                        queue1[i].pop(); 
                    }
                    break;
                }
            }
            SQueue.unlock();

            if(completed) {
                queue<int> qq = q1;
                q1 = q2;
                q2 = qq;
                if(!q1.empty()) completed = false;
            }
        }
    }
}