#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <functional>

namespace task_graph_cpp
{

enum class NodeType : uint32_t
{
    None = 0,
    GlobalStart,
    GlobalStop,
    GlobalInput,
    GlobalOutput,
    Barrier,
    Data,
    DataPlugin,
    Task,
    TaskPlugin
};

enum class RelationCount : int
{
    Any = -1,
    Zero = 0,
    One = 1,
    Many = 2
};

struct NodeInfo
{
    int m_index;
    NodeType m_type;
    std::unordered_set<int> m_outflows;
    std::unordered_set<int> m_inflows;
};

class NodeSet
{
public:
    using NodeInfoPtr = std::shared_ptr<NodeInfo>;
    using IssueReporterCB = std::function<void(const std::string&)>;

public:
    struct EdgeHash
    {
        size_t operator()(const std::pair<int, int>& edge) const;
    };

    struct EdgeTypeHash
    {
        size_t operator()(const std::pair<NodeType, NodeType>& edge) const;
    };

    using ValidEdgeTable = std::unordered_map<
        std::pair<NodeType, NodeType>,
        std::pair<RelationCount, RelationCount>,
        EdgeTypeHash>;

public:
    NodeSet();
    NodeInfoPtr get_global_start() const;
    NodeInfoPtr get_global_stop() const;
    NodeInfoPtr add_node(NodeType flags);
    int node_count() const;
    NodeInfoPtr try_get_node(int index) const;
    void add_link(NodeInfoPtr node_from, NodeInfoPtr node_to);
    void get_inflows(int index, std::vector<NodeInfoPtr>& inflows) const;
    void get_outflows(int index, std::vector<NodeInfoPtr>& outflows) const;

public:
    bool validate(IssueReporterCB issue_reporter) const;
    bool validate_node_identity(IssueReporterCB issue_reporter) const;
    bool validate_edges_against_flows(IssueReporterCB issue_reporter) const;
    bool validate_flows_against_edges(IssueReporterCB issue_reporter) const;
    bool validate_by_type(IssueReporterCB issue_reporter) const;

private:
    std::vector<NodeInfoPtr> m_nodes;
    std::unordered_set<std::pair<int, int>, EdgeHash> m_edges;
    ValidEdgeTable m_valid_edge_table;
};

} // namespace task_graph_cpp
