#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

/**
 * @todo
 */
class TaskData;
using TaskDataPtr = std::shared_ptr<TaskData>;

class Event;
using EventPtr = std::shared_ptr<Event>;

class TaskGraph
{
public:
    TaskGraph();
    ~TaskGraph();

public:
private:
    std::vector<SubgraphPtr> m_subgraphs;
    std::vector<TaskPtr> m_tasks;
    std::vector<TaskDataPtr> m_data;
    std::vector<EventPtr> m_events;
};

} // namespace tg::core
