#include <iostream>
#include "tg/core/test_case/test_case_main.hpp"
#include "tg/core/subgraph.hpp"
#include "tg/core/test_case/blur_task.hpp"
#include "tg/core/task_dataset.hpp"

void test_case_main()
{
    using namespace tg::core;
    using namespace tg::core::test_case;

    SubgraphPtr subgraph = std::make_shared<Subgraph>();

    // Create a blur task and add it to the subgraph
    auto blur_task = std::make_shared<BlurTask>("input_image", "output_image");
    subgraph->add_task(blur_task);

    // Simulate executor behavior of populating inputs before task execution
    auto dataset = blur_task->get_dataset();
    dataset->at(0)->try_assign(
        std::make_shared<fake_opencv::Mat>(fake_opencv::Size{640, 480}, 16), 
        typeid(fake_opencv::Mat)
    );

    blur_task->on_execute();


    std::shared_ptr<void> fake_output;
    std::type_index fake_output_type{typeid(void)};
    bool success = dataset->at(1)->try_get(fake_output, fake_output_type);
    if (!success)
    {
        std::cerr << "Failed to get output from the dataset." << std::endl;
        return;
    }
    std::cout << "Output type: " << fake_output_type.name() << std::endl;
    std::cout << "Output pointer: " << fake_output.get() << std::endl;
}
