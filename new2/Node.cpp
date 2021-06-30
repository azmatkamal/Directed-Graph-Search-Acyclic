#include <climits>
#include <iostream>
#include <queue>
#include <vector>

namespace Node_NS
{
    template <class T>
    class Node
    {
        public:
            std::vector<Node<T>> children;
            T value;

            Node() : value(INT_MAX) {}
            Node(T value) : value(value) {}

            void addNode(T value)
            {
                auto node = Node<T>(value);
                this->children.push_back(node);
            }
    };
};