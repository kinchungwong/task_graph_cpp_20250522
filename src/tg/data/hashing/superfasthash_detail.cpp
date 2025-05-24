#include <type_traits>
#include "common/project_macros.hpp"
#include "tg/data/hashing/superfasthash_detail.hpp"

namespace tg::data::hashing::superfasthash_detail
{

template <typename T>
std::enable_if_t<
    std::is_integral_v<T> && sizeof(T) == 1u, uint32_t>
zero_ext(T value)
{
    return static_cast<uint32_t>(static_cast<uint8_t>(value));
}

template <typename T>
std::enable_if_t<
    std::is_integral_v<T> && sizeof(T) == 1u, uint32_t>
sign_ext(T value)
{
    return static_cast<uint32_t>(static_cast<int32_t>(static_cast<int8_t>(value)));
}

uint32_t
superfasthash_init(uint32_t uint32_data_sz)
{
    return uint32_data_sz;
}

uint32_t
superfasthash_char_x4(uint32_t state, char c0, char c1, char c2, char c3)
{
    uint32_t v0 = zero_ext(c0);
    uint32_t v1 = zero_ext(c1);
    uint32_t v2 = zero_ext(c2);
    uint32_t v3 = zero_ext(c3);
    uint32_t v01_shift = v0 | (v1 << 8);
    uint32_t v23_shift = (v2 << 11) | (v3 << 19);
    state += v01_shift;
    state ^= (state >> 16) ^ v23_shift;
    state += (state >> 11);
    return state;
}

uint32_t
superfasthash_char_range(uint32_t state, const char* pchars, size_t sz)
{
    using namespace superfasthash_detail;
    if (!pchars || !sz)
    {
        return state;
    }
    size_t ofs = 0u;
    while (ofs + 4u <= sz)
    {
        state = superfasthash_char_x4(state, pchars[ofs], pchars[ofs + 1u],
            pchars[ofs + 2u], pchars[ofs + 3u]);
        ofs += 4u;
    }
    switch ((ofs + 4u - sz) & 3u)
    {
        case 0u:
            break;
        case 1u:
            state += sign_ext(pchars[ofs]);
            state ^= (state << 10);
            state += (state >> 1);
            break;
        case 2u:
            state += zero_ext(pchars[ofs]);
            state += (zero_ext(pchars[ofs + 1u]) << 8);
            state ^= (state << 11);
            state += (state >> 17);
            break;
        case 3u:
            state += zero_ext(pchars[ofs]);
            state += (zero_ext(pchars[ofs + 1u]) << 8);
            state ^= (state << 16);
            state ^= (sign_ext(pchars[ofs + 2u]) << 18);
            state += (state >> 11);
            break;
    }
    return state;
}

uint32_t
superfasthash_close(uint32_t state)
{
    state ^= (state << 3);
    state += (state >> 5);
    state ^= (state << 4);
    state += (state >> 17);
    state ^= (state << 25);
    state += (state >> 6);
    return state;
}

} // namespace tg::data::hashing::superfasthash_detail
