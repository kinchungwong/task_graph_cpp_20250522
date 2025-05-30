#include "tg/core/task_data.hpp"

namespace tg::core
{

#if 0 // NOT IMPLEMENTED
// TaskData::TaskData(
//     const std::string& name,
//     TaskDataFlags flags,
//     std::optional<std::type_index> expected_type,
//     ValidatorPtr validator
// )
//     : m_mutex{}
//     , m_name{name}
//     , m_flags{flags}
//     , m_expected{expected_type}
//     , m_validator{std::move(validator)}
//     , m_actual{std::type_index(typeid(void))}
//     , m_value{}
// {
// }
#endif // NOT IMPLEMENTED

TaskData::TaskData(const std::string& name)
    : m_mutex{}
    , m_name{name}
    , m_expected{std::nullopt}
    , m_actual{std::type_index(typeid(void))}
    , m_value{}
{
}

TaskData::~TaskData()
{
}

bool TaskData::try_assign(std::shared_ptr<void> value, std::type_index actual_type)
{
    LockType lock(m_mutex);
    if (m_value)
    {
        return false;
    }
    if (!value)
    {
        throw std::invalid_argument("TaskData::assign(): value cannot be null.");
    }
    if (actual_type == std::type_index(typeid(void)))
    {
        throw std::invalid_argument("TaskData::assign(): actual_type cannot be void.");
    }
    if (m_expected)
    {
        const auto& expected = m_expected.value();
        if (expected != actual_type)
        {
            std::string s_expected{expected.name()};
            std::string s_actual{actual_type.name()};
            throw std::invalid_argument("TaskData::assign(): type mismatch. Expected: " + s_expected + ", got: " + s_actual);
        }
    }
    m_value = std::move(value);
    m_actual = actual_type;
    return true;
}

bool TaskData::try_get(std::shared_ptr<void>& out_value, std::type_index& out_type) const
{
    LockType lock(m_mutex);
    if (!m_value)
    {
        return false;
    }
    out_value = m_value;
    out_type = m_actual;
    return true;
}

bool TaskData::consume(std::shared_ptr<void>& out_value, std::type_index& out_type)
{
    LockType lock(m_mutex);
    if (!m_value)
    {
        return false;
    }
    out_value = std::move(m_value);
    out_type = m_actual;
    out_value.reset();
    out_type = std::type_index(typeid(void));
    return true;
}

} // namespace tg::core
