#include "vanesa_hash.h"
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
                // kai sukeičiam elementus, atnaujinam hash
                unsigned int a = (unsigned char)arr[j];
                unsigned int b = (unsigned char)arr[j+1];

                int idx = j % 8; // pasirenkam, kurį 32-bit bloką keisti
                seed[idx] = (seed[idx] << 5) + (seed[idx] >> 3) + (a * 17 + b * 31 + j * 13);

                std::swap(arr[j], arr[j+1]);
            }
        }
    }
    return seed;
}

// salt (16 baitų)
static std::vector<uint8_t> make_salt(const std::string& msg) { 
    std::vector<uint8_t> salt(16, 0); // sukuriam tuščią salt
    for (size_t i = 0; i < msg.size(); i++) {
        // kiekviena raidė įmaišom į vieną iš 16 baitų (ASCII + pozicija*13), 0xFF kad neviršytų 255
        salt[i % 16] = (salt[i % 16] + (uint8_t)msg[i] + (i * 13)) & 0xFF;
    }
    return salt;
}

// hash pavertimas į hex
static std::string hash_to_hex(const std::array<uint32_t, 8>& h) {
    std::ostringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint32_t part : h) {
        ss << std::setw(8) << part;
    }
    return ss.str();
}

std::string generate_vanesa_hash(const std::string& msg) {
    // salt
    auto salt = make_salt(msg);

    // data = salt + msg
    std::vector<char> data;
    data.reserve(salt.size() + msg.size());
    for (uint8_t b : salt) data.push_back((char)b);
    data.insert(data.end(), msg.begin(), msg.end());
    
    // seed: 8 reikšmės po 32 bitus
    std::array<uint32_t, 8> seed = {
        (uint32_t)msg.length() * 123,            // ilgio seed
        (uint32_t)(unsigned char)msg.front() * 4567, // pirmo simbolio ASCII * konstanta
        (uint32_t)(unsigned char)msg.back() * 8910,  // paskutinio simbolio ASCII * konstanta
        (uint32_t)(msg.length() << 16) ^ 0xDEAD,     // ilgis pastumtas
        0xAAAAAAAAu ^ (uint32_t)msg.length(),        // XOR su ilgiu
        0x55555555u + (uint32_t)msg.length(),        // šita konstanta
        0xF0F0F0F0u ^ (unsigned char)msg[0],         // pirmo simbolio įtaka
        0x0F0F0F0Fu ^ (unsigned char)msg.back()      // paskutinio simbolio įtaka
    };

    // paleidžiam bubble sort su hash skaičiavimu
    auto h = bubble_sort_and_hash(data, seed);
    
    return hash_to_hex(h);
}
