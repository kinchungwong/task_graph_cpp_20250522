#include "tg/core/task.hpp"
#include "tg/core/task_dataset.hpp"

namespace tg::core
{

Task::Task()
    : m_dataset{std::make_shared<TaskDataSet>()}
{
}

Task::~Task()
{
}

TaskDataSetPtr Task::get_dataset() const
{
    return m_dataset;
}

} // namespace tg::core
