#include "functions.h"
#include "constants.h"

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::cerr;

namespace {
    // pavercia sveika skaiciu i base62 simboli
    inline char to_base62(int v) {
        v %= 62;
        if (v < 0) v += 62;
        return BASE62[v];
    }

    // padalina i blokus po 4 elementus
    // jei paskutinis blokas trumpesnis – pildom modulo 64 ziurint nuo pradzios
    // trukstamus elementus ima is pradinio masyvo su indeksu % min(64, n)
    vector<int> split_into_blocks_pad4(const vector<int>& nums) {
        vector<int> out;
        out.reserve(((nums.size() + 3) / 4) * 4);

        const size_t n = nums.size();
        const size_t pad_domain = std::min<size_t>(64, n == 0 ? 1 : n);

        for (size_t i = 0; i < n; i += 4) {
            size_t rem = std::min<size_t>(4, n - i);
            for (size_t k = 0; k < rem; ++k) out.push_back(nums[i + k]);
            if (rem < 4) {
                for (size_t k = rem; k < 4; ++k) {
                    size_t idx = (k - rem) % pad_domain;
                    out.push_back(n == 0 ? 0 : nums[idx]);
                }
            }
        }
        return out;
    }

    // elementai maisomi priklausomai nuo ju vertes
    void value_dependent_shuffle(vector<int>& flat_blocks) {
        if (flat_blocks.empty()) return;
        
        vector<int> temp = flat_blocks; // kopija
        const size_t n = flat_blocks.size();
        
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
            
            flat_blocks[new_pos] = value;
        }
    }

    // apvercia visa masyva 
    template <class T>
    void reverse_all(vector<T>& v) {
        std::reverse(v.begin(), v.end());
    }

    // padalina per puse ir sukeicia vietomis
    template <class T>
    void swap_halves(vector<T>& v) {
        size_t n = v.size();
        size_t h = n / 2;
        // jei n nelyginis – antroji puse bus +1 elementu ilgesne; sukeiciam segmentais
        vector<T> first(v.begin(), v.begin() + h);
        vector<T> second(v.begin() + h, v.end());
        v.clear();
        v.insert(v.end(), second.begin(), second.end());
        v.insert(v.end(), first.begin(), first.end());
    }

    // ASCII masymas - kiekvienas elementas paveiks 3 kitus 
    void ascii_mixer(vector<int>& blocks) {
        if (blocks.empty()) return;
        vector<int> temp = blocks; // kopija, kad nepaveiktu vienas kito
        
        for (size_t i = 0; i < temp.size(); ++i) {
            int val = temp[i];
            
            size_t target1 = (i + abs(val)) % blocks.size();
            size_t target2 = (i + abs(val) * 2) % blocks.size();
            size_t target3 = (i + abs(val) * 3) % blocks.size();
            
            blocks[target1] = (blocks[target1] + val) % 256;
            blocks[target2] = (blocks[target2] + val * 2) % 256;
            blocks[target3] = (blocks[target3] + val * 3) % 256;
        }
    }

} // namespace

string generate_hashe(const string& user_input) {
    // 1) kiekviena simboli paverciu i ascii koda
    vector<int> ascii_vals;
    ascii_vals.reserve(user_input.size());
    for (unsigned char c : user_input) ascii_vals.push_back((int)c);

    // jiei ivestis tuscia vistiek generuoju hash viska uzpildant nuliais
    if (ascii_vals.empty()) {
        ascii_vals.push_back(0);
    }

    // 2) ivairiausi maisyma hash generavimo vyksta 4 roundai
    constexpr int ROUNDS = 4;

    // dabartinio roundo duomenys
    vector<int> state = ascii_vals;

    // paskutinio raundo duomenu kopija
    vector<int> blocks_flat;

    for (int r = 0; r < ROUNDS; ++r) {
        // a) padalinu skaicius i blokus po 4 su pildymu modulo 64 nuo pradzios
        blocks_flat = split_into_blocks_pad4(state);

        // b) apverciu bloku eile (po 4 elementus)
        vector<std::array<int,4>> blocks;
        blocks.reserve(blocks_flat.size() / 4);
        for (size_t i = 0; i < blocks_flat.size(); i += 4) {
            blocks.push_back(std::array<int,4>{
                blocks_flat[i+0], blocks_flat[i+1], blocks_flat[i+2], blocks_flat[i+3]
            });
        }
        std::reverse(blocks.begin(), blocks.end()); // apverciu bloku tvarka

        blocks_flat.clear();
        blocks_flat.reserve(blocks.size() * 4);
        for (auto &b : blocks) { // d) vel sujungiu i viena masyva
            blocks_flat.push_back(b[0]);
            blocks_flat.push_back(b[1]);
            blocks_flat.push_back(b[2]);
            blocks_flat.push_back(b[3]);
        }

        // c) maisymas priklausomai nuo elemento reiksmes
        value_dependent_shuffle(blocks_flat);

        // e) apverciu visa masyva
        reverse_all(blocks_flat);

        // f) padalinu masyva per puse ir sukeiciu dalis vietomis
        swap_halves(blocks_flat);

        // g) ASCII maisyma kur vienas elementas paveikia kitus 3
        ascii_mixer(blocks_flat);

        // jei tuscia ivestis
        if (blocks_flat.empty()) {
            blocks_flat.push_back(0);
        }

        // kitas raundas naudos dabartini masyva
        state = blocks_flat;
    }

    // 3) seed su kuriuo maisysiu
    string seed = "Kx9mN3vL8qR5wY1pZ7jT2bF6hC4nA0sD"; // 32 simboliai

    // 4) maisymas su seed 
    for (size_t i = 0; i < blocks_flat.size(); ++i) {
        size_t si = i % seed.size(); // seed indeksa sukame ratu
        int rez = ((int)(unsigned char)seed[si] * blocks_flat[i] * (i + 1)) % 256; // daugyba + pozicijos poveikis + mod 256
        seed[si] = to_base62(rez % 62); // mod 62 kad griztu i base62 simboli
    }

    // 5-6) sukuriu galutini hash – 64 base62 simboliai
    string out;
    out.reserve(64);
    for (int i = 0; i < 64; ++i) {
        int a = (int)(unsigned char)seed[i % seed.size()]; // seed simbolis (ratu)
        int b = blocks_flat[i % blocks_flat.size()]; // masyvo elementas (ratu)
        int v = (a + b + i * 17) % 62; // pozicijos itaka ir mod 62
        out.push_back(to_base62(v));
    }

    return out;
}
