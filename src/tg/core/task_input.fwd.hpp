#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

template <typename T>
class TaskInput
{
public:
    using ValueType = T;
    static_assert(!std::is_reference_v<T>, "T in TaskInput<T> cannot be a reference type");
    static_assert(!std::is_void_v<T>, "T in TaskInput<T> cannot be void");
    static_assert(!std::is_const_v<T>, "T in TaskInput<T> cannot be const-qualified");
    static_assert(!std::is_volatile_v<T>, "T in TaskInput<T> cannot be volatile-qualified");

public:
    TaskInput(Context& ctx, const std::string& name);
    const T& operator*() const;
    const T* operator->() const;

private:
    TaskInput(const TaskInput&) = delete;
    TaskInput(TaskInput&&) = delete;
    TaskInput& operator=(const TaskInput&) = delete;
    TaskInput& operator=(TaskInput&&) = delete;

private:
    std::shared_ptr<T> m_data;
};

} // namespace tg::core
