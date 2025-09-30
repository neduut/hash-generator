#include "md5.h"
#include "tests.h"
#include "hash.h"
#include "mylib.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <chrono>
#include <numeric>
#include <cstring>
#include <vector>

using namespace std;

// MD5 konstantos
const uint32_t MD5_K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// MD5 shift amounts
const int MD5_S[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21
};

// MD5 pomocne funkcijos
inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) | (~x & z);
}

inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
    return (x & z) | (y & ~z);
}

inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
    return x ^ y ^ z;
}

inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
    return y ^ (x | ~z);
}

inline uint32_t leftrotate(uint32_t value, int shift) {
    return (value << shift) | (value >> (32 - shift));
}

string md5_hash(const string& input) {
    // MD5 pradžios reikšmės
    uint32_t h0 = 0x67452301;
    uint32_t h1 = 0xefcdab89;
    uint32_t h2 = 0x98badcfe;
    uint32_t h3 = 0x10325476;
    
    // Paruošti žinutę
    string msg = input;
    uint64_t original_length = msg.length() * 8;
    
    // Pridėti '1' bitą (0x80 byte)
    msg += char(0x80);
    
    // Pridėti nulius iki 448 mod 512 = 56 mod 64 bytes
    while ((msg.length() % 64) != 56) {
        msg += char(0x00);
    }
    
    // Pridėti originalų ilgį kaip 64-bit little-endian
    for (int i = 0; i < 8; ++i) {
        msg += char((original_length >> (i * 8)) & 0xFF);
    }
    
    // Apdoroti kiekvieną 512-bit bloką
    for (size_t chunk_start = 0; chunk_start < msg.length(); chunk_start += 64) {
        uint32_t w[16];
        
        // Nukopijuoti bloką į w[] kaip little-endian 32-bit žodžius
        for (int i = 0; i < 16; ++i) {
            w[i] = 0;
            for (int j = 0; j < 4; ++j) {
                w[i] |= uint32_t(uint8_t(msg[chunk_start + i * 4 + j])) << (j * 8);
            }
        }
        
        // Inicializuoti hash reikšmes šiam blokui
        uint32_t a = h0, b = h1, c = h2, d = h3;
        
        // Pagrindiniai MD5 žingsniai
        for (int i = 0; i < 64; ++i) {
            uint32_t f, g;
            
            if (i < 16) {
                f = F(b, c, d);
                g = i;
            } else if (i < 32) {
                f = G(b, c, d);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                f = H(b, c, d);
                g = (3 * i + 5) % 16;
            } else {
                f = I(b, c, d);
                g = (7 * i) % 16;
            }
            
            f = f + a + MD5_K[i] + w[g];
            a = d;
            d = c;
            c = b;
            b = b + leftrotate(f, MD5_S[i]);
        }
        
        // Pridėti šio bloko hash prie bendro rezultato
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }
    
    // Sukurti galutinį hash kaip hex string (little-endian)
    stringstream ss;
    ss << hex << setfill('0');
    
    auto write_little_endian = [&ss](uint32_t value) {
        for (int i = 0; i < 4; ++i) {
            ss << setw(2) << ((value >> (i * 8)) & 0xFF);
        }
    };
    
    write_little_endian(h0);
    write_little_endian(h1);
    write_little_endian(h2);
    write_little_endian(h3);
    
    return ss.str();
}

