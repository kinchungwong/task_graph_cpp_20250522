#include "tg/core/test_case/blur_task.hpp"
#include "tg/core/subgraph.hpp"
#include "tg/core/test_case/fake_opencv.hpp"
#include "tg/core/task_input.hpp"
#include "tg/core/task_output.hpp"

namespace tg::core::test_case
{

BlurTask::BlurTask(const std::string& input, const std::string& output)
    : m_input(input)
    , m_output(output)
{
}

BlurTask::~BlurTask()
{
}

void BlurTask::on_added(Subgraph& subgraph)
{
    subgraph.add_input(m_input);
    subgraph.add_output(m_output);
}

void BlurTask::on_execute(Context& context)
{
    TaskInput<fake_opencv::Mat> input(context, m_input);
    TaskOutput<fake_opencv::Mat> output(context, m_output);
    fake_opencv::Size sz = input->size();
    int cv_type = input->type();
    output.emplace(sz, cv_type);
    fake_opencv::GaussianBlur(*input, *output);
}

} // namespace tg::core::test_case
