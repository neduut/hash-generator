#include "gustavo_hash.h"
#include <bitset>
#include <sstream>
#include <cmath>

static std::string wordToBinary(const std::string& text) {
    std::string binary;
    for (unsigned char c : text) {
        binary += std::bitset<8>(c).to_string();
    }
    return binary;
}

static char fourNumbersToHex(const std::string& nibble) {
    int value = std::bitset<4>(nibble).to_ulong();
    if (value < 10) return '0' + value;
    return 'A' + (value - 10);
}

static std::string binaryToHex(const std::string& binary) {
    std::string hex;
    for (size_t i = 0; i < binary.size(); i += 4) {
        hex += fourNumbersToHex(binary.substr(i, 4));
    }
    return hex;
}

std::string generate_gustavo_hash(const std::string& input) {
    std::string data = input;
    int seed = (data.length() % 10) + 9;
    std::string working = wordToBinary(data);
    while (working.length() < 32) {
        working += working;
        working += "1011101";
    }
    
    int amount_of_1 = 0;
    int amount_of_0 = 0;
    for (size_t i = 0; i < working.length(); i++) {
        if (working[i] == '1')
            amount_of_1 += i;
        else
            amount_of_0 += i;
    }
    
    if (amount_of_1 < 0) amount_of_1 *= -1;
    if (amount_of_0 < 0) amount_of_0 *= -1;
    
    int bigger = ((amount_of_0 > amount_of_1) ? amount_of_0 : amount_of_1) * seed;
    int smaller = ((amount_of_0 < amount_of_1) ? amount_of_0 : amount_of_1);
    if ((bigger % smaller) == 0)
        smaller++;
    
    if (bigger < 0) bigger *= -1;
    if (smaller < 0) smaller *= -1;
    
    if (bigger > 100000) {
        bigger = bigger % 1000000 + 1000000;
        smaller = smaller % 1000000 + 100000;
    }
    
    while (working.size() < 256) {
        working += working;
    }
    
    int current = 0;
    for (int i = 0; i < bigger; i++) {
        char temp = working[current];
        int next_pos = (current + smaller - i) % working.length();
        working[current] = working[next_pos];
        working[next_pos] = temp;
        current = next_pos;
    }
    
    working = binaryToHex(working);
    working = wordToBinary(working);
    
    current = 0;
    for (int i = 0; i < bigger; i++) {
        char temp = working[current];
        int next_pos = (current + smaller - i) % working.length();
        working[current] = working[next_pos];
        working[next_pos] = temp;
        current = next_pos;
    }
    
    std::string Hashed(256, '\0');
    for (int i = 0; i < 256; i++) {
        Hashed[i] = working[i];
    }
    
    return binaryToHex(Hashed);
}
