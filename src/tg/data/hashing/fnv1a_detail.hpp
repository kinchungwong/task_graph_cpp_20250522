#pragma once
#include <cstddef>
#include <cstdint>

namespace tg::data::hashing::fnv1a_detail
{

uint64_t fnv1a_init();
uint64_t fnv1a_uint8(uint64_t state, uint8_t value);
uint64_t fnv1a_char(uint64_t state, char value);
uint64_t fnv1a_char_range(uint64_t state, const char* pchars, size_t sz);
uint64_t fnv1a_memory_range(uint64_t state, const void* psrc, size_t sz);

} // namespace tg::data::hashing::fnv1a_detail
