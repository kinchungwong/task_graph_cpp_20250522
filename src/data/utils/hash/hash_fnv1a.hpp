#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <utility>
#include <tuple>

namespace data::utils::hash
{
    /**
     * @warning Not checked for correctness.
     * @todo Need redesign for template consistency
     */
    struct Fnv1aEval
    {
        static constexpr uint64_t INITIAL = UINT64_C(0xcbf29ce484222325);
        static constexpr uint64_t PRIME = UINT64_C(0x100000001b3);
        uint64_t m_state;

        constexpr Fnv1aEval() noexcept
            : m_state{INITIAL}
        {}

        constexpr Fnv1aEval(uint64_t state) noexcept 
            : m_state{state}
        {}

        constexpr uint64_t transform(uint8_t value) noexcept
        {
            m_state ^= value;
            m_state *= PRIME;
            return m_state;
        }

        constexpr uint64_t transform(int8_t value) noexcept
        {
            m_state ^= static_cast<uint8_t>(value);
            m_state *= PRIME;
            return m_state;
        }

        constexpr uint64_t transform(uint16_t value) noexcept
        {
            uint8_t b0  = static_cast<uint8_t>(value);
            uint8_t b1  = static_cast<uint8_t>(value >> 8);
            m_state ^= b0;
            m_state *= PRIME;
            m_state ^= b1;
            m_state *= PRIME;
            return m_state;
        }

        constexpr uint64_t transform(int16_t value) noexcept
        {
            return transform(static_cast<uint16_t>(value));
        }

        constexpr uint64_t transform(uint32_t value) noexcept
        {
            uint8_t b0  = static_cast<uint8_t>(value);
            uint8_t b1  = static_cast<uint8_t>(value >> 8);
            uint8_t b2  = static_cast<uint8_t>(value >> 16);
            uint8_t b3  = static_cast<uint8_t>(value >> 24);
            m_state ^= b0;
            m_state *= PRIME;
            m_state ^= b1;
            m_state *= PRIME;
            m_state ^= b2;
            m_state *= PRIME;
            m_state ^= b3;
            m_state *= PRIME;
            return m_state;
        }

        constexpr uint64_t transform(int32_t value) noexcept
        {
            return transform(static_cast<uint32_t>(value));
        }

        constexpr uint64_t transform(uint64_t value) noexcept
        {
            uint8_t b0  = static_cast<uint8_t>(value);
            uint8_t b1  = static_cast<uint8_t>(value >> 8);
            uint8_t b2  = static_cast<uint8_t>(value >> 16);
            uint8_t b3  = static_cast<uint8_t>(value >> 24);
            uint8_t b4  = static_cast<uint8_t>(value >> 32);
            uint8_t b5  = static_cast<uint8_t>(value >> 40);
            uint8_t b6  = static_cast<uint8_t>(value >> 48);
            uint8_t b7  = static_cast<uint8_t>(value >> 56);
            m_state ^= b0;
            m_state *= PRIME;
            m_state ^= b1;
            m_state *= PRIME;
            m_state ^= b2;
            m_state *= PRIME;
            m_state ^= b3;
            m_state *= PRIME;
            m_state ^= b4;
            m_state *= PRIME;
            m_state ^= b5;
            m_state *= PRIME;
            m_state ^= b6;
            m_state *= PRIME;
            m_state ^= b7;
            m_state *= PRIME;
            return m_state;
        }

        constexpr uint64_t transform(int64_t value) noexcept
        {
            return transform(static_cast<uint64_t>(value));
        }

        constexpr uint64_t transform(bool value) noexcept
        {
            constexpr uint8_t b0 = UINT8_C(0x0);
            constexpr uint8_t b1 = UINT8_C(0x1);
            return transform(static_cast<uint8_t>(value ? b1 : b0));
        }

        template <typename T>
        uint64_t transform_obj(T value)
        {
            uint8_t bytes[sizeof(T)];
            std::memcpy(bytes, &value, sizeof(T));
            for (uint8_t bv : bytes)
            {
                m_state ^= bv;
                m_state *= PRIME;
            }
            return m_state;
        }

        template <typename... Ts, size_t idx>
        uint64_t detail_transform_tuple(const std::tuple<Ts...>& tup)
        {
            if constexpr (idx < sizeof...(Ts))
            {
                auto value = std::get<idx>(tup);
                transform(value);
                detail_transform_tuple<Ts..., idx + 1u>(tup);
            }
            return m_state;
        }

        template <typename... Ts>
        uint64_t transform(const std::tuple<Ts...>& tup)
        {
            return detail_transform_tuple<Ts..., 0u>(tup);
        }

        template <typename T1, typename T2>
        uint64_t transform(const std::pair<T1, T2>& pair)
        {
            transform(pair.first);
            transform(pair.second);
            return m_state;
        }
    };

} // namespace utils::hashing::fnv1a
