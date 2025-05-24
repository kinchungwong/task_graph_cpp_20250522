#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "common/project_macros.hpp"

namespace tg::data::hashing::bitwise_detail
{

constexpr uint32_t INLINE_ALWAYS
rotate_32(uint32_t value, int32_t shift)
{
    uint32_t ushift = static_cast<uint32_t>(shift) & 31u;
    return ushift ? ((value << ushift) | (value >> (32u - ushift))) : value;
}


constexpr uint64_t INLINE_ALWAYS
rotate_64(uint64_t value, int32_t shift)
{
    uint32_t ushift = static_cast<uint32_t>(shift) & 63u;
    return ushift ? ((value << ushift) | (value >> (64u - ushift))) : value;
}

} // namespace tg::data::hashing::bitwise_detail
