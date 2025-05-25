# factory_demo_2.cpp (converted into markdown)
```cpp

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>
#include <optional>
#include <initializer_list>

//dummy
class DataContext;
using DataContextPtr = std::shared_ptr<DataContext>;
using DataContextFunctor = std::function<std::optional<std::exception_ptr>(DataContextPtr)>;

class Nameable
{
protected:
    std::string m_name;
    const std::string& name() const
    {
        return m_name;
    }
    void name(const std::string& name)
    {
        m_name = name;
    }
    ~Nameable() = default;
};

class Data : private Nameable
{
public:
    using Nameable::name;
};
using DataPtr = std::shared_ptr<Data>;

class Task : private Nameable
{
public:
    Task();
    using Nameable::name;
    void set_inputs(std::initializer_list<DataPtr> inputs)
    {
        m_inputs = inputs;
    }
    void set_outputs(std::initializer_list<DataPtr> outputs)
    {
        m_outputs = outputs;
    }
    void set_functor(DataContextFunctor functor)
    {
        m_functor = std::move(functor);
    }
private:
    std::vector<DataPtr> m_inputs;
    std::vector<DataPtr> m_outputs;
    DataContextFunctor m_functor;
};
using TaskPtr = std::shared_ptr<Task>;

class SubgraphFactory
{
public:
    DataPtr make_data();
    DataPtr make_data(const std::string& name)
    {
        auto data = this->make_data();
        data->name(name);
        return data;
    }
    TaskPtr make_task();
    TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs)
    {
        auto task = this->make_task();
        task->set_inputs(inputs);
        task->set_outputs(outputs);
        return task;
    }

    TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs, const std::string& name)
    {
        auto task = this->make_task();
        task->set_inputs(inputs);
        task->set_outputs(outputs);
        task->name(name);
        return task;
    }

    TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs, const std::string& name, DataContextFunctor functor)
    {
        auto task = this->make_task();
        task->set_inputs(inputs);
        task->set_outputs(outputs);
        task->name(name);
        task->set_functor(std::move(functor));
        return task;
    }
};

void facade_demo()
{
    // ====== New code ======
    auto sg_factory = std::make_shared<SubgraphFactory>();
    
    // ====== Naming everything (not necessary) ======
    // auto d_gray = sg_factory->make_data("gray");
    // auto d_gray_blur = sg_factory->make_data("gray_blur");

    // ====== Don't give names, unless they are needed ======
    // Names are needed when interfacing with another subgraph or global inputs/outputs.
    // ======
    auto d_gray = sg_factory->make_data();
    auto d_gray_blur = sg_factory->make_data();

    auto f_blur0 = [d_gray, d_gray_blur](DataContextPtr ctx) -> std::optional<std::exception_ptr>
    {
        try
        {
            // Dummy implementation of the blur operation
            // In a real implementation, this would apply a blur to the input data
            auto img_gray = ctx->data_get(d_gray);
            cv::Mat img_gray_blur;
            cv::blur(img_gray, img_gray_blur, ...);
            ctx->data_movein(d_gray_blur, std::move(img_gray_blur));
            return std::nullopt; // No exception
        }
        catch (std::exception& ex)
        {
            return std::current_exception();
        }
    };
    auto t_blur0 = sg_factory->make_task({d_gray}, {d_gray_blur}, "blur0", f_blur0);
}

```
