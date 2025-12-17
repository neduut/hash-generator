#include "juste_hash.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <random>
#include <omp.h>

using namespace std;

// Pagalbinė funkcija bitų skirtumui skaičiuoti
int count_bit_differences(const string& h1, const string& h2) {
    int diff = 0;
    size_t min_len = min(h1.length(), h2.length());
    
    for (size_t i = 0; i < min_len; ++i) {
        unsigned char c1 = h1[i];
        unsigned char c2 = h2[i];
        unsigned char xor_result = c1 ^ c2;
        
        // Skaičiuojame 1-us bituose
        for (int j = 0; j < 8; ++j) {
            if (xor_result & (1 << j)) diff++;
        }
    }
    return diff;
}

// Pagalbinė funkcija hex skirtumui skaičiuoti
int count_hex_differences(const string& h1, const string& h2) {
    int diff = 0;
    size_t min_len = min(h1.length(), h2.length());
    
    for (size_t i = 0; i < min_len; ++i) {
        if (h1[i] != h2[i]) diff++;
    }
    return diff;
}

// Generuoja atsitiktinį stringą
string random_string(int length) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    
    static random_device rd;
    static mt19937 gen(rd());
    static uniform_int_distribution<> dis(0, sizeof(alphanum) - 2);
    
    string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += alphanum[dis(gen)];
    }
    return result;
}

void test_performance_comparison() {
    cout << "\n=== EFEKTYVUMO PALYGINIMAS ===\n\n";
    
    // Nuskaitome failą
    ifstream file("files/konstitucija.txt");
    if (!file) {
        cerr << "Klaida: nepavyko atidaryti files/konstitucija.txt\n";
        return;
    }
    
    vector<string> lines;
    string line;
    while (getline(file, line)) {
        lines.push_back(line);
    }
    file.close();
    
    vector<int> line_counts = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, (int)lines.size()};
    
    cout << "| Eilučių kiekis | Justė |\n";
    cout << "|:---:|:---:|\n";
    
    for (int count : line_counts) {
        if (count > (int)lines.size()) count = lines.size();
        
        string input;
        for (int i = 0; i < count; ++i) {
            input += lines[i];
            if (i + 1 < count) input += " ";
        }
        
        // Justė
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i) {
            volatile auto h = justeHash(input);
        }
        auto end = chrono::high_resolution_clock::now();
        double juste_time = chrono::duration<double, milli>(end - start).count() / 10.0;
        
        string count_str = (count == (int)lines.size()) ? "Visas failas" : to_string(count) + (count == 1 ? " eilutė" : " eilutės");
        
        cout << "| " << count_str << " | " 
             << fixed << setprecision(2) << juste_time << " |\n";
    }
}

void test_collision_resistance() {
    cout << "\n=== ATSPARUMAS KOLIZIJOMS ===\n\n";
    
    const int NUM_PAIRS = 100000;
    vector<int> lengths = {10, 100, 500, 1000};
    
    cout << "| Vieno string poroje ilgis | Justė |\n";
    cout << "|:---:|:---:|\n";
    
    omp_set_num_threads(24);
    
    for (int len : lengths) {
        int juste_collisions = 0;
        
        #pragma omp parallel for reduction(+:juste_collisions)
        for (int i = 0; i < NUM_PAIRS; ++i) {
            string s1 = random_string(len);
            string s2 = random_string(len);
            
            if (justeHash(s1) == justeHash(s2) && s1 != s2) juste_collisions++;
        }
        
        cout << "| " << len << " | "
             << juste_collisions << " |\n";
    }
}

void test_avalanche_effect() {
    cout << "\n=== LAVINOS EFEKTAS ===\n\n";
    
    const int NUM_PAIRS = 100000;
    const int STR_LEN = 200;
    
    omp_set_num_threads(24);
    
    // Statistika
    struct Stats {
        double bit_min = 100.0, bit_max = 0.0, bit_sum = 0.0;
        double hex_min = 100.0, hex_max = 0.0, hex_sum = 0.0;
    };
    
    Stats juste_stats;
    
    #pragma omp parallel
    {
        Stats local_juste;
        
        #pragma omp for
        for (int i = 0; i < NUM_PAIRS; ++i) {
            string s1 = random_string(STR_LEN);
            string s2 = s1;
            
            // Pakeičiame vieną atsitiktinį simbolį
            int pos = rand() % s2.length();
            s2[pos] = (s2[pos] == 'a') ? 'b' : 'a';
            
            // Justė
            string h1_juste = justeHash(s1);
            string h2_juste = justeHash(s2);
            int bit_diff_juste = count_bit_differences(h1_juste, h2_juste);
            int hex_diff_juste = count_hex_differences(h1_juste, h2_juste);
            double bit_pct_juste = (bit_diff_juste * 100.0) / (h1_juste.length() * 8);
            double hex_pct_juste = (hex_diff_juste * 100.0) / h1_juste.length();

            local_juste.bit_min = min(local_juste.bit_min, bit_pct_juste);
            local_juste.bit_max = max(local_juste.bit_max, bit_pct_juste);
            local_juste.bit_sum += bit_pct_juste;
            local_juste.hex_min = min(local_juste.hex_min, hex_pct_juste);
            local_juste.hex_max = max(local_juste.hex_max, hex_pct_juste);
            local_juste.hex_sum += hex_pct_juste;
        }
        
        #pragma omp critical
        {
            juste_stats.bit_min = min(juste_stats.bit_min, local_juste.bit_min);
            juste_stats.bit_max = max(juste_stats.bit_max, local_juste.bit_max);
            juste_stats.bit_sum += local_juste.bit_sum;
            juste_stats.hex_min = min(juste_stats.hex_min, local_juste.hex_min);
            juste_stats.hex_max = max(juste_stats.hex_max, local_juste.hex_max);
            juste_stats.hex_sum += local_juste.hex_sum;
        }
    }
    
    cout << "### Lavinos efektas - BIT skirtumas\n\n";
    cout << "| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |\n";
    cout << "|:---:|:---:|:---:|:---:|\n";
    cout << "| Justė | " << fixed << setprecision(2) << juste_stats.bit_max << "% | " 
         << juste_stats.bit_min << "% | " << (juste_stats.bit_sum / NUM_PAIRS) << "% |\n";
    
    cout << "\n### Lavanos efektas - HEX skirtumas\n\n";
    cout << "| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |\n";
    cout << "|:---:|:---:|:---:|:---:|\n";
    cout << "| Justė | " << juste_stats.hex_max << "% | " 
         << juste_stats.hex_min << "% | " << (juste_stats.hex_sum / NUM_PAIRS) << "% |\n";
}

int main() {
    cout << "=== JUSTĖS HASH TESTAVIMAS ===\n";
    cout << "Naudojami 24 thread'ai testavimui\n\n";
    
    test_performance_comparison();
    test_collision_resistance();
    test_avalanche_effect();
    
    cout << "\n=== TESTAI BAIGTI ===\n";
    
    return 0;
}
