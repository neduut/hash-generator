#include "migle_hash.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <array>
#include <utility>
#include <cstdint>

// bubble sort (rikiuoja pagal baito reikšmę) su hash skaičiavimu per swap'us (256 bit)
static std::array<uint32_t, 8> bubble_sort_and_hash(std::vector<char>& arr, std::array<uint32_t, 8> seed) {
    int n = (int)arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - 1 - i; ++j) {
            if (arr[j] > arr[j+1]) {
                unsigned int a = (unsigned char)arr[j];
                unsigned int b = (unsigned char)arr[j+1];
                int idx = j % 8;
                seed[idx] = (seed[idx] << 5) + (seed[idx] >> 3) + (a * 17 + b * 31 + j * 13);
                std::swap(arr[j], arr[j+1]);
            }
        }
    }
    return seed;
}

static std::string hash_to_hex(const std::array<uint32_t, 8>& h) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint32_t part : h) {
        ss << std::setw(8) << part;
    }
    return ss.str();
}

std::string migle_hash(const std::string& input) {
    std::vector<char> data(input.begin(), input.end());
    std::array<uint32_t, 8> seed = {0x12345678, 0x9abcdef0, 0x11111111, 0x22222222,
                                     0x33333333, 0x44444444, 0x55555555, 0x66666666};
    seed = bubble_sort_and_hash(data, seed);
    return hash_to_hex(seed);
}
