#pragma once
#include <string>
#include <vector>
#include <unordered_set>

#include "tg/facade/facade_common.hpp"

namespace tg::facade
{

enum class EdgeType
{
    None = 0,
    DataToTask = 1,
    TaskToData = 2
};

struct EdgeInfo
{
    EdgeType etype;
    std::string from_name;
    std::string to_name;
};

class Subgraph
{
public:
    const std::string name;
    explicit Subgraph(const std::string& name, std::initializer_list<TaskInfo> tasks)
        : name{name}
    {
        for (const auto& task : tasks)
        {
            this->add_task(task);
        }
    }

    void add_task(const TaskInfo& task)
    {
        m_task_names.insert(task.name);
        for (const auto& input : task.inputs)
        {
            m_data_names.insert(input.name);
            m_edges.push_back({EdgeType::DataToTask, input.name, task.name});
        }
        for (const auto& output : task.outputs)
        {
            m_data_names.insert(output.name);
            m_edges.push_back({EdgeType::TaskToData, task.name, output.name});
        }
    }

private:
    std::unordered_set<std::string> m_data_names;
    std::unordered_set<std::string> m_task_names;
    std::vector<EdgeInfo> m_edges;
};

} // namespace tg::facade
