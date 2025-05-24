#include <stdexcept>
#include <limits>
#include <cstring>
#include "task_graph_cpp/task_graph.hpp"
#include "utils/hash_fnv1a.hpp"

namespace task_graph_cpp
{

using NodeInfoPtr = NodeSet::NodeInfoPtr;
using EdgeTypeHash = NodeSet::EdgeTypeHash;

namespace detail
{
bool contains_int(const std::unordered_set<int>& s, int value)
{
    return s.find(value) != s.end();
}
// bool contains_intpair(const std::unordered_set<std::pair<int, int>>& s, int first, int second)
// {
//     return s.find(std::make_pair(first, second)) != s.end();    
// }

std::unordered_set<std::pair<NodeType, NodeType>, EdgeTypeHash> init_valid_edge_types()
{
    std::unordered_set<std::pair<NodeType, NodeType>, EdgeTypeHash> valid_edge_types;
    // globals
    valid_edge_types.insert(std::make_pair(NodeType::GlobalStart, NodeType::GlobalInput));
    valid_edge_types.insert(std::make_pair(NodeType::GlobalOutput, NodeType::GlobalStop));
    constexpr const NodeType input_like[] = {
        NodeType::Data,
        NodeType::DataPlugin,
        NodeType::GlobalInput,
    };
    constexpr const NodeType output_like[] = {
        NodeType::Data,
        NodeType::DataPlugin,
        NodeType::GlobalOutput,
    };
    constexpr const NodeType task_like[] = {
        NodeType::Task,
        NodeType::TaskPlugin,
    };
    for (const auto& task : task_like)
    {
        for (const auto& input : input_like)
        {
            valid_edge_types.insert(std::make_pair(input, task));
        }
        for (const auto& output : output_like)
        {
            valid_edge_types.insert(std::make_pair(task, output));
        }
        valid_edge_types.insert(std::make_pair(task, NodeType::Barrier));
        valid_edge_types.insert(std::make_pair(NodeType::Barrier, task));
    }
    return valid_edge_types;
}

} // namespace detail (inside task_graph_cpp)

NodeSet::NodeSet()
    : m_nodes{}
    , m_edges{}
    , m_valid_edge_types{detail::init_valid_edge_types()}
{
    this->add_node(NodeType::GlobalStart);
    this->add_node(NodeType::GlobalStop);
}

NodeInfoPtr NodeSet::get_global_start() const
{
    return m_nodes.at(0);
}

NodeInfoPtr NodeSet::get_global_stop() const
{
    return m_nodes.at(1);
}

int NodeSet::node_count() const
{
    return static_cast<int>(m_nodes.size());
}

NodeInfoPtr NodeSet::try_get_node(int index) const
{
    if (index < 0)
    {
        return nullptr;
    }
    size_t u_index = static_cast<size_t>(index);
    if (u_index >= m_nodes.size())
    {
        return nullptr;
    }
    return m_nodes.at(index);
}

NodeInfoPtr NodeSet::add_node(NodeType node_type)
{
    size_t sz = m_nodes.size();
    if (sz >= std::numeric_limits<int>::max())
    {
        throw std::runtime_error("NodeSet: too many nodes");
    }
    int index = static_cast<int>(sz);
    NodeInfoPtr node = std::make_shared<NodeInfo>();
    node->m_index = index;
    node->m_type = node_type;
    m_nodes.push_back(node);
    return node;
}
    
void NodeSet::add_link(NodeInfoPtr node_from, NodeInfoPtr node_to)
{
    if (!node_from || !node_to)
    {
        throw std::invalid_argument("add_link: node pointer cannot be null");
    }
    int index_from = node_from->m_index;
    int index_to = node_to->m_index;
    if (index_from == index_to)
    {
        throw std::runtime_error("Cannot link a node to itself");
    }
    auto p = std::make_pair(index_from, index_to);
    if (m_edges.find(p) != m_edges.end())
    {
        return;
    }
    auto q = std::make_pair(index_to, index_from);
    if (m_edges.find(q) != m_edges.end())
    {
        throw std::runtime_error("A link in the opposite direction already exists");
    }
    m_edges.insert(p);
    m_nodes.at(index_from)->m_outflows.insert(index_to);
    m_nodes.at(index_to)->m_inflows.insert(index_from);
}

void NodeSet::get_inflows(int index, std::vector<NodeInfoPtr>& inflows) const
{
    auto node = this->m_nodes.at(index);
    const auto& inflow_indices = node->m_inflows;
    inflows.clear();
    inflows.reserve(inflow_indices.size());
    for (int inflow_index : inflow_indices)
    {
        inflows.push_back(this->m_nodes.at(inflow_index));
    }
}

void NodeSet::get_outflows(int index, std::vector<NodeInfoPtr>& outflows) const
{
    auto node = this->m_nodes.at(index);
    const auto& outflow_indices = node->m_outflows;
    outflows.clear();
    outflows.reserve(outflow_indices.size());
    for (int outflow_index : outflow_indices)
    {
        outflows.push_back(this->m_nodes.at(outflow_index));
    }
}

size_t NodeSet::EdgeHash::operator()(const std::pair<int, int>& edge) const
{
    // GCC hash is identity cast, not usable. Need something else.
    using Fnv1a = utils::hashing::fnv1a::Fnv1aEval;
    return static_cast<size_t>(Fnv1a{}.transform(edge));
}

size_t NodeSet::EdgeTypeHash::operator()(const std::pair<NodeType, NodeType>& edge) const
{
    using Fnv1a = utils::hashing::fnv1a::Fnv1aEval;
    auto int_pair = std::make_pair(static_cast<int>(edge.first), static_cast<int>(edge.second));
    return static_cast<size_t>(Fnv1a{}.transform(int_pair));
}

bool NodeSet::validate(IssueReporterCB issue_reporter) const
{
    return this->validate_node_identity(issue_reporter) &&
           this->validate_edges_against_flows(issue_reporter) &&
           this->validate_flows_against_edges(issue_reporter);
}

bool NodeSet::validate_node_identity(IssueReporterCB issue_reporter) const
{
    char msg[1024u] = {};
    const size_t sz = m_nodes.size();
    if (sz >= std::numeric_limits<int>::max())
    {
        snprintf(msg, sizeof(msg), "NodeSet: too many nodes: %zu", sz);
        issue_reporter(std::string(msg));
        return false;
    }
    bool good = true;
    for (size_t idx = 0; idx < sz; ++idx)
    {
        auto node = m_nodes.at(idx);
        int index = static_cast<int>(idx);
        if (!node)
        {
            snprintf(msg, sizeof(msg), "Node %d is null", index);
            issue_reporter(std::string(msg));
            good = false;
            continue;
        }
        if (node->m_index != index)
        {
            snprintf(msg, sizeof(msg), "Node %d index mismatch: actual: %d", index, node->m_index);
            issue_reporter(std::string(msg));
            good = false;
        }
        if (node->m_type == NodeType::None)
        {
            snprintf(msg, sizeof(msg), "Node %d has no flags set", index);
            issue_reporter(std::string(msg));
            good = false;
        }
    }
    return good;
}

bool NodeSet::validate_edges_against_flows(IssueReporterCB issue_reporter) const
{
    char msg[1024u] = {};
    bool good = true;
    for (const auto& node : m_nodes)
    {
        int index = node->m_index;
        for (int outflow_index : node->m_outflows)
        {
            auto p = std::make_pair(index, outflow_index);
            if (m_edges.find(p) == m_edges.end())
            {
                snprintf(msg, sizeof(msg), "At node %d, outflow %d not found in edges", index, outflow_index);
                issue_reporter(std::string(msg));
                good = false;
            }
        }
        for (int inflow_index : node->m_inflows)
        {
            auto p = std::make_pair(inflow_index, index);
            if (m_edges.find(p) == m_edges.end())
            {
                snprintf(msg, sizeof(msg), "At node %d, inflow %d not found in edges", index, inflow_index);
                issue_reporter(std::string(msg));
                good = false;
            }
        }
    }
    return good;
}

bool NodeSet::validate_flows_against_edges(IssueReporterCB issue_reporter) const
{
    char msg[1024u] = {};
    const int count = this->node_count();
    bool good = true;
    for (const auto& [index_from, index_to] : m_edges)
    {
        if (index_from < 0 || index_from >= count ||
            index_to < 0 || index_to >= count)
        {
            snprintf(msg, sizeof(msg), "Invalid node index in edges: (%d, %d), node count is %d", index_from, index_to, count);
            issue_reporter(std::string(msg));
            good = false;
            continue;
        }
        auto node_from = m_nodes.at(index_from);
        auto node_to = m_nodes.at(index_to);
        if (!detail::contains_int(node_from->m_outflows, index_to))
        {
            snprintf(msg, sizeof(msg), "Edge (%d, %d) not found in outflows of Node %d", index_from, index_to, index_from);
            issue_reporter(std::string(msg));
            good = false;
        }
        if (!detail::contains_int(node_to->m_inflows, index_from))
        {
            snprintf(msg, sizeof(msg), "Edge (%d, %d) not found in inflows of Node %d", index_from, index_to, index_to);
            issue_reporter(std::string(msg));
            good = false;
        }
    }
    return good;
}

bool NodeSet::validate_by_type(IssueReporterCB issue_reporter) const
{
    char msg[1024u] = {};
    bool good = true;
    for (const auto& node : m_nodes)
    {
        const NodeType nodetype = node->m_type;
        const auto index = node->m_index;
        const auto& inflows = node->m_inflows;
        const auto& outflows = node->m_outflows;
        switch (nodetype)
        {
            case NodeType::GlobalStart:
            {
                if (index != 0)
                {
                    snprintf(msg, sizeof(msg), "GlobalStart node only allowed at index 0, found at %d", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                if (!inflows.empty())
                {
                    snprintf(msg, sizeof(msg), "GlobalStart node %d cannot have inflows", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                if (outflows.empty())
                {
                    snprintf(msg, sizeof(msg), "Warning: GlobalStart node %d has no outflow, graph is likely non-connected", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                for (int outflow_index : outflows)
                {
                    auto outflow_node = m_nodes.at(outflow_index);
                    auto outflow_type = outflow_node->m_type;
                    if (outflow_node->m_type != NodeType::GlobalInput)
                    {
                        snprintf(msg, sizeof(msg), 
                            "GlobalStart node %d contains outflow to node %d which is not GlobalInput. "
                            "Actual NodeType: %d", index, outflow_index, static_cast<int>(outflow_type));
                        issue_reporter(std::string(msg));
                        good = false;
                    }
                }
                break; //switch(nodetype)
            }
            case NodeType::GlobalStop:
            {
                if (index != 1)
                {
                    snprintf(msg, sizeof(msg), "GlobalStop node only allowed at index 1, found at %d", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                if (!outflows.empty())
                {
                    snprintf(msg, sizeof(msg), "GlobalStop node %d cannot have outflows", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                if (inflows.empty())
                {
                    snprintf(msg, sizeof(msg), "Warning: GlobalStop node %d has no inflow, graph is likely non-connected", index);
                    issue_reporter(std::string(msg));
                    good = false;
                }
                for (int inflow_index : inflows)
                {
                    auto inflow_node = m_nodes.at(inflow_index);
                    auto inflow_type = inflow_node->m_type;
                    if (inflow_node->m_type != NodeType::GlobalOutput)
                    {
                        snprintf(msg, sizeof(msg), 
                            "GlobalStop node %d contains inflow from node %d which is not GlobalOutput. "
                            "Actual NodeType: %d", index, inflow_index, static_cast<int>(inflow_type));
                        issue_reporter(std::string(msg));
                        good = false;
                    }
                }
                break; //switch(nodetype)
            }
            default:
            {
                snprintf(msg, sizeof(msg), "Node %d type is bad. Actual NodeType: %d", index, static_cast<int>(nodetype));
                issue_reporter(std::string(msg));
                good = false;
                break; //switch(nodetype)
            }
        }
        ////// WRONG, but use as a template //////
        // switch (nodetype)
        // {
        //     case NodeType::GlobalStart:
        //     case NodeType::GlobalStop:
        //         break;
        //     case NodeType::GlobalInput:
        //     case NodeType::GlobalOutput:
        //         if (!node->m_outflows.empty())
        //         {
        //             snprintf(msg, sizeof(msg), "Node %d of type %d has outflows", node->m_index, static_cast<int>(nodetype));
        //             issue_reporter(std::string(msg));
        //             good = false;
        //         }
        //         break;
        //     case NodeType::Barrier:
        //         if (!node->m_outflows.empty() || !node->m_inflows.empty())
        //         {
        //             snprintf(msg, sizeof(msg), "Node %d of type %d has outflows or inflows", node->m_index, static_cast<int>(nodetype));
        //             issue_reporter(std::string(msg));
        //             good = false;
        //         }
        //         break;
        //     default:
        //         break;
        // }
        // if (nodetype == NodeType::Data || nodetype == NodeType::DataPlugin)
        // {
        //     if (node->m_outflows.size() > 1)
        //     {
        //         snprintf(msg, sizeof(msg), "Node %d of type %d has more than one outflow", node->m_index, static_cast<int>(nodetype));
        //         issue_reporter(std::string(msg));
        //         good = false;
        //     }
        // }
        // else if (nodetype == NodeType::Task || nodetype == NodeType::TaskPlugin)
        // {
        //     if (node->m_inflows.size() > 1)
        //     {
        //         snprintf(msg, sizeof(msg), "Node %d of type %d has more than one inflow", node->m_index, static_cast<int>(nodetype));
        //         issue_reporter(std::string(msg));
        //         good = false;
        //     }
        // }
    }
    return good;
}

} // namespace task_graph_cpp
