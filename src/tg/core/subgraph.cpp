#include "tg/core/subgraph.hpp"
#include "tg/core/task.hpp"
#include "tg/core/task_dataset.hpp"

namespace tg::core
{

Subgraph::Subgraph()
{
}

Subgraph::~Subgraph()
{
}

void Subgraph::add_task(TaskPtr task)
{
    auto dataset = task->get_dataset();
    std::vector<TaskDataPtr> all_data;
    dataset->get_all(all_data);
    for (const auto& data : all_data)
    {
        //! @todo
    }
    m_tasks.emplace_back(std::move(task));
}

void Subgraph::add_input(const std::string& name)
{
}

void Subgraph::add_output(const std::string& name)
{
}

} // namespace tg::core
