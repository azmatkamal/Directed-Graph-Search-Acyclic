#include <climits>
#include <iostream>
#include <queue>
#include <vector>
#include "Node.cpp"


namespace Graph_NS
{

    template <class T>
    class Graph
    {
        private:
            std::vector<Node_NS::Node<T>> nodes;
            // this propriety means that the graph is an directed graph
            // but at the moment this is the only type of graph implemented
            // in the library
            bool directed;

            Node_NS::Node<T> *find_node(Node_NS::Node<T> &node, T value)
            {
                if (node.value == value)
                {
                    return &node;
                }
                for (auto &child : node.children)
                {
                    auto *found = find_node(child, value);
                    if (found != nullptr)
                        return found;
                }
                return nullptr;
            }

            // void dfs_helper(Node_NS::Node<T> const &node, std::vector<T> &result)
            // {
            //     result.push_back(node.value);
            //     std::vector<T> values;
            //     for (auto child : node.children)
            //         values.push_back(child.value);
            //     for (auto &other : node.children)
            //         dfs_helper(other, result);
            // }

            void bfs_helper(Node_NS::Node<T> const &node, std::vector<T> &path)
            {
                std::deque<Node_NS::Node<T>> to_visit_queue;
                to_visit_queue.push_back(node);
                while (!to_visit_queue.empty())
                {
                    auto to_visit = to_visit_queue.front();
                    to_visit_queue.pop_front();
                    path.push_back(to_visit.value);
                    for (auto child : to_visit.children)
                    {
                        to_visit_queue.push_back(child);
                    }
                }
            }

        public:
            Graph(bool directed) : directed(directed) {}

            void add_edge(T u, T v)
            {
                if (!nodes.empty())
                {
                    for (auto &node : nodes)
                    {
                        auto *found = find_node(node, u);
                        if (found != nullptr)
                        {
                            found->addNode(v);
                            return;
                        }
                    }
                }
                this->nodes.emplace_back(u);
                auto &node = this->nodes.back();
                node.addNode(v);
            }

            std::vector<std::vector<T>> bfs()
            {
                std::vector<std::vector<T>> paths;
                paths.reserve(nodes.size());
                for (auto node : nodes)
                {
                    std::vector<T> path;
                    this->bfs_helper(node, path);
                    paths.push_back(std::move(path));
                }
                return paths;
            }
    };
}; 