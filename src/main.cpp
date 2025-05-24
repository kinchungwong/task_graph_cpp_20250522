#include <iostream>
#include "task_graph_cpp/task_graph.hpp"

int main(int argc, char** argv)
{
    using namespace task_graph_cpp;
    NodeSet nodes;
    auto p1 = nodes.add_node(NodeType::Task);
    auto p2 = nodes.add_node(NodeType::TaskPlugin);
    auto p3 = nodes.add_node(NodeType::Data);
    auto p4 = nodes.add_node(NodeType::DataPlugin);
    nodes.add_link(p1, p2);
    nodes.add_link(p2, p3);
    nodes.add_link(p3, p4);
    nodes.add_link(p4, p1);
    std::cout << p4->m_inflows.size() << std::endl;
    std::cout << p4->m_outflows.size() << std::endl;
    auto reporter = [](const std::string& msg) {
        std::cerr << "Error: " << msg << std::endl;
    };
    if (!nodes.validate(reporter))
    {
        std::cerr << "Validation failed!" << std::endl;
        return 1;
    }
    return 0;
}
