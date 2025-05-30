#pragma once
#include "tg/core/fwd.hpp"

namespace tg::core
{

class GlobalDataSet
{
public:
    GlobalDataSet();
    ~GlobalDataSet();

private:
    GlobalDataSet(const GlobalDataSet&) = delete;
    GlobalDataSet(GlobalDataSet&&) = delete;
    GlobalDataSet& operator=(const GlobalDataSet&) = delete;
    GlobalDataSet& operator=(GlobalDataSet&&) = delete;
};

} // namespace tg::core
