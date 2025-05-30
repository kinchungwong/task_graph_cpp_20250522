# factory_demo_1.cpp (converted into markdown)

```cpp
#include <memory>
#include "tg/facade/facade_common.hpp"
#include "tg/facade/subgraph.hpp"
#include "tg/demo/factory_demo.hpp"

using namespace tg::facade;

// dummy
class SubgraphFactory
{
public:
    std::shared_ptr<DataName> make_data(const std::string& name);
    std::shared_ptr<TaskInfo> make_task(const std::string& name);
    std::shared_ptr<Subgraph> export_subgraph(const std::string& name);
};

void facade_demo()
{
    // ====== Old code ======
    // DataName gray("gray"); // input
    // DataName gray_blur("gray_blur");
    // TaskInfo blur0("blur0");
    // blur0 << gray >> gray_blur;

    // ====== New code ======
    auto sg_factory = std::make_shared<SubgraphFactory>();
    auto d_gray = sg_factory->make_data("d_gray");
    auto d_gray_blur = sg_factory->make_data("d_gray_blur");
    auto t_blur0 = sg_factory->make_task("t_blur0");
    t_blur0->set_inputs({d_gray});
    t_blur0->set_outputs({d_gray_blur});

    // ===== Old code ======
    // TaskInfo dilate0("dilate0");
    // DataName gray_blur_dilate("gray_blur_dilate");
    // dilate0 << gray_blur >> gray_blur_dilate;

    // ====== New code ======
    auto d_gray_blur_dilate = sg_factory->make_data("d_gray_blur_dilate");
    auto t_dilate0 = sg_factory->make_task("t_dilate0");
    t_dilate0->set_inputs({d_gray_blur});
    t_dilate0->set_outputs({d_gray_blur_dilate});

    // ===== Old code ======
    // TaskInfo erode0("erode0");
    // DataName gray_blur_erode("gray_blur_erode");
    // erode0 << gray_blur >> gray_blur_erode;

    // ===== Old code ======
    // TaskInfo subtract0("subtract0");
    // DataName edge_morpho("edge_morpho");
    // subtract0 << gray_blur_dilate << gray_blur_erode >> edge_morpho;

    // ===== Old code ======
    // TaskInfo morpho_to_weight("morpho_to_weight");
    // DataName edge_weight("edge_weight");
    // morpho_to_weight << edge_morpho >> edge_weight;

    // ===== Old code ======
    // TaskInfo mul0("mul0");
    // DataName pyr_interp_nume("pyr_interp_nume");
    // mul0 << gray << edge_weight >> pyr_interp_nume;

    // ===== Old code ======
    // TaskInfo cvt0("cvt0");
    // DataName pyr_interp_denom("pyr_interp_denom");
    // cvt0 << edge_weight >> pyr_interp_denom;

    // ===== Old code ======
    // TaskInfo pyr_interp_nume_worker("pyr_interp_nume_worker");
    // DataName pyr_interp_nume_out("pyr_interp_nume_out");
    // pyr_interp_nume_worker << pyr_interp_nume >> pyr_interp_nume_out;

    // ===== Old code ======
    // TaskInfo pyr_interp_denom_worker("pyr_interp_denom_worker");
    // DataName pyr_interp_denom_out("pyr_interp_denom_out");
    // pyr_interp_denom_worker << pyr_interp_denom >> pyr_interp_denom_out;

    // ===== Old code ======
    // TaskInfo div0("div0");
    // DataName thresh_surface("thresh_surface");
    // div0 << pyr_interp_nume_out << pyr_interp_denom_out >> thresh_surface;

    // ===== Old code ======
    // TaskInfo subtract1("subtract1");
    // DataName thresh_unscaled("thresh_unscaled");
    // subtract1 << gray << thresh_surface >> thresh_unscaled;

    // ===== Old code ======
    // TaskInfo scale0("scale0");
    // DataName thresh_scaled("thresh_scaled");
    // scale0 << thresh_unscaled >> thresh_scaled;

    // ===== Old code ======
    // Subgraph subgraph("subgraph", {
    //     blur0,
    //     dilate0,
    //     erode0,
    //     subtract0,
    //     morpho_to_weight,
    //     mul0,
    //     cvt0,
    //     pyr_interp_nume_worker,
    //     pyr_interp_denom_worker,
    //     div0,
    //     subtract1,
    //     scale0
    // });

    // ====== New code ======
    // Because the factory tracks everything it creates, it already
    // knows everything about the subgraph.
    // ======
    auto subgraph = sg_factory->export_subgraph("subgraph");
}
```
