#pragma once
#include "tg/core/task_input.fwd.hpp"
#include "tg/core/context.hpp"

namespace tg::core
{

template <typename T>
TaskInput<T>::TaskInput(Context& ctx, const std::string& name)
{
    auto p = ctx.get_data(name, std::type_index(typeid(T)));
    if (!p)
    {
        throw std::runtime_error("TaskInput<T>::ctor() : data for " + name + " is not of type " + typeid(T).name());
    }
    // Note: a non-null shared_ptr<void> casting to T* does not throw.
    m_data = std::static_pointer_cast<T>(p);
}

template <typename T>
const T& TaskInput<T>::operator*() const
{
    if (!m_data)
    {
        throw std::runtime_error("TaskInput<T>::operator*() : m_data is null.");
    }
    return *m_data;
}

template <typename T>
const T* TaskInput<T>::operator->() const
{
    if (!m_data)
    {
        throw std::runtime_error("TaskInput<T>::operator*() : m_data is null.");
    }
    return m_data.get();
}

} // namespace tg::core
