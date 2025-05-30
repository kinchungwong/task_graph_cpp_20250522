#include "tg/core/task_dataset.hpp"
#include "tg/core/task_data.hpp"

namespace tg::core
{

TaskDataSet::TaskDataSet()
    : m_mutex{}
    , m_add_frozen{false}
    , m_check_duplicate{true}
    , m_data{}
    , m_names{}
{
}

TaskDataSet::~TaskDataSet()
{
}

void TaskDataSet::add(TaskDataPtr data)
{
    LockType lock(m_mutex);
    if (m_add_frozen)
    {
        throw std::logic_error("TaskDataSet::add(): cannot add TaskData after freeze_add() has been called.");
    }
    if (!data)
    {
        throw std::invalid_argument("TaskDataSet::add(): TaskData cannot be null.");
    }
    if (m_check_duplicate)
    {
        for (const auto& existing_data: m_data)
        {
            if (existing_data.get() == data.get())
            {
                throw std::invalid_argument("TaskDataSet::add(): same TaskData instance cannot be added twice.");
            }
        }
    }
    m_data.emplace_back(std::move(data));
}

void TaskDataSet::freeze_add()
{
    LockType lock(m_mutex);
    m_add_frozen = true;
}

TaskDataPtr TaskDataSet::at(int index) const
{
    LockType lock(m_mutex);
    auto sz = m_data.size();
    if (index < 0 || static_cast<size_t>(index) >= sz)
    {
        throw std::out_of_range("TaskDataSet::at(): bad index " + std::to_string(index));
    }
    return m_data.at(index);
}

void TaskDataSet::get_all(std::vector<TaskDataPtr>& out_items) const
{
    if (!out_items.empty())
    {
        throw std::invalid_argument("TaskDataSet::get_all(): out_items need to be empty.");
    }
    LockType lock(m_mutex);
    out_items.assign(m_data.begin(), m_data.end());
}

void TaskDataSet::release()
{
    LockType lock(m_mutex);
    for (auto& data : m_data)
    {
        data->release();
    }
}

} // namespace tg::core
