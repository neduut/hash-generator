#include "vanesa_hash.h"
#include <sstream>
#include <iomanip>
#include <bitset>
#include <map>
#include <random>
#include <locale>
#include <codecvt>
#include <cstdint>

static std::map<wchar_t, uint16_t> getLithuanianCharMap() {
    std::map<wchar_t, uint16_t> charMap;
    charMap[L'ą'] = 0xC485; charMap[L'č'] = 0xC48D; charMap[L'ę'] = 0xC499;
    charMap[L'ė'] = 0xC497; charMap[L'į'] = 0xC4AF; charMap[L'š'] = 0xC5A1;
    charMap[L'ų'] = 0xC5B3; charMap[L'ū'] = 0xC5AB; charMap[L'ž'] = 0xC5BE;
    charMap[L'Ą'] = 0xC484; charMap[L'Č'] = 0xC48C; charMap[L'Ę'] = 0xC498;
    charMap[L'Ė'] = 0xC496; charMap[L'Į'] = 0xC4AE; charMap[L'Š'] = 0xC5A0;
    charMap[L'Ų'] = 0xC5B2; charMap[L'Ū'] = 0xC5AA; charMap[L'Ž'] = 0xC5BD;
    return charMap;
}

static std::string convertLithuanianText(const std::string &input) {
    static auto charMap = getLithuanianCharMap();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wide = conv.from_bytes(input);
    std::string result;
    
    for (wchar_t wc : wide) {
        auto it = charMap.find(wc);
        if (it != charMap.end()) {
            uint16_t code = it->second;
            result.push_back(static_cast<char>((code >> 8) & 0xFF));
            result.push_back(static_cast<char>(code & 0xFF));
        } else {
            std::string utf8char = conv.to_bytes(wc);
            result += utf8char;
        }
    }
    return result;
}

static uint32_t safeStringToUint32(const std::string& str, const std::string& seedui) {
    std::string truncated = str;
    if (truncated.length() > 9) {
        truncated = truncated.substr(0, 9);
    }
    
    uint32_t seed = 0;
    for (unsigned char c : seedui) {
        seed = seed * 31 + c;
    }
    uint32_t hash = seed;
    
    for (unsigned char c : truncated) {
        hash = hash * seed + c;
    }
    return hash;
}

std::string generate_vanesa_hash(const std::string &ivestis) {
    std::string isvestis;
    std::string konvertuotasIvestis = convertLithuanianText(ivestis);
    
    std::string seedString;
    std::string seedui;
    if (!konvertuotasIvestis.empty()) {
        for (size_t i = 0; i < konvertuotasIvestis.size(); i += 1000) {
            int suma = 0;
            for (size_t j = i; j < i + 10 && j < konvertuotasIvestis.size(); j++) {
                suma += static_cast<unsigned char>(konvertuotasIvestis[j]);
            }
            seedString += std::to_string(suma);
        }
        
        for (size_t i = 0; i < konvertuotasIvestis.size(); i += 20) {
            int ones = 0;
            for (size_t j = i; j < i + 20 && j < konvertuotasIvestis.size(); j++) {
                std::bitset<8> bits(static_cast<unsigned char>(konvertuotasIvestis[j]));
                ones += bits.count();
            }
            seedString += std::to_string(ones);
            seedui += std::to_string(ones);
        }
    } else {
        seedString = "0";
    }
    
    std::string binaryInput;
    for (unsigned char c : konvertuotasIvestis) {
        binaryInput += std::bitset<8>(c).to_string();
    }
    
    if (binaryInput.empty()) {
        binaryInput = "10000000";
    }
    
    std::string originalBinary = binaryInput;
    while (binaryInput.size() < 256) {
        std::string toAdd = originalBinary;
        for (size_t i = 0; i < toAdd.size() && binaryInput.size() < 256; i++) {
            char newBit = (binaryInput[i % binaryInput.size()] == toAdd[i]) ? '0' : '1';
            binaryInput += newBit;
        }
    }
    if (binaryInput.size() > 256) {
        binaryInput = binaryInput.substr(0, 256);
    }
    
    uint32_t mySeed = safeStringToUint32(seedString, seedui);
    std::mt19937 rng(mySeed);
    
    std::string mixedBinary;
    for (size_t i = 0; i < binaryInput.size(); i += 32) {
        uint32_t randVal = rng();
        for (size_t j = 0; j < 32 && i + j < binaryInput.size(); j++) {
            int bit = binaryInput[i + j] - '0';
            int rbit = (randVal >> j) & 1;
            bit ^= rbit;
            mixedBinary.push_back(bit ? '1' : '0');
        }
    }
    
    for (size_t i = 0; i + 4 <= mixedBinary.size(); i += 4) {
        std::string nibble = mixedBinary.substr(i, 4);
        int value = std::stoi(nibble, nullptr, 2);
        std::stringstream ss;
        ss << std::hex << value;
        isvestis += ss.str();
    }
    return isvestis;
}
