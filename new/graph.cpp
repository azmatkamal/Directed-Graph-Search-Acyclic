#include<iostream>
#include<vector>

#include "edge.cpp"
#include "node.cpp"

template <class NT>
class Graph {
    private:
        std::vector<Node<NT> nodes;
    
    public:
        Graph() {}
        Graph(const std::vector<Node<NT>>& nodes) {
            this->nodes = nodes;
        }
        Graph& addNode(const Node<NT>& node) {
            this->nodes.push_back(node);
            return *this;
        }
        Graph &addNode(int& node_id, const Node<NT> &node)
        {
            this->nodes.push_back(Node<NT>[node_id, data]);
            return *this;
        }
        Graph &addNode(const NT &data)
        {
            this->nodes.push_back(Node<NT>[this->nodes.size(), data]);
            return *this;
        }
}