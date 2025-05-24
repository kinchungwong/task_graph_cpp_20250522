#include <iostream>
#include <string>
#include <vector>
#include "data/unique_list.hpp"

// Example custom hash function for strings
struct StringHash {
    size_t operator()(const std::string& str) const {
        return std::hash<std::string>{}(str);
    }
};

// Example of a large, non-copyable type
struct LargeObject {
    std::vector<int> data;
    std::string name;
    
    // Make it non-copyable
    LargeObject(const LargeObject&) = delete;
    LargeObject& operator=(const LargeObject&) = delete;
    
    // But moveable
    LargeObject(LargeObject&&) = default;
    LargeObject& operator=(LargeObject&&) = default;
    
    LargeObject(std::string n, size_t size) : name(std::move(n)), data(size, 42) {}
    
    bool operator==(const LargeObject& other) const {
        return name == other.name && data == other.data;
    }
};

struct LargeObjectHash {
    size_t operator()(const LargeObject& obj) const {
        return std::hash<std::string>{}(obj.name) ^ obj.data.size();
    }
};

int main() {
    std::cout << "Testing UniqueDataSet with different types...\n\n";
    
    // Test 1: Small copyable type (uses direct storage)
    {
        std::cout << "Test 1: Small copyable type (int)\n";
        data::UniqueDataSet<int, std::hash<int>> int_set;
        
        auto [idx1, added1] = int_set.add(42);
        auto [idx2, added2] = int_set.add(24);
        auto [idx3, added3] = int_set.add(42); // Duplicate
        
        std::cout << "Added 42: index=" << idx1 << ", new=" << added1 << "\n";
        std::cout << "Added 24: index=" << idx2 << ", new=" << added2 << "\n";
        std::cout << "Added 42 again: index=" << idx3 << ", new=" << added3 << "\n";
        std::cout << "Size: " << int_set.size() << "\n";
        std::cout << "Contains 42: " << int_set.contains(42) << "\n\n";
    }
    
    // Test 2: String type with custom hash (uses direct storage)
    {
        std::cout << "Test 2: String type with custom hash\n";
        data::UniqueDataSet<std::string, StringHash> string_set;
        
        auto [idx1, added1] = string_set.add("hello");
        auto [idx2, added2] = string_set.add("world");
        auto [idx3, added3] = string_set.add("hello"); // Duplicate
        
        std::cout << "Added 'hello': index=" << idx1 << ", new=" << added1 << "\n";
        std::cout << "Added 'world': index=" << idx2 << ", new=" << added2 << "\n";
        std::cout << "Added 'hello' again: index=" << idx3 << ", new=" << added3 << "\n";
        std::cout << "Size: " << string_set.size() << "\n";
        std::cout << "Element at index 0: '" << string_set[0] << "'\n\n";
    }
    
    // Test 3: Large non-copyable type (uses shared_ptr storage)
    {
        std::cout << "Test 3: Large non-copyable type\n";
        data::UniqueDataSet<LargeObject, LargeObjectHash> large_set;
        
        auto [idx1, added1] = large_set.add(LargeObject("obj1", 100));
        auto [idx2, added2] = large_set.add(LargeObject("obj2", 200));
        
        std::cout << "Added large object 1: index=" << idx1 << ", new=" << added1 << "\n";
        std::cout << "Added large object 2: index=" << idx2 << ", new=" << added2 << "\n";
        std::cout << "Size: " << large_set.size() << "\n";
        std::cout << "First object name: '" << large_set[0].name << "'\n";
        std::cout << "First object data size: " << large_set[0].data.size() << "\n\n";
    }
    
    return 0;
}