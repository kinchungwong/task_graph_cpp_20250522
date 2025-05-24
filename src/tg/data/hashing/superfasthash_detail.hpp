#pragma once
#include <cstddef>
#include <cstdint>

namespace tg::data::hashing::superfasthash_detail
{

uint32_t superfasthash_init(uint32_t uint32_data_sz);
uint32_t superfasthash_char_x4(uint32_t state, char c0, char c1, char c2, char c3);
uint32_t superfasthash_char_range(uint32_t state, const char* pchars, size_t sz);
uint32_t superfasthash_close(uint32_t state);

} // namespace tg::data::hashing::superfasthash_detail
