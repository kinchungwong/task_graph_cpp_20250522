# factory_demo_4.cpp (converted into markdown)
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



/**
 * @brief A magical object that provides access to data items using various naming schemes.
 * @details
 * When used at design time, it captures the names of inputs and outputs.
 * When used at execution time, it translates the subgraph-local names into global names,
 * and fetches or populates the corresponding data items in the global context.
 */
class DataContext
{
public:
    DataContext() = default;
    virtual ~DataContext() = default;

    bool is_exec_time() const; // only in factory_demo_4.cpp

    std::shared_ptr<void> get(DataPtr data, std::type_index value_type) const;
    void set(DataPtr data, std::type_index value_type, std::shared_ptr<void> value);

    void set_expected_type(DataPtr data, std::type_index);
    void set_validator(DataPtr data, std::function<bool(const std::shared_ptr<void>&)> validator);
    std::type_index expected_type(DataPtr data) const;
    std::type_index actual_type(DataPtr data) const;
};

using DataContextPtr = std::shared_ptr<DataContext>;
using DataContextFunctor = std::function<std::optional<std::exception_ptr>(DataContextPtr)>;

/**
 * @brief InputContext is a template accessor for named data item in the DataContext.
 * 
 * @remarks InputContext is not allowed to cache anything about the data; each dereference
 * must fetch the data from the DataContext, so that the fact of data being used is captured.
 * 
 * @remarks At design time, the constructor does not throw (it does not require the data
 * to exist). However, dereferencing at design time must throw.
 */
template <typename T>
class InputContext
{
public:
    InputContext(DataContextPtr ctx, const std::string& name); // only in factory_demo_4.cpp
    std::shared_ptr<T> get() const;
    std::shared_ptr<T> consume(); // data is removed from the context
    const T& operator*() const;
    const T* operator->() const;
private:
    DataContextPtr m_ctx;
    std::string m_name; // name of the data in the context, possibly a local name (not namespaced)
    //! @todo Should store a fast retrieval key or slot ID for the data from the given m_ctx
};

/**
 * @brief OutputContext is a template mutator for named data item in the DataContext.
 * 
 * @remarks OutputContext is not allowed to cache anything about the data; all operations
 * must write through to the DataContext.
 * 
 * @remarks At design time, the constructor does not throw. However, dereferencing at
 * design time must throw.
 */
template <typename T>
class OutputContext
{
public:
    OutputContext(DataContextPtr ctx, const std::string& name); // only in factory_demo_4.cpp
    void assign(std::shared_ptr<T> value); // stores the smart pointer as shared_ptr<void> in the context
    void emplace(const T& value); // copy-construct
    void emplace(T&& value); // move-construct
    void emplace(); // default constructed (e.g. empty cv::Mat)
    T& operator*() const; // valid after emplace; must throw otherwise
    T* operator->() const; // valid after emplace; must throw otherwise
private:
    DataContextPtr m_ctx;
    std::string m_name; // name of the data in the context, possibly a local name (not namespaced)
    //! @todo Should store a fast retrieval key or slot ID for the data from the given m_ctx
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
    // DataPtr make_data();
    // DataPtr make_data(const std::string& name)
    // {
    //     auto data = this->make_data();
    //     data->name(name);
    //     return data;
    // }

    // TaskPtr make_task();
    // TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs)
    // {
    //     auto task = this->make_task();
    //     task->set_inputs(inputs);
    //     task->set_outputs(outputs);
    //     return task;
    // }

    // TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs, const std::string& name)
    // {
    //     auto task = this->make_task();
    //     task->set_inputs(inputs);
    //     task->set_outputs(outputs);
    //     task->name(name);
    //     return task;
    // }

    // TaskPtr make_task(std::initializer_list<DataPtr> inputs, std::initializer_list<DataPtr> outputs, const std::string& name, DataContextFunctor functor)
    // {
    //     auto task = this->make_task();
    //     task->set_inputs(inputs);
    //     task->set_outputs(outputs);
    //     task->name(name);
    //     task->set_functor(std::move(functor));
    //     return task;
    // }

    DataPtr make_external_input(const std::string& name);

    DataPtr make_external_output(const std::string& name);

    TaskPtr make_task(DataContextFunctor functor)
    {
        auto task = this->make_task();
        task->set_functor(std::move(functor));
        return task;
    }
};

void facade_demo()
{
    // ====== Code for design iteration 4 ======

    auto sg_factory = std::make_shared<SubgraphFactory>();

    sg_factory->make_external_input("gray");

    struct io_blur0
    {
        InputContext<cv::Mat> imat_gray;
        OutputContext<cv::Mat> omat_blur;
        /**
         * @brief Defines the input and output for one step function.
         * @note The names are local to the subgraph to which the step function belongs.
         */
        explicit io_blur0(DataContextPtr ctx)
            : imat_gray(ctx, "gray")
            , omat_blur(ctx, "gray_blur")
        {
        }
    };

    auto design_blur0 = [](DataContextPtr ctx) -> std::shared_ptr<io_blur0>
    {
        // The data names used here are subgraph-local names.
        // However, at execution time, accessing the DataContext
        // using these subgraph-local names will instead fetch the
        // corresponding global data (which would have namespaced names)
        // to faciliate data exchange between subgraphs.

        // auto io = std::make_shared<io_blur0>();
        // io->imat_gray = InputContext<cv::Mat>(ctx, "gray");
        // io->omat_blur = OutputContext<cv::Mat>(ctx, "gray_blur");
        // return io;

        return std::make_shared<io_blur0>(ctx);
    };

    auto exec_blur0 = [](std::shared_ptr<io_blur0> io)
    {
        /**
         * @brief default constructed cv::Mat
         * @note Is it better to require or omit this line?
         *       What if we default-construct on first dereference?
         */
        io->omat_blur.emplace(); 

        cv::blur(*(io->imat_gray), *(io->omat_blur));
    };

    auto f_blur0 = [design_blur0, exec_blur0](DataContextPtr ctx) -> std::optional<std::exception_ptr>
    {
        try
        {
            auto io = design_blur0(ctx);
            if (ctx->is_exec_time())
            {
                exec_blur0(io);
            }
        }
        catch (std::exception& ex)
        {
            return std::current_exception();
        }
        return std::nullopt;
    };

    auto t_blur0 = sg_factory->make_task(f_blur0);

    // Optional: give name to task. Probably not necessary.
    t_blur0->name("t_blur");
}

```
