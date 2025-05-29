#pragma once
#include "tg/core/task_output.fwd.hpp"
#include "tg/core/context.hpp"

namespace tg::core
{

template <typename T>
TaskOutput<T>::TaskOutput(Context& ctx, const std::string& name)
    : m_ctx{ctx}
    , m_name{name}
    , m_data{}
{
    if (!m_ctx.can_write(m_name, std::type_index(typeid(T))))
    {
        throw std::runtime_error("TaskOutput<T>::ctor() : cannot write data for " + m_name + " of type " + typeid(T).name());
    }
}

template <typename T>
TaskOutput<T>::~TaskOutput()
{
    if (m_data)
    {
        m_ctx.set_data(m_name, m_data, std::type_index(typeid(T)));
    }
}

template <typename T>
template <typename... Args>
T& TaskOutput<T>::emplace(Args&&... args)
{
    if (m_data)
    {
        throw std::runtime_error("TaskOutput<T>::emplace() : m_data is already initialized.");
    }
    m_data = std::make_shared<T>(std::forward<Args>(args)...);
    return *m_data;
}

template <typename T>
T& TaskOutput<T>::operator*()
{
    if (!m_data)
    {
        throw std::runtime_error("TaskOutput<T>::operator*() : m_data is null.");
    }
    return *m_data;
}

template <typename T>
T* TaskOutput<T>::operator->()
{
    if (!m_data)
    {
        throw std::runtime_error("TaskOutput<T>::operator*() : m_data is null.");
    }
    return m_data.get();
}

} // namespace tg::core
