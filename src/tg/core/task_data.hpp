#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

/**
 * @brief Represents a single piece of data that a Task can use.
 * 
 * @details
 * TaskData is an item in TaskDataSet, which serves two purposes,
 * at design time and at execution time.
 * 
 * At design time, TaskData conveys information about an input
 * or output that will be used by a Task.
 * 
 * At execution time, TaskData operates similar to a "dead letter
 * box":
 * (1) the Executor populates the inputs from the global dataset; <br/>
 * (2) the task uses the inputs to produce the outputs;  <br/>
 * (3) the Executor then copies the output into the global dataset. <br/>
 * (4) TaskDataSet is cleared by the Executor after execution. <br/>
 * 
 * @todo
 * Currently, TaskData cannot distinguish between actions performed
 * by Executor and actions performed by Task.
 * 
 * @todo
 * Currently, user-defined data validation callback has not been implemented.
 */
class TaskData
{
public:
    using MutexType = std::mutex;
    using LockType = std::unique_lock<MutexType>;

public:
    TaskData(const std::string& name, TaskDataFlags flags);

    virtual ~TaskData();

    /**
     * @brief Prevents further modifications to the metadata of this TaskData.
     */
    void freeze_metadata();

    /**
     * @brief 
     */
    bool try_assign(std::shared_ptr<void> value, std::type_index actual_type);

    /**
     * @brief Reads out the value of this TaskData.
     */
    bool try_get(std::shared_ptr<void>& out_value, std::type_index& out_type) const;

    /**
     * @brief Release data ownership.
     * @note If the data is still in active use by other tasks, its shared_ptr will
     * be owned by the global dataset.
     */
    void release();

private:
    TaskData(const TaskData&) = delete;
    TaskData& operator=(const TaskData&) = delete;
    TaskData(TaskData&&) = delete;
    TaskData& operator=(TaskData&&) = delete;

private:
    mutable MutexType m_mutex;
    std::string m_name;  ///< Name of the data item.
    TaskDataFlags m_flags;  ///< Flags associated with the data item.
    std::optional<std::type_index> m_expected;  ///< Expected type of the data item.
    // ValidatorPtr m_validator;  ///< Optional validator for the data item.
    std::type_index m_actual; ///< Actual type of the data item, set after the first call to set().
    std::shared_ptr<void> m_value;  ///< Actual value of the data item.
};

} // namespace tg::core
