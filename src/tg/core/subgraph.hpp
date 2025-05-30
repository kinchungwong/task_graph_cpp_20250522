#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

class Subgraph
{
public:
    Subgraph();
    ~Subgraph();

public:
    void add_task(TaskPtr task);
    void add_input(const std::string& name);
    void add_output(const std::string& name);

private:
    std::vector<TaskPtr> m_tasks;
};

} // namespace tg::core
