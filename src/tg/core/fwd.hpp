#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <typeinfo>
#include <typeindex>
#include <stdexcept>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include "tg/core/task_data_flags.hpp"

namespace tg::core
{

class Task;
using TaskPtr = std::shared_ptr<Task>;

class TaskData;
using TaskDataPtr = std::shared_ptr<TaskData>;

class TaskDataSet;
using TaskDataSetPtr = std::shared_ptr<TaskDataSet>;

class Subgraph;
using SubgraphPtr = std::shared_ptr<Subgraph>;

template <typename T> class TaskInput;
template <typename T> class TaskOutput;

class not_implemented : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace tg::core
