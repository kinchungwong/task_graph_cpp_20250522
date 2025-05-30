#pragma once
#include "tg/core/task_input.fwd.hpp"

namespace tg::core
{

template <typename T>
TaskInput<T>::TaskInput(const std::string& name)
    : TaskData(name)
{
}

template <typename T>
TaskInput<T>::~TaskInput()
{
}

template <typename T>
const T& TaskInput<T>::operator*() const
{
    return *this->operator->();
}

template <typename T>
const T* TaskInput<T>::operator->() const
{
    std::shared_ptr<void> out_value;
    std::type_index out_type{typeid(void)};
    if (!this->try_get(out_value, out_type))
    {
        throw std::runtime_error("TaskInput<T>::operator*() : failed to get value.");
    }
    /**
     * @invariant out_value is not null, enforced by TaskData::try_get()
     */
    if (out_type != std::type_index(typeid(T)))
    {
        std::string str_expected{typeid(T).name()};
        std::string str_actual{out_type.name()};
        throw std::runtime_error("TaskInput<T>::operator*() : type mismatch. Expected: " +
            str_expected + ", got: " + str_actual);
    }
    /**
     * @note Equivalent to std::static_pointer_cast<T>(out_value)
     * @ref https://en.cppreference.com/w/cpp/memory/shared_ptr/pointer_cast
     */
    return static_cast<const T*>(out_value.get());
}

} // namespace tg::core
