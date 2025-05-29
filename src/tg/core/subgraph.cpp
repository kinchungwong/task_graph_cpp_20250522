#include "tg/core/subgraph.hpp"
#include "tg/core/task.hpp"

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
    task->on_added(*this);
}

void Subgraph::add_input(const std::string& name)
{
}

void Subgraph::add_output(const std::string& name)
{
}

} // namespace tg::core
