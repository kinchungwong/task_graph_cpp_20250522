#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

class Task
{
public:
    virtual ~Task();
    TaskDataSetPtr get_dataset() const;
    virtual void on_execute() = 0;

protected:
    Task();

private:
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) = delete;
    Task& operator=(Task&&) = delete;

protected:
    /**
     * @brief List of data that this task will use.
     *
     * @details
     * Each task is bound to a dataset object to enforce consistency between
     * design-time and execute-time. The TaskDataSet object only owns the
     * data temporarily: the actual data is assigned immediately before task
     * execution, and removed immediately after execution.
     */
    const TaskDataSetPtr m_dataset;
};

} // namespace tg::core
