# Overview of current design

Last updated: 2025-05-29, 22:22pm

***This document must be kept up-to-date.***

## Terminology, concepts, and fundamental objects in the design

- Task
    - A task object encapsulates a block of executable code and related information.
    - Typical workload performed by an individual task is such as one OpenCV image operation.
    - It allows querying about the inputs and outputs that will be used during execution, ahead of the execution.
- Task graph
    - Manages a group of tasks for collaborative execution
    - These tasks can potentially be executed in parallel, subject to dependency-aware scheduling
    - These tasks act on a shared set of data, exchanged and mediated by a Context object
    - Task execution is mediated by a thread-safe and dependency-aware executor, backed by a thread pool
    - To ensure correct operation, a Task Graph performs many types of validations through all phases of use
    - Task Graph does not use the fork-join pattern due to its pitfalls.
- Subgraph
    - A subgraph is an incremental way of creating a Task Graph from tasks.
    - Tasks can be added to subgraph.
        - Within a subgraph, tasks can pass data using names (string labels) local to the subgraph.
        - In this sense, a subgraph is also a kind of namespace.
            - Any name that is local to the subgraph will have a fully-qualified ("namespaced") counterpart.
    - Subgraphs can be added to a Task Graph.
        - Data can be passed between subgraphs by adding connections using their fully-qualified names.
    - A subgraph also allow for multiple instancing.
        - When a subgraph is added to the Task Graph multiple times, each subgraph is deep-cloned, and their tasks will be able to execute independently.
- Data
    - Data is anything that can be produced and used by task.
    - The Task Graph system is responsible for protecting the data in multi-threaded execution.
        - Because the Task Graph system has several abstraction layers, some layers may not have explicit thread synchronization constructs (e.g. ```std::mutex```).
        - Incorrectly inserted synchronization constructs can sometimes introduce bugs into the system.
    - The Task Graph system must support arbitrary data types, subject to some limitations.
        - While OpenCV matrix (```cv::Mat```) is often used as an example, other types such as ```std::vector<std::vector<std::string>>``` must be allowed as well.
        - To ensure correct operation, tasks that produce or use a piece of data must specify the exact same type (in C++ sense).
    - Data is stored in a type-erased manner, as a tuple of (```std::shared_ptr<void>``` and ```std::type_index```).
        - The type-erased shared pointer allows type-agnostic lifetime management, thanks to destructor encapsulation.
        - The ```std::type_index``` is used for type enforcement at runtime.
    - Type checking of data using ```std::type_index``` occurs at various phases of use.
        - When a task is added to a subgraph, it conveys the type information of its inputs and outputs to the subgraph.
        - When data is passed (by name) between tasks in a subgraph, their type information is checked.
        - During task execution:
            - The task that produces an output uses a C++ class template (```TaskOutput<T>```) to perform the type-erasure safely, and also to validate the type.
            - The task that tkaes an input uses a C++ class template (```TaskInput<T>```) to validate the type before casting the type-erased pointer back to its original type.

## Intended use cases and key design wins

- Task Graph automatically accelerates image processing pipelines with a high degree of potential parallelizability.
    - "Pipeline" is used in an informal sense, because the actual algorithm might be more like a network than a sequential pipeline.
- For Task Graph, the ideal size of an individual task's workload is a basic OpenCV image operation on a 1-megapixel 8-bit or 24-bit image.
- Intermediate data produced by a task can be used by other tasks concurrently.
- Intermediate data that is not needed anymore after task execution are automatically released at the earliest possible moment.
- Task Graph is orthogonal to and composable with the Object Pool optimization technique.
    - Object Pool is slated for a future iteration of implementation.

## Typical programmer-user flow

- Implementation of components and parts
    - Implement some new tasks.
    - Implement some algorithms that use various tasks, old and new.
        - Algorithms are essentially subgraph factories that produce template-copies of connected tasks.
- Configuration
    - Tasks are added to a subgraph.
        - Some validations are performed.
    - Subgraphs are added to a Task Graph.
        - Data connections between subgraphs are also explicitly added to a Task Graph.
- Execution
    - Before execution:
        - Task Graph performs additional validation.
        - Execution-time data structures are created.
            - Data nodes that are equivalent are consolidated.
            - The dependency graph is simplified.
            - Additional flow control barriers may be inserted.
                - Flow control barriers are used to prevent a bloat of work-in-progress.
    - During execution:
        - Tasks that are ready to execute are sent to the Executor for execution.
            - Worker threads receive ready-to-execute tasks using a task queue.
        - When a task is finished:
            - The dependency graph is updated (see: Kahn's algorithm)
            - If it finds ready-to-execute tasks, these are sent to the Executor, thus keeping the Task Graph in motion.
    - After execution:
        - Cleanup
