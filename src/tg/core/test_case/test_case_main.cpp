#include "tg/core/test_case/test_case_main.hpp"
#include "tg/core/subgraph.hpp"
#include "tg/core/test_case/blur_task.hpp"
#include "tg/core/context.hpp"

void test_case_main()
{
    using namespace tg::core;
    using namespace tg::core::test_case;

    SubgraphPtr subgraph = std::make_shared<Subgraph>();
    ContextPtr context = std::make_shared<Context>(/*subgraph*/);

    // Create a blur task and add it to the subgraph
    auto blur_task = std::make_shared<BlurTask>("input_image", "output_image");
    subgraph->add_task(blur_task);

    blur_task->on_execute(*context);
}
