#include "tg/core/test_case/blur_task.hpp"
#include "tg/core/subgraph.hpp"
#include "tg/core/test_case/fake_opencv.hpp"
#include "tg/core/task_dataset.hpp"
#include "tg/core/task_input.hpp"
#include "tg/core/task_output.hpp"

namespace tg::core::test_case
{

BlurTask::BlurTask(const std::string& input, const std::string& output)
    : Task{}
    , m_input{std::make_shared<TaskInput<fake_opencv::Mat>>(input)}
    , m_output{std::make_shared<TaskOutput<fake_opencv::Mat>>(output)}
{
    auto dataset = this->get_dataset();
    dataset->add(m_input);
    dataset->add(m_output);
    dataset->freeze_add();
}

BlurTask::~BlurTask()
{
}

void BlurTask::on_execute()
{
    const fake_opencv::Mat& input = **m_input;
    fake_opencv::Size sz = input.size();
    int cv_type = input.type();
    fake_opencv::Mat& output = m_output->emplace(sz, cv_type);
    fake_opencv::GaussianBlur(input, output);
}

} // namespace tg::core::test_case
