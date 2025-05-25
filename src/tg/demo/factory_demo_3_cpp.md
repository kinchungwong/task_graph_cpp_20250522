# factory_demo_3.cpp (converted into markdown)
```cpp

#include <string>
#include <utility>
#include <memory>
#include <vector>
#include <functional>
#include <stdexcept>
#include <optional>
#include <initializer_list>
#include <typeinfo>
#include <typeindex>

//dummy
namespace cv
{
    struct Mat{};
    void blur(const Mat&, Mat&);
};
class Data;
using DataPtr = std::shared_ptr<Data>;
class DataContext
{
public:
    DataContext() = default;
    virtual ~DataContext() = default;

    // if true, the context is being used when building the subgraph, which
    // needs to know what the inputs and outputs are.
    // if false, the task is executed as part of the subgraph.
    bool is_design_time() const;

    std::shared_ptr<void> get(DataPtr data) const;
    void set(DataPtr data, std::shared_ptr<void> value);

    std::type_index expected_type(DataPtr data) const;
    std::type_index actual_type(DataPtr data) const;
};

using DataContextPtr = std::shared_ptr<DataContext>;
using DataContextFunctor = std::function<std::optional<std::exception_ptr>(DataContextPtr)>;

template <typename T>
class InputContext
{
public:
    InputContext(DataContextPtr ctx, DataPtr data);
    std::shared_ptr<T> get() const;
    std::shared_ptr<T> consume(); // data is removed from the context
    const T& operator*() const;
    const T* operator->() const;
private:
    DataContextPtr m_ctx;
    DataPtr m_data;
};

template <typename T>
class OutputContext
{
public:
    OutputContext(DataContextPtr ctx, DataPtr data);
    void assign(std::shared_ptr<T> value); // stores the smart pointer as shared_ptr<void> in the context
    void emplace(const T& value); // copy-construct
    void emplace(T&& value); // move-construct
    void emplace(); // default constructed
private:
    DataContextPtr m_ctx;
    DataPtr m_data;
};

class TaskContext
{
public:
    TaskContext(DataContextPtr ctx, const std::string& name);    
};

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
    TaskPtr make_task(DataContextFunctor functor)
    {
        auto task = this->make_task();
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

    // ====== Old code from design iteration 2 ======
    // auto f_blur0 = [d_gray, d_gray_blur](DataContextPtr ctx) -> std::optional<std::exception_ptr>
    // {
    //     try
    //     {
    //         // Dummy implementation of the blur operation
    //         // In a real implementation, this would apply a blur to the input data
    //         auto img_gray = ctx->data_get(d_gray);
    //         cv::Mat img_gray_blur;
    //         cv::blur(img_gray, img_gray_blur, ...);
    //         ctx->data_movein(d_gray_blur, std::move(img_gray_blur));
    //         return std::nullopt; // No exception
    //     }
    //     catch (std::exception& ex)
    //     {
    //         return std::current_exception();
    //     }
    // };

    // ====== Code for design iteration 3 ======
    auto f_blur0 = [d_gray, d_gray_blur](DataContextPtr ctx) -> std::optional<std::exception_ptr>
    {
        try
        {
            TaskContext tc(ctx, "blur0");
            InputContext<cv::Mat> ic_gray(ctx, d_gray);
            OutputContext<cv::Mat> oc_gray_blur(ctx, d_gray_blur);
            if (!ctx->is_design_time())
            {
                // In a real implementation, this would apply a blur to the input data
                const cv::Mat& img_gray = *ic_gray;
                cv::Mat img_gray_blur;
                cv::blur(img_gray, img_gray_blur);
                oc_gray_blur.emplace(img_gray_blur);
            }
            return std::nullopt; // No exception
        }
        catch (std::exception& ex)
        {
            return std::current_exception();
        }
    };
    auto t_blur0 = sg_factory->make_task(f_blur0);
}

```
