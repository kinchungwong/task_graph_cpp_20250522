#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

class Task
{
public:
    virtual ~Task();
    virtual void on_added(Subgraph& subgraph) = 0;
    virtual void on_execute(Context& context) = 0;

protected:
    Task();

private:
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) = delete;
    Task& operator=(Task&&) = delete;
};

} // namespace tg::core
