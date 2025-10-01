#include "ownHash.h"
#include "constants.h"

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <bit> // PATOBULINIMAS: std::rotl C++20 efektyvesniam bit rotation

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::cerr;

namespace {
    // pavercia sveika skaiciu i base62 simboli
    inline char to_base62(int sk) {
        sk %= 62;
        if (sk < 0) sk += 62;
        return BASE62[sk];
    }

    // PATOBULINIMAS: pridėtas bit rotation geresniam diffusion
    inline uint32_t rotl32(uint32_t value, int shift) {
        shift &= 31; // apsauga nuo per didelio shift
        return (value << shift) | (value >> (32 - shift));
    }

    // PATOBULINIMAS: pridėtas xor maisymas stipresniam avalanche efektui
    inline int enhanced_mix(int value, int salt, int position) {
        uint32_t v = static_cast<uint32_t>(value);
        uint32_t s = static_cast<uint32_t>(salt);
        uint32_t p = static_cast<uint32_t>(position);
        
        // kompleksinis maisymas su bit rotation ir xor
        v ^= rotl32(s, 7) + p * 0x9E3779B9; // golden ratio konstanta
        v = rotl32(v, 13) ^ (v >> 16);
        v *= 0x85EBCA6B; // kita magic konstanta diffusion gerinimui
        v ^= v >> 13;
        v *= 0xC2B2AE35;
        v ^= v >> 16;
        
        return static_cast<int>(v & 0xFF); // grąžinu 0-255 intervale
    }

    // elementai maisomi priklausomai nuo ju vertes
    // PATOBULINIMAS: pridėtas papildomas XOR mixing geresniam avalanche
    void value_dependent_shuffle(vector<int>& previous) {
        if (previous.empty()) return;
        
        vector<int> temp = previous; // kopija
        const size_t n = previous.size();
        
        for (size_t i = 0; i < n; ++i) {
            int value = temp[i];
            size_t new_pos;
            
            if (value % 2 == 0) {  // lyginis - keliauja i prieki
                size_t jump = (17 * abs(value) * 7 + i * 23) % n; // pozicijos itaka
                new_pos = (i + jump) % n;
            } else {  // nelyginis - keliauja atgal
                size_t jump = (13 * abs(value) * 11 + i * 19) % n;  // pozicijos itaka
                new_pos = (i + n - (jump % n)) % n;  // apsauga nuo underflow
            }
            
            // PATOBULINIMAS: pridėtas enhanced_mix stipresniam bit diffusion
            previous[new_pos] = enhanced_mix(value, temp[(i + 1) % n], static_cast<int>(i));
        }
    }

    // padalina per puse ir sukeicia vietomis
    // PATOBULINIMAS: pridėtas cross-mixing tarp pusių
    void swap_halves(vector<int>& previous) {
        size_t n = previous.size();
        size_t h = n / 2; // antroji pusė bus ilgesnė jei nelyginis dydis
        vector<int> first(previous.begin(), previous.begin() + h);
        vector<int> second(previous.begin() + h, previous.end());
        
        // PATOBULINIMAS: cross-mixing tarp pusių prieš sukeičiant
        for (size_t i = 0; i < std::min(first.size(), second.size()); ++i) {
            int mix1 = enhanced_mix(first[i], second[i], static_cast<int>(i));
            int mix2 = enhanced_mix(second[i], first[i], static_cast<int>(i + h));
            first[i] = mix1;
            second[i] = mix2;
        }
        
        previous.clear();
        previous.insert(previous.end(), second.begin(), second.end());
        previous.insert(previous.end(), first.begin(), first.end());
    }

    // pagrindinis masymas - kiekvienas elementas paveiks 3 kitus
    // PATOBULINIMAS: stiprintas avalanche efektas su nelinijiniu mixing
    void three_in_one_mixer(vector<int>& previous) {
        if (previous.empty()) return;
        vector<int> temp = previous; // kopija, kad turet senas reiksmes
        
        for (size_t i = 0; i < temp.size(); ++i) {
            int sk = temp[i];

            size_t e1 = (i + sk) % previous.size();
            size_t e2 = (i + sk * 2) % previous.size();
            size_t e3 = (i + sk * 3) % previous.size();

            // PATOBULINIMAS: nelinijikas mixing vietoj paprastos sumos
            previous[e1] = enhanced_mix(previous[e1] + sk, sk, static_cast<int>(i));
            previous[e2] = enhanced_mix(previous[e2] + sk * 2, sk * 2, static_cast<int>(i + 1));
            previous[e3] = enhanced_mix(previous[e3] + sk * 3, sk * 3, static_cast<int>(i + 2));
            
            // PATOBULINIMAS: papildomas feedback loop geresniam diffusion
            size_t feedback_pos = (i + previous[e1] + previous[e2] + previous[e3]) % previous.size();
            previous[feedback_pos] ^= rotl32(sk, static_cast<int>(i % 32)) & 0xFF;
        }
    }

    // generuoju seed priklausomai nuo input simboliu
    // PATOBULINIMAS: stiprintas seed generavimas su nelinijiniu mixing
    string generate_seed(const vector<int>& current) {
        string seed = "Kx9mN3vL8qR5wY1pZ7jT2bF6hC4nA0sD";
        
        // PATOBULINIMAS: pakeista į sudėtingesnę matricą geresniam diffusion
        int matrix[2][2] = {{17, 29}, {23, 31}}; // pirminiai skaičiai
        
        for (size_t i = 0; i + 1 < current.size(); i += 2) {
            int a = current[i];
            int b = current[i + 1];
            
            size_t pos1 = i % seed.size();
            size_t pos2 = (i + 1) % seed.size();
            
            // PATOBULINIMAS: enhanced mixing matricų operacijoms
            int mix1 = enhanced_mix(matrix[0][0] * a + matrix[0][1] * b, a + b, static_cast<int>(i));
            int mix2 = enhanced_mix(matrix[1][0] * a + matrix[1][1] * b, a ^ b, static_cast<int>(i + 1));
            
            seed[pos1] = BASE62[mix1 % 62];
            seed[pos2] = BASE62[mix2 % 62];
        }
        
        // jei liko vienas elementas nelyginiame masyve
        if (current.size() % 2 == 1) {
            size_t last_idx = current.size() - 1;
            size_t pos = last_idx % seed.size();
            // PATOBULINIMAS: enhanced mixing paskutiniam elementui
            int mix = enhanced_mix(current[last_idx] * 17, static_cast<int>(pos * 23), static_cast<int>(last_idx));
            seed[pos] = BASE62[mix % 62];
        }
        
        return seed;
    }

} // namespace

string generate_hash(const string& user_input) {
    // 1) kiekviena simboli paverciu i ascii koda
    vector<int> current;
    current.reserve(user_input.size());
    for (unsigned char c : user_input) current.push_back((int)c);

    // jei ivestis tuscia, pridedu nuli kad hash vis tiek butu sugeneruotas
    if (current.empty()) {
        current.push_back(0);
    }

    // 2) ivairiausi maisymai hash generavimo vyksta 4 roundai 
    // PATOBULINIMAS: padidinti rounds kiekį geresniam security
    constexpr int ROUNDS = 6; // buvo 4, dabar 6 - stipresnis security

    // dabartinio roundo duomenys
    vector<int> data = current;

    // paskutinio raundo duomenu kopija
    vector<int> previous;

    for (int r = 0; r < ROUNDS; ++r) {
        // a) naudojam originalius duomenis be papildomo padding
        previous = data;

        // b) maisymas priklausomai nuo elemento reiksmes - efektyvus diffusion
        value_dependent_shuffle(previous);

        // c) maisymas kur vienas elementas paveikia kitus 3 - stipriausias efektas
        three_in_one_mixer(previous);

        // d) padalinu masyva per puse ir sukeiciu dalis vietomis - finalus permutation
        swap_halves(previous);

        // PATOBULINIMAS: pridėtas inter-round mixing
        if (r < ROUNDS - 1) { // ne paskutinis raundas
            for (size_t i = 0; i < previous.size(); ++i) {
                previous[i] = enhanced_mix(previous[i], r + 1, static_cast<int>(i));
            }
        }

        // jei tuscia ivestis
        if (previous.empty()) {
            previous.push_back(0);
        }

        // kitas raundas naudos dabartini masyva
        data = previous;
    }

    // 3) generuoju seed priklausomai nuo input simboliu
    string seed = generate_seed(current);
    
    // 4) salt generavimas ir integravimas
    // PATOBULINIMAS: stiprintas salt mechanizmas
    string salt;
    if (!current.empty()) {
        // salt generuojamas is input charakteristiku
        size_t input_sum = 0;
        for (int ascii : current) input_sum += ascii;
        
        // PATOBULINIMAS: sudėtingesnis salt generavimas su XOR
        for (int i = 0; i < 4; ++i) {
            int salt_val = enhanced_mix(
                static_cast<int>(input_sum * (i + 7)), 
                current[i % current.size()] * 13, 
                i
            );
            salt += to_base62(salt_val);
        }
        
        // integruoju salt i seed
        // PATOBULINIMAS: sudėtingesnis salt integravimas
        for (size_t i = 0; i < salt.size(); ++i) {
            int salt_ascii = (int)(unsigned char)salt[i];
            size_t salt_pos = (input_sum + i * salt_ascii) % seed.size();
            int mixed_val = enhanced_mix(
                (int)(unsigned char)seed[salt_pos] + salt_ascii,
                static_cast<int>(input_sum),
                static_cast<int>(i)
            );
            seed[salt_pos] = to_base62(mixed_val);
        }
    }

    // 5) maisymas su seed 
    // PATOBULINIMAS: stiprintas seed mixing su enhanced funkcija
    for (size_t i = 0; i < previous.size(); ++i) {
        size_t si = i % seed.size(); // seed indeksa sukame ratu
        int seed_val = (int)(unsigned char)seed[si];
        int data_val = previous[i];
        int pos_factor = static_cast<int>(i + 1);
        
        // PATOBULINIMAS: naudojam enhanced_mix vietoj paprastos daugybos
        int mixed = enhanced_mix(seed_val * data_val, pos_factor, static_cast<int>(i));
        seed[si] = to_base62(mixed % 62); // mod 62 kad griztu i base62 simboli
    }

    // 6) sukuriu galutini hash – 64 base62 simboliai
    // PATOBULINIMAS: finalizavimo etapas su stipresniu mixing
    string out;
    out.reserve(64);
    for (int i = 0; i < 64; ++i) {
        int a = (int)(unsigned char)seed[i % seed.size()]; // seed simbolis (ratu)
        int b = previous[i % previous.size()]; // masyvo elementas (ratu)
        
        // PATOBULINIMAS: enhanced finalization su nelinijiniu mixing
        int final_mix = enhanced_mix(a + b, i * 17, i);
        out.push_back(to_base62(final_mix % 62));
    }

    return out;
}
