#pragma once
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <type_traits>

namespace data
{

// Forward declaration for internal use
template <typename T, typename Hash>
class UniqueDataSet;

namespace detail {
    // Helper to determine if T should be stored directly or via smart pointer
    template <typename T>
    struct storage_policy {
        // Use direct storage if T is small, copyable, and move-constructible
        static constexpr bool use_direct = 
            sizeof(T) <= 64 && 
            std::is_copy_constructible_v<T> && 
            std::is_move_constructible_v<T>;
        
        using type = std::conditional_t<use_direct, T, std::shared_ptr<T>>;
    };
    
    // Hash adapter for direct storage
    template <typename T, typename Hash>
    struct direct_hash {
        Hash hasher;
        
        size_t operator()(const T& value) const {
            return hasher(value);
        }
    };
    
    // Hash adapter for shared_ptr storage
    template <typename T, typename Hash>
    struct ptr_hash {
        Hash hasher;
        
        size_t operator()(const std::shared_ptr<T>& ptr) const {
            if (!ptr) return 0;
            return hasher(*ptr);
        }
    };
    
    // Equality adapter for direct storage
    template <typename T>
    struct direct_equal {
        bool operator()(const T& lhs, const T& rhs) const {
            return lhs == rhs;
        }
    };
    
    // Equality adapter for shared_ptr storage
    template <typename T>
    struct ptr_equal {
        bool operator()(const std::shared_ptr<T>& lhs, const std::shared_ptr<T>& rhs) const {
            if (!lhs || !rhs) return lhs == rhs;
            return *lhs == *rhs;
        }
    };
}

/**
 * A container that maintains a vector of unique elements with O(1) lookup.
 * 
 * Template parameters:
 * - T: The element type (may be non-copyable, non-hashable)
 * - Hash: Hash function object type for T (required)
 * 
 * The class automatically chooses between direct storage and shared_ptr storage
 * based on the characteristics of type T to minimize overhead.
 */
template <typename T, typename Hash>
class UniqueDataSet
{
private:
    using storage_t = typename detail::storage_policy<T>::type;
    static constexpr bool use_direct_storage = detail::storage_policy<T>::use_direct;
    
    using hash_t = std::conditional_t<
        use_direct_storage,
        detail::direct_hash<T, Hash>,
        detail::ptr_hash<T, Hash>
    >;
    
    using equal_t = std::conditional_t<
        use_direct_storage,
        detail::direct_equal<T>,
        detail::ptr_equal<T>
    >;
    
    std::vector<storage_t> m_data;
    std::unordered_map<storage_t, size_t, hash_t, equal_t> m_lookup;
    
public:
    using value_type = T;
    using size_type = size_t;
    
    // Constructor
    explicit UniqueDataSet(Hash hash_func = Hash{})
        : m_lookup(0, hash_t{hash_func}, equal_t{})
    {
    }
    
    // Add an element (perfect forwarding)
    template <typename U>
    std::pair<size_t, bool> add(U&& value) {
        if constexpr (use_direct_storage) {
            return add_direct(std::forward<U>(value));
        } else {
            return add_ptr(std::forward<U>(value));
        }
    }
    
    // Get element by index
    const T& operator[](size_t index) const {
        if constexpr (use_direct_storage) {
            return m_data[index];
        } else {
            return *m_data[index];
        }
    }
    
    // Find index of element (returns size() if not found)
    size_t find(const T& value) const {
        if constexpr (use_direct_storage) {
            auto it = m_lookup.find(value);
            return (it != m_lookup.end()) ? it->second : size();
        } else {
            // For ptr storage, we need to create a temporary shared_ptr for lookup
            // This is unavoidable overhead, but minimized by the storage policy
            auto temp_ptr = std::make_shared<T>(value);
            auto it = m_lookup.find(temp_ptr);
            return (it != m_lookup.end()) ? it->second : size();
        }
    }
    
    // Check if element exists
    bool contains(const T& value) const {
        return find(value) < size();
    }
    
    // Size operations
    size_t size() const { return m_data.size(); }
    bool empty() const { return m_data.empty(); }
    
    // Clear all elements
    void clear() {
        m_data.clear();
        m_lookup.clear();
    }
    
    // Reserve capacity
    void reserve(size_t capacity) {
        m_data.reserve(capacity);
        m_lookup.reserve(capacity);
    }
    
    // Iterator support
    auto begin() const {
        if constexpr (use_direct_storage) {
            return m_data.begin();
        } else {
            // For ptr storage, we'd need a custom iterator that dereferences
            // For simplicity, we'll provide index-based access
            return m_data.begin();
        }
    }
    
    auto end() const {
        return m_data.end();
    }

private:
    // Implementation for direct storage
    template <typename U>
    std::pair<size_t, bool> add_direct(U&& value) {
        auto it = m_lookup.find(value);
        if (it != m_lookup.end()) {
            return {it->second, false}; // Already exists
        }
        
        size_t index = m_data.size();
        m_data.emplace_back(std::forward<U>(value));
        m_lookup.emplace(m_data.back(), index);
        return {index, true}; // New element added
    }
    
    // Implementation for shared_ptr storage
    template <typename U>
    std::pair<size_t, bool> add_ptr(U&& value) {
        auto ptr = std::make_shared<T>(std::forward<U>(value));
        auto it = m_lookup.find(ptr);
        if (it != m_lookup.end()) {
            return {it->second, false}; // Already exists
        }
        
        size_t index = m_data.size();
        m_data.push_back(ptr);
        m_lookup.emplace(ptr, index);
        return {index, true}; // New element added
    }
};

} // namespace data

