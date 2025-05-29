#pragma once
#include "tg/core/task.hpp"

namespace tg::core::test_case
{

class BlurTask final : public Task
{
public:
    BlurTask(const std::string& input, const std::string& output);
    ~BlurTask();
    void on_added(Subgraph& subgraph) final;
    void on_execute(Context& context) final;

private:
    std::string m_input;
    std::string m_output;
};

} // namespace tg::core::test_case
