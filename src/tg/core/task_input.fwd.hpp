#pragma once
#include "tg/core/fwd.hpp"
#include "tg/core/task_data.hpp"

namespace tg::core
{

template <typename T>
class TaskInput final : public TaskData
{
public:
    using ValueType = T;
    static_assert(!std::is_reference_v<T>, "T in TaskInput<T> cannot be a reference type");
    static_assert(!std::is_void_v<T>, "T in TaskInput<T> cannot be void");
    static_assert(!std::is_const_v<T>, "T in TaskInput<T> cannot be const-qualified");
    static_assert(!std::is_volatile_v<T>, "T in TaskInput<T> cannot be volatile-qualified");

public:
    explicit TaskInput(const std::string& name);
    ~TaskInput();
    const T& operator*() const;
    const T* operator->() const;

private:
    TaskInput(const TaskInput&) = delete;
    TaskInput(TaskInput&&) = delete;
    TaskInput& operator=(const TaskInput&) = delete;
    TaskInput& operator=(TaskInput&&) = delete;
};

} // namespace tg::core
