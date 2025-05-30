#pragma once
#include "tg/core/task_output.fwd.hpp"

namespace tg::core
{

template <typename T>
TaskOutput<T>::TaskOutput(const std::string& name)
    : TaskData{name}
{
}

template <typename T>
TaskOutput<T>::~TaskOutput()
{
    /**
     * @note It is not TaskOutput's responsibility to write data back
     * to the context. That responsibility lies with the Executor.
     */
}

template <typename T>
template <typename... Args>
T& TaskOutput<T>::emplace(Args&&... args)
{
    std::shared_ptr<T> sp = std::make_shared<T>(std::forward<Args>(args)...);
    std::shared_ptr<void> vp = std::static_pointer_cast<void>(sp);
    auto ti = std::type_index(typeid(T));
    if (!this->try_assign(vp, ti))
    {
        throw std::runtime_error("TaskOutput<T>::emplace() : failed to assign value.");
    }
    T* rp = sp.get();
    return *rp;
}

template <typename T>
T& TaskOutput<T>::operator*()
{
    return *this->operator->();
}

template <typename T>
T* TaskOutput<T>::operator->()
{
    std::shared_ptr<void> out_value;
    std::type_index out_type;
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
    return static_cast<T*>(out_value.get());
}

} // namespace tg::core
