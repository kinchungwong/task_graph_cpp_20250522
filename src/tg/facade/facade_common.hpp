#pragma once
#include <string>
#include <vector>
#include <unordered_set>

namespace tg::facade
{

class DataName
{
public:
    const std::string name;
    explicit DataName(const std::string& name) : name{name} {}
    explicit DataName(const char* name) : name{name} {}
    DataName(const DataName&) = default;
    DataName(DataName&&) = default;
    DataName& operator=(const DataName&) = delete;
    DataName& operator=(DataName&&) = delete;
};

struct DataNameHash
{
    std::size_t operator()(const DataName& dataName) const
    {
        return std::hash<std::string>()(dataName.name);
    }
};

struct DataNameEqual
{
    bool operator()(const DataName& lhs, const DataName& rhs) const
    {
        return lhs.name == rhs.name;
    }
};

class TaskInfo
{
public:
    using DataNameSet = std::unordered_set<DataName, DataNameHash, DataNameEqual>;
    const std::string name;
    DataNameSet inputs;
    DataNameSet outputs;

    explicit TaskInfo(const std::string& name) : name{name}, inputs{}, outputs{} {}
    explicit TaskInfo(const char* name) : name{name}, inputs{}, outputs{} {}
    TaskInfo(const TaskInfo&) = default;
    TaskInfo(TaskInfo&&) = default;
    TaskInfo& operator=(const TaskInfo&) = delete;
    TaskInfo& operator=(TaskInfo&&) = delete;

    TaskInfo& operator<<(const DataName& input)
    {
        inputs.insert(input);
        return *this;
    }

    TaskInfo& operator>>(const DataName& output)
    {
        outputs.insert(output);
        return *this;
    }
};

} // namespace tg::facade
