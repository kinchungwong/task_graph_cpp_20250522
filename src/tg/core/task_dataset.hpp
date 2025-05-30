#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

/**
 * @brief A collection of TaskData objects that a Task can use.
 *
 * @details
 * Each TaskDataSet instance must be bound to a Task instance. Failure
 * to observe this rule can lead to undefined algorithm behavior.
 * 
 * TaskDataSet serves two purposes, at design time and at execution time,
 * and enforces consistency between them.
 * 
 * At design time, TaskDataSet captures the metadata about the inputs
 * and outputs of a task, such as their names, types, and validation rules.
 * 
 * At execution time, TaskDataSet is temporarily populated with the actual
 * inputs, and captures the outputs produced by the task. The inputs are
 * assigned immediately before task execution, and the outputs are
 * transferred to the global dataset after execution.
 */
class TaskDataSet
{
public:
    using MutexType = std::mutex;
    using LockType = std::unique_lock<MutexType>;

public:
    TaskDataSet();
    ~TaskDataSet();

public:
    /**
     * @brief Adds a TaskData object to the dataset, at design time.
     */
    void add(TaskDataPtr data);

    /**
     * @brief Prevents further modifications to the list of TaskData.
     */
    void freeze_add();

    /**
     * @brief Access TaskData by index.
     * @param index The index of the TaskData, which is the zero-based position
     * of the TaskData in the dataset in the same order of insertion.
     */
    TaskDataPtr at(int index) const;

    /**
     * @brief Access all TaskData items.
     * @param out_items A caller-provided empty vector that will be populated
     *                  with all TaskData items.
     * @details This method is used by the executor to populate all inputs and 
     * retrieve all outputs before and after task execution.
     */
    void get_all(std::vector<TaskDataPtr>& out_items) const;

private:
    TaskDataSet(const TaskDataSet&) = delete;
    TaskDataSet& operator=(const TaskDataSet&) = delete;
    TaskDataSet(TaskDataSet&&) = delete;
    TaskDataSet& operator=(TaskDataSet&&) = delete;

private:
    mutable MutexType m_mutex;
    bool m_add_frozen;

    /**
     * @note Optional: check for duplicated TaskData instance (by reference),
     * since unchecked duplicates can lead to unexpected algorithm behavior.
     */
    bool m_check_duplicate;

    /**
     * @brief List of TaskData. This list is append-only at design time.
     * Modification is not allowed after calling freeze_add().
     */
    std::vector<TaskDataPtr> m_data;

    /**
     * @brief A mapping of names to the list index. Multiple names
     * can refer to the same index.
     */
    std::unordered_map<std::string, int> m_names;
};

} // namespace tg::core
