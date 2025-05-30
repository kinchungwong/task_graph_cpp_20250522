#pragma once
#include <cstdint>

namespace tg::core
{

enum class TaskDataFlags : uint32_t
{
    None = 0u,

    /**
     * @brief Input: the Task requires this data as input.
     * @details The Executor must populate this data before task execution.
     */
    Input = 1u << 0,

    /**
     * @brief Output: the Task produces this data as output.
     * @details After task execution, the Executor copies this data to the
     * global dataset.
     */
    Output = 1u << 1,

    /**
     * @brief Consume: the Task is given exclusive and destructive access to the data.
     * @details After task execution, the data will be put into Expired state.
     */
    Consume = (Input | (1u << 2)), 
};

inline TaskDataFlags operator|(TaskDataFlags lhs, TaskDataFlags rhs)
{
    using UT = std::underlying_type_t<TaskDataFlags>;
    return static_cast<TaskDataFlags>(static_cast<UT>(lhs) | static_cast<UT>(rhs));
}

inline TaskDataFlags operator&(TaskDataFlags lhs, TaskDataFlags rhs)
{
    using UT = std::underlying_type_t<TaskDataFlags>;
    return static_cast<TaskDataFlags>(static_cast<UT>(lhs) & static_cast<UT>(rhs));
}

inline TaskDataFlags operator~(TaskDataFlags rhs)
{
    using UT = std::underlying_type_t<TaskDataFlags>;
    return static_cast<TaskDataFlags>(~static_cast<UT>(rhs));
}

inline bool operator!(TaskDataFlags rhs)
{
    using UT = std::underlying_type_t<TaskDataFlags>;
    return !static_cast<UT>(rhs);
}

} // namespace tg::core
