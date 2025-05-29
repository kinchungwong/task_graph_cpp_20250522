#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

class Context
{
public:
    using MutexType = std::mutex;
    using LockType = std::unique_lock<MutexType>;

public:
    Context();
    ~Context();
    bool can_write(const std::string& name, std::type_index type) const;
    std::shared_ptr<void> get_data(const std::string& name, std::type_index type) const;
    void set_data(const std::string& name, std::shared_ptr<void> data, std::type_index type);

private:
    void lck_check_name(const std::string& name) const;

private:
    mutable MutexType m_mutex;
    std::unordered_map<std::string, int> m_names;
    std::vector<std::pair<std::shared_ptr<void>, std::type_index>> m_data;
};

} // namespace tg::core
