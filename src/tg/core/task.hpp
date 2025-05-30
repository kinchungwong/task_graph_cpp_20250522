#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

/**
 * @brief Base class for tasks in the TaskGraph framework.
 *
 * @details
 * The Task object encapsulates a piece of executable code, and also
 * a description of the inputs and outputs used by the task.
 *
 * To ensure correct operations across design time and execute time,
 * each Task instance is bound to an instance of TaskDataSet.
 */
class Task
{
public:
    virtual ~Task();

    /**
     * @brief Returns the TaskDataSet instance bound to this Task.
     */
    TaskDataSetPtr get_dataset() const;

    /**
     * @brief Executes the code.
     * 
     * @details
     * Before execution, the Executor must populate the inputs needed by this
     * task, by copying the data from the global dataset.
     * 
     * After execution, the outputs must be copied to the global dataset. After
     * that, all TaskData instances need to be cleared.
     */
    virtual void on_execute() = 0;

protected:
    Task();

private:
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;
    Task(Task&&) = delete;
    Task& operator=(Task&&) = delete;

private:
    const TaskDataSetPtr m_dataset;
};

} // namespace tg::core
