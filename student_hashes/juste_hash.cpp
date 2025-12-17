#include "juste_hash.h"
#include <iomanip>
#include <sstream>
#include <cstdint>

std::string justeHash(const std::string& input) {
    uint64_t part0 = 0x1234567890abcdefULL;
    uint64_t part1 = 0xfedcba0987654321ULL;
    uint64_t part2 = 0x0f1e2d3c4b5a6978ULL;
    uint64_t part3 = 0x89abcdef01234567ULL;

    for (unsigned char b : input) {
        part0 = part0 + b;
        part1 = part1 + part0 * 3;
        part2 = part2 + part1 + (b * 7);
        part3 = part3 + part2 + (part0 * 1);
    }

    std::ostringstream oss;
    oss << std::hex << std::setw(16) << std::setfill('0') << part0 
        << std::setw(16) << std::setfill('0') << part1 
        << std::setw(16) << std::setfill('0') << part2 
        << std::setw(16) << std::setfill('0') << part3;
    
    return oss.str();
}
