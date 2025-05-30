#include "tg/facade/facade_common.hpp"
#include "tg/facade/subgraph.hpp"

void facade_demo()
{
    using namespace tg::facade;

    DataName gray("gray"); // input
    DataName gray_blur("gray_blur");
    TaskInfo blur0("blur0");
    blur0 << gray >> gray_blur;

    TaskInfo dilate0("dilate0");
    DataName gray_blur_dilate("gray_blur_dilate");
    dilate0 << gray_blur >> gray_blur_dilate;

    TaskInfo erode0("erode0");
    DataName gray_blur_erode("gray_blur_erode");
    erode0 << gray_blur >> gray_blur_erode;

    TaskInfo subtract0("subtract0");
    DataName edge_morpho("edge_morpho");
    subtract0 << gray_blur_dilate << gray_blur_erode >> edge_morpho;

    TaskInfo morpho_to_weight("morpho_to_weight");
    DataName edge_weight("edge_weight");
    morpho_to_weight << edge_morpho >> edge_weight;

    TaskInfo mul0("mul0");
    DataName pyr_interp_nume("pyr_interp_nume");
    mul0 << gray << edge_weight >> pyr_interp_nume;

    TaskInfo cvt0("cvt0");
    DataName pyr_interp_denom("pyr_interp_denom");
    cvt0 << edge_weight >> pyr_interp_denom;

    TaskInfo pyr_interp_nume_worker("pyr_interp_nume_worker");
    DataName pyr_interp_nume_out("pyr_interp_nume_out");
    pyr_interp_nume_worker << pyr_interp_nume >> pyr_interp_nume_out;

    TaskInfo pyr_interp_denom_worker("pyr_interp_denom_worker");
    DataName pyr_interp_denom_out("pyr_interp_denom_out");
    pyr_interp_denom_worker << pyr_interp_denom >> pyr_interp_denom_out;

    TaskInfo div0("div0");
    DataName thresh_surface("thresh_surface");
    div0 << pyr_interp_nume_out << pyr_interp_denom_out >> thresh_surface;

    TaskInfo subtract1("subtract1");
    DataName thresh_unscaled("thresh_unscaled");
    subtract1 << gray << thresh_surface >> thresh_unscaled;

    TaskInfo scale0("scale0");
    DataName thresh_scaled("thresh_scaled");
    scale0 << thresh_unscaled >> thresh_scaled;

    Subgraph subgraph("subgraph", {
        blur0,
        dilate0,
        erode0,
        subtract0,
        morpho_to_weight,
        mul0,
        cvt0,
        pyr_interp_nume_worker,
        pyr_interp_denom_worker,
        div0,
        subtract1,
        scale0
    });
}
