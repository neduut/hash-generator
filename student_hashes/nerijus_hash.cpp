#include "nerijus_hash.h"
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>

namespace {
constexpr uint64_t MASK64 = ~0ull;

inline uint64_t rotl64(uint64_t x, int r) {
    r &= 63;
    return ((x << r) & MASK64) | (x >> (64 - r));
}
}

std::string generate_nerijus_hash(const std::string& text) {
    uint64_t a = 0x1A2B3C4D5E6F7788ull;
    uint64_t b = 0x8899AABBCCDDEEFFull;
    uint64_t c = 0x0123456789ABCDEFull;
    uint64_t d = 0xF0E1D2C3B4A59687ull;

    for (unsigned char ch : text) {
        a ^= ch;
        a = rotl64(a, 7);
        a = (a * 33ull + (ch ^ (ch >> 2))) & MASK64;

        b ^= rotl64(ch, 11);
        b = (b * 29ull + (ch ^ (ch >> 4))) & MASK64;

        c ^= rotl64(ch, 19);
        c = (c * 35ull + (ch ^ (ch >> 6))) & MASK64;

        d ^= rotl64(ch, 23);
        d = (d * 39ull + (ch ^ (ch >> 8))) & MASK64;
    }

    a ^= rotl64(b, 13);  a = (a + c) & MASK64;
    b ^= rotl64(c, 17);  b = (b + d) & MASK64;
    c ^= rotl64(d, 29);  c = (c + a) & MASK64;
    d ^= rotl64(a, 31);  d = (d + b) & MASK64;

    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::nouppercase
       << std::setw(16) << a
       << std::setw(16) << b
       << std::setw(16) << c
       << std::setw(16) << d;
    return ss.str();
}
