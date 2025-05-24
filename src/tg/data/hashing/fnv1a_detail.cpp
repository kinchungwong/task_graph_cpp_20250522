#include "common/project_macros.hpp"
#include "tg/data/hashing/fnv1a_detail.hpp"

namespace tg::data::hashing::fnv1a_detail
{

constexpr uint64_t FNV1A_INITIAL = UINT64_C(0xCBF29CE484222325);
constexpr uint64_t FNV1A_PRIME = UINT64_C(0x100000001B3);

uint64_t fnv1a_init()
{
    return FNV1A_INITIAL;
}

uint64_t fnv1a_uint8(uint64_t state, uint8_t value)
{
    state ^= static_cast<uint64_t>(value);
    state *= FNV1A_PRIME;
    return state;
}

uint64_t fnv1a_char(uint64_t state, char value)
{
    state ^= static_cast<uint64_t>(static_cast<uint8_t>(value));
    state *= FNV1A_PRIME;
    return state;
}

uint64_t fnv1a_char_range(uint64_t state, const char* pchars, size_t sz)
{
    for (size_t k = 0u; k < sz; ++k)
    {
        state = fnv1a_char(state, pchars[k]);
    }
    return state;
}

uint64_t fnv1a_memory_range(uint64_t state, const void* psrc, size_t sz)
{
    const char* pchars = reinterpret_cast<const char*>(psrc);
    for (size_t k = 0u; k < sz; ++k)
    {
        state = fnv1a_char(state, pchars[k]);
    }
    return state;
}

} // namespace tg::data::hashing::fnv1a_detail
