#pragma once
#include <cstdint>
#include <memory>
#include <optional>
#include <typeindex>
#include <stdexcept>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

namespace tg::core
{

class Task;
using TaskPtr = std::shared_ptr<Task>;

class Subgraph;
using SubgraphPtr = std::shared_ptr<Subgraph>;

class Context;
using ContextPtr = std::shared_ptr<Context>;

template <typename T> class TaskInput;
template <typename T> class TaskOutput;

class not_implemented : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

} // namespace tg::core
