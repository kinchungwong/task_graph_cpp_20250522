#include "tg/core/context.hpp"

namespace tg::core
{

Context::Context()
{
}

Context::~Context()
{
}

bool Context::can_write(const std::string& name, std::type_index type) const
{
    LockType lock(m_mutex);
    this->lck_check_name(name);
    return true;
}

std::shared_ptr<void> Context::get_data(const std::string& name, std::type_index type) const
{
    LockType lock(m_mutex);
    this->lck_check_name(name);
    const auto& pt = this->m_data.at(m_names.at(name));
    if (pt.second != type)
    {
        throw std::runtime_error("Context::get_data() : type mismatch for " + name + ", expected " + type.name() + ", got " + pt.second.name());
    }
    if (!pt.first)
    {
        throw std::runtime_error("Context::get_data() : data for " + name + " is null.");
    }
    return pt.first;
}

void Context::set_data(const std::string& name, std::shared_ptr<void> data, std::type_index type)
{
    LockType lock(m_mutex);
    this->lck_check_name(name);
    this->m_data.at(m_names.at(name)) = {std::move(data), type};
}

void Context::lck_check_name(const std::string& name) const
{
    // Private method, requires lock to be held by the caller.
    if (name.empty())
    {
        throw std::runtime_error("Context::check_name() : name cannot be empty.");
    }
    auto iter_name = m_names.find(name);
    if (iter_name == m_names.end())
    {
        throw std::runtime_error("Context::check_name() : name '" + name + "' is not registered.");
    }
    int index = iter_name->second;
    if (index < 0 || index >= static_cast<int>(m_data.size()))
    {
        throw std::runtime_error("Context::check_name() : index out of bounds for name '" + name + "'.");
    }
}

} // namespace tg::core
