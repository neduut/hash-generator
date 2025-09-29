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
    // (t.y. trukstamus elementus ima is pradinio masyvo su indeksu % min(64, n))
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

    // pritaiko permutacijos formule kas antram blokui (zingsnis 4)
    void permute_every_second_block(vector<int>& flat_blocks) {
        for (size_t b = 0; b * 4 < flat_blocks.size(); ++b) {
            const bool is_second = ((b + 1) % 2 == 0); 
            if (!is_second) continue;
            int temp[4];
            // out[new_index] = in[index], kur new_index = (index*3 + 1) % 4
            for (int index = 0; index < 4; ++index) {
                int new_index = (index * 3 + 1) % 4;
                temp[new_index] = flat_blocks[b * 4 + index];
            }
            for (int i = 0; i < 4; ++i) flat_blocks[b*4 + i] = temp[i];
        }
    }

    // apvercia visa masyva (zingsnis 6)
    template <class T>
    void reverse_all(vector<T>& v) {
        std::reverse(v.begin(), v.end());
    }

    // padalina per puse ir sukeicia vietomis (zingsnis 7)
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

} // namespace

string generate_hashe(const string& user_input) {
    // 1) kiekviena simboli paverciu i ascii koda
    vector<int> ascii_vals;
    ascii_vals.reserve(user_input.size());
    for (unsigned char c : user_input) ascii_vals.push_back((int)c);

    // leidziu tuscia ivesti
    if (ascii_vals.empty()) {
        ascii_vals.push_back(0);
    }

    // viskas vyksta 8 ROUNDUS
    constexpr int ROUNDS = 4;

    // 8) pradinis seed (kad net trumpas ivedimas duotu 64 simboliu hash)
    string seed = "A1b2C3d4E5f6G7h8I9j0K1l2M3n4O5p6"; // 32 simboliai

    // "state" – einamojo raundo įėjimas; pradedam nuo ASCII
    vector<int> state = ascii_vals;

    // paskutinio raundo blocks_flat saugosim čia – bus reikalingas finalui
    vector<int> blocks_flat;

    for (int r = 0; r < ROUNDS; ++r) {
        // 2) padalinu skaicius i blokus po 4 su pildymu modulo 64 nuo pradzios
        blocks_flat = split_into_blocks_pad4(state);

        // 3) apverciu bloku eile (po 4 elementus) – bloku lygmeniu
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
        for (auto &b : blocks) { // 5) vel sujungiu i viena masyva
            blocks_flat.push_back(b[0]);
            blocks_flat.push_back(b[1]);
            blocks_flat.push_back(b[2]);
            blocks_flat.push_back(b[3]);
        }

        // 4) kiekvieno antro bloko elementus sukeiciu pagal formule
        permute_every_second_block(blocks_flat);

        // 5) visi blokai sujungti i viena masyva (blocks_flat) jau padaryta anksciau

        // 6) apverciu visa masyva
        reverse_all(blocks_flat);

        // 7) padalinu masyva per puse ir sukeiciu dalis vietomis
        swap_halves(blocks_flat);

        // jei tuscias (pvz. ivesta tuscia eilute), tai kad nebutu problemu
        if (blocks_flat.empty()) {
            blocks_flat.push_back(0);
        }

        // 9) kiekviena masyvo elementa imaisau i seed simboli
        for (size_t i = 0; i < blocks_flat.size(); ++i) {
            size_t si = i % seed.size(); // seed indeksa sukame ratu
            int sum = (int)(unsigned char)seed[si] + blocks_flat[i]; // seed ascii + masyvo elementas
            seed[si] = to_base62(sum % 62); // mod 62 -> base62 simbolis
        }

        // kitas raundas naudos dabartini masyva
        state = blocks_flat;
    }

    // 10–11) sukuriu galutini hash – 64 base62 simboliai (naudojam paskutinio raundo blocks_flat)
    string out;
    out.reserve(64);
    for (int i = 0; i < 64; ++i) {
        int a = (int)(unsigned char)seed[i % seed.size()]; // seed simbolis (ratu)
        int b = blocks_flat[i % blocks_flat.size()]; // masyvo elementas (ratu)
        int v = (a + b + i * 17) % 62; // pozicijos itaka ir mod 62
        out.push_back(to_base62(v));
    }

    return out; // 64 base62 simboliai
}
