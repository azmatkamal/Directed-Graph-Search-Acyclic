#include <iostream>
#include <list>
#include <queue>
#include <vector>
#include "../data.cpp"

using namespace std;

class Graph
{

private:
    int vertices;
    list<int> *adjlist;
    vector<bool> visited;

public:
    Graph()
    {
    }

    Graph(int nodes)
    {
        adjlist = new list<int>[nodes];
        visited.resize(nodes, false);
        vertices = nodes;
    }

    ~Graph()
    {
        delete[] adjlist;
    }

    void AddEdge(int src, int dst)
    {
        adjlist[src].push_back(dst);
        adjlist[dst].push_back(src);
    }

    void BFS(int source)
    {

        queue<int> Q;
        visited[source] = true;
        Q.push(source);

        while (!Q.empty())
        {

            int node = Q.front();
            Q.pop();
            cout << node << " ";

            for (auto &adj_node : adjlist[node])
            {
                if (!visited[adj_node])
                {
                    visited[adj_node] = true;
                    Q.push(adj_node);
                }
            }
        }
        // Reset the visited array for next iteration of breadth first search
        fill(visited.begin(), visited.end(), false);
    }
};

int main()
{

    Graph g(22);

    for (auto n : nodes)
    {
        g.AddEdge(n[0], n[1]);
    }

    cout << "BFS Graph Traversal" << endl;
    cout << "Source Node(0): ";
    g.BFS(0);
    cout << endl;
    cout << "Source Node(3): ";
    g.BFS(3);
    cout << endl;

    return 0;
}
