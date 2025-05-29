#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

template <typename T>
class TaskOutput
{
public:
    using ValueType = T;
    static_assert(!std::is_reference_v<T>, "T in TaskOutput<T> cannot be a reference type");
    static_assert(!std::is_void_v<T>, "T in TaskOutput<T> cannot be void");
    static_assert(!std::is_const_v<T>, "T in TaskOutput<T> cannot be const-qualified");
    static_assert(!std::is_volatile_v<T>, "T in TaskOutput<T> cannot be volatile-qualified");

public:
    TaskOutput(Context& ctx, const std::string& name);
    ~TaskOutput();

    template <typename... Args>
    T& emplace(Args&&... args);

    T& operator*();
    T* operator->();

private:
    TaskOutput(const TaskOutput&) = delete;
    TaskOutput(TaskOutput&&) = delete;
    TaskOutput& operator=(const TaskOutput&) = delete;
    TaskOutput& operator=(TaskOutput&&) = delete;

private:
    // m_ref_ctx, m_name needed for writing data back to the context in destructor
    Context& m_ctx;
    std::string m_name;
    std::shared_ptr<T> m_data;
};

} // namespace tg::core
