#pragma once
#include "tg/core/task.hpp"
#include "tg/core/task_input.fwd.hpp"
#include "tg/core/task_output.fwd.hpp"
#include "tg/core/test_case/fake_opencv.hpp"

namespace tg::core::test_case
{

class BlurTask final : public Task
{
public:
    BlurTask(const std::string& input, const std::string& output);
    ~BlurTask();
    void on_execute() final;

private:
    std::shared_ptr<TaskInput<fake_opencv::Mat>> m_input;
    std::shared_ptr<TaskOutput<fake_opencv::Mat>> m_output;
};

} // namespace tg::core::test_case
