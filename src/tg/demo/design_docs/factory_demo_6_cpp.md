# factory_demo_6.cpp (converted into markdown)
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
#include <type_traits>

//dummy
namespace cv
{
    struct Mat{};
    void blur(const Mat&, Mat&);
};

using PtrTypePair = std::pair<std::shared_ptr<void>, std::type_index>;

class ContextualTask;
using ContextualTaskPtr = std::shared_ptr<ContextualTask>;
class TaskContext;
using TaskContextPtr = std::shared_ptr<TaskContext>;

template <typename T> class TaskInput
{
public:
    static_assert(!std::is_reference_v<T>, "T in TaskInput<T> cannot be a reference type");
    static_assert(!std::is_void_v<T>, "T in TaskInput<T> cannot be void");
    static_assert(!std::is_const_v<T>, "T in TaskInput<T> cannot be const-qualified");
    static_assert(!std::is_volatile_v<T>, "T in TaskInput<T> cannot be volatile-qualified");

public:
    TaskInput(TaskContextPtr ctx, const std::string& name);
    const T& operator*() const;
private:
    TaskInput(const TaskInput&) = delete;
    TaskInput(TaskInput&&) = delete;
    TaskInput& operator=(const TaskInput&) = delete;
    TaskInput& operator=(TaskInput&&) = delete;
private:
    TaskContextPtr m_ctx;
    std::string m_name;
    int m_slot_id;
    PtrTypePair m_pt;
};

template <typename T> class TaskOutput
{
public:
    static_assert(!std::is_reference_v<T>, "T in TaskOutput<T> cannot be a reference type");
    static_assert(!std::is_void_v<T>, "T in TaskOutput<T> cannot be void");
    static_assert(!std::is_const_v<T>, "T in TaskOutput<T> cannot be const-qualified");
    static_assert(!std::is_volatile_v<T>, "T in TaskOutput<T> cannot be volatile-qualified");

public:
    TaskOutput(TaskContextPtr ctx, const std::string& name)
        : m_ctx(std::move(ctx))
        , m_name(name)
        , m_pt(std::make_pair(nullptr, std::type_index(typeid(T))))
    {
        if (!m_ctx)
        {
            throw std::invalid_argument("TaskOutput<T>::TaskOutput(): ctx cannot be nullptr");
        }
        m_slot_id = m_ctx->writer_enter(m_name);
    }
    template <typename... Args>
    T& emplace(Args&&... args)
    {
        if (m_pt.first)
        {
            throw std::logic_error("TaskOutput<T>::emplace(): value already constructed");
        }
        m_pt.first = std::make_shared<T>(std::forward<Args>(args)...);
        return this->operator*();
    }
    T& operator*()
    {
        if (!m_pt.first)
        {
            throw std::logic_error("TaskOutput<T>::operator*(): value not constructed");
        }
        return *std::static_pointer_cast<T>(m_pt.first);
    }
    ~TaskOutput()
    {
        m_ctx->writer_leave(m_slot_id, std::move(m_pt.first), m_pt.second);
    }
private:
    TaskOutput(const TaskOutput&) = delete;
    TaskOutput(TaskOutput&&) = delete;
    TaskOutput& operator=(const TaskOutput&) = delete;
    TaskOutput& operator=(TaskOutput&&) = delete;
private:
    TaskContextPtr m_ctx;
    std::string m_name;
    int m_slot_id;
    PtrTypePair m_pt;
};

/**
 * @brief Context-aware task interface.
 * 
 * @details
 * This task interface is designed to be used within a task graph framework,
 * and will be used in two different lifecycles:
 *
 * 1. During the design phase, where tasks are created and configured.
 *     ctor() --> clone() --> attach(ctx_design) -- dtor()
 *
 * 2. During the execution phase, where tasks are executed with a specific context.
 *     (from clone()) --> attach(ctx_execute) --> execute() -- dtor()
 */
class ContextualTask
{
protected:
    ContextualTask();

public:
    virtual ~ContextualTask();
    virtual ContextualTaskPtr clone() = 0;
    virtual void attach(TaskContextPtr ctx) = 0;
    virtual void execute() = 0;

private:
    ContextualTask(const ContextualTask&) = delete;
    ContextualTask(ContextualTask&&) = delete;
    ContextualTask& operator=(const ContextualTask&) = delete;
    ContextualTask& operator=(ContextualTask&&) = delete;
};

class BlurTask final : public ContextualTask
{
public:
    explicit BlurTask(const std::string& input_name = "input", const std::string& output_name = "output")
        : ContextualTask()
        , m_input_name(input_name)
        , m_output_name(output_name)
        , m_input(std::nullopt)
        , m_output(std::nullopt)
    {}

    ContextualTaskPtr clone() final
    {
        return std::make_shared<BlurTask>(m_input_name, m_output_name);
    }

    void attach(TaskContextPtr ctx) final
    {
        if (!ctx)
        {
            throw std::invalid_argument("BlurTask::attach(): ctx cannot be nullptr");
        }
        m_input.emplace(ctx, m_input_name);
        m_output.emplace(ctx, m_output_name);
    }

    void execute() final
    {
#if 1
        m_output->emplace();
        cv::blur(**m_input, **m_output);
#else
        /**
         * Equivalent but more verbose code, showing the multiple dereferences.
         */
        const cv::Mat& input = m_input.value().operator*();
        cv::Mat& output = m_output->emplace();
        cv::blur(input, output);
#endif
    }

private:
    // The std::optional<> is merely used to allow late binding of TaskInput and TaskOutput.
    std::string m_input_name;
    std::string m_output_name;
    std::optional<TaskInput<cv::Mat>> m_input;
    std::optional<TaskOutput<cv::Mat>> m_output;
};

class SubgraphFactory
{
public:

    void mark_external_input(const std::string& name);
    void mark_external_output(const std::string& name);
    void add_task(ContextualTaskPtr task);
};

void facade_demo()
{
    // ====== Code for design iteration 6 ======

    auto sg_factory = std::make_shared<SubgraphFactory>();
    sg_factory->mark_external_input("gray");
    sg_factory->add_task(std::make_shared<BlurTask>("gray", "gray_blur"));
}

```
