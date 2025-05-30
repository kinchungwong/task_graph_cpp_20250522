#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

#if 0 // NOT IMPLEMENTED
// /**
//  * @todo
//  */
// enum class TaskDataFlags
// {};

// /**
//  * @todo
//  */
// class Validator;
// using ValidatorPtr = std::shared_ptr<Validator>;
#endif // NOT IMPLEMENTED

/**
 * @brief Represents a single piece of data that a Task can use.
 * 
 * @details
 * TaskData is an item in TaskDataSet, which serves two purposes,
 * at design time and at execution time.
 * 
 * At execution time, the Executor will ask TaskDataSet for all
 * TaskData items, and populate the inputs. After execution,
 * outputs will be transferred to the global dataset.
 */
class TaskData
{
public:
    using MutexType = std::mutex;
    using LockType = std::unique_lock<MutexType>;

public:

#if 0 // NOT IMPLEMENTED
    TaskData(
        const std::string& name,
        TaskDataFlags flags = {},
        std::optional<std::type_index> expected_type = std::nullopt,
        ValidatorPtr validator = nullptr
    );
#endif // NOT IMPLEMENTED

    explicit TaskData(const std::string& name);

    virtual ~TaskData();

    /**
     * @brief Prevents further modifications to the metadata of this TaskData.
     */
    void freeze_metadata();
    bool try_assign(std::shared_ptr<void> value, std::type_index actual_type);
    bool try_get(std::shared_ptr<void>& out_value, std::type_index& out_type) const;
    bool consume(std::shared_ptr<void>& out_value, std::type_index& out_type);

private:
    TaskData(const TaskData&) = delete;
    TaskData& operator=(const TaskData&) = delete;
    TaskData(TaskData&&) = delete;
    TaskData& operator=(TaskData&&) = delete;

private:
    mutable MutexType m_mutex;
    std::string m_name;  ///< Name of the data item.
    // TaskDataFlags m_flags;  ///< Flags associated with the data item.
    std::optional<std::type_index> m_expected;  ///< Expected type of the data item.
    // ValidatorPtr m_validator;  ///< Optional validator for the data item.
    std::type_index m_actual; ///< Actual type of the data item, set after the first call to set().
    std::shared_ptr<void> m_value;  ///< Actual value of the data item.
};

} // namespace tg::core
