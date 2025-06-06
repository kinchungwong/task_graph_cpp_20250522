# Goal

Initial design for a class that manages global data, named "GlobalDataSet".

(There is already a GlobalDataSet class in the code base; the goal is to create a better design to replace that.)

- Instruction version ID: 2025-06-05, 19:58pm (current)

# Requirements

- It contains an unordered_map from a "key type" to a variant data type. 
- Currently the "key type" hasn't been chosen; design flexibility is needed.
- However, due to the size and complexity of the global data set class, it is impermissible to make the entire class a template. Instead, the responsibilities must be carefully broken down into a PImpl (Private Implementation pattern) layer, and a template layer.
- The variant data type is implemented using a pair of ```std::shared_ptr<void>``` and a ```std::type_index``` for runtime type-safe type-erasure variant implementation.
- The global data set class has two distinct phases in its lifecycle. These are the "design" phase and the "execution" phase.
- The "design phase" is single-threaded. 
- The "execution phase" is massively multi-threaded on multiple CPU cores.
- During the first phase, keys are populated. The values will be a pair consisting of empty ```std::shared_ptr<void>```, i.e. it points to ```std::nullptr_t``` (no ownership).
- When the class transitions into the second phase, the map is locked, so that no new keys can be added.
- Optionally, the actual data can be moved to a vector for faster linear access using their item index.
- The reason for having a frozen vector with non-frozen referent values is so that the first level item retrieval does not require locking a mutex. This is essential because during massively multi-threaded execution, the mutex on the vector can become a single point of bottleneck and severely degrade performance.
- Optionally, a conversion step may be implemented; the key type (which, according to the design flexibility required, may be a string type) can be converted into some assigned integer types for faster access during the execution phase.
