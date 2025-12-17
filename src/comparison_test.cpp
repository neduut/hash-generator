#include "mylib.h"
#include "ownHash.h"
#include "sha256.h"
#include "md5.h"
#include "vanesa_hash.h"
#include "constants.h"

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
    
    cout << "| Eilučių kiekis | Neda (v0.1final) | SHA-256 | MD5 | Vanesė |\n";
    cout << "|:---:|:---:|:---:|:---:|:---:|\n";
    
    for (int count : line_counts) {
        if (count > (int)lines.size()) count = lines.size();
        
        string input;
        for (int i = 0; i < count; ++i) {
            input += lines[i];
            if (i + 1 < count) input += " ";
        }
        
        // Test ownHash
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i) {
            volatile auto h = generate_hash(input);
        }
        auto end = chrono::high_resolution_clock::now();
        double own_time = chrono::duration<double, milli>(end - start).count() / 10.0;
        
        // Test SHA-256
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i) {
            volatile auto h = sha256_hash(input);
        }
        end = chrono::high_resolution_clock::now();
        double sha_time = chrono::duration<double, milli>(end - start).count() / 10.0;
        
        // Test MD5
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i) {
            volatile auto h = md5_hash(input);
        }
        end = chrono::high_resolution_clock::now();
        double md5_time = chrono::duration<double, milli>(end - start).count() / 10.0;
        
        // Test Vanesė
        start = chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i) {
            volatile auto h = generate_vanesa_hash(input);
        }
        end = chrono::high_resolution_clock::now();
        double vanesa_time = chrono::duration<double, milli>(end - start).count() / 10.0;
        
        string count_str = (count == (int)lines.size()) ? "Visas failas" : to_string(count) + (count == 1 ? " eilutė" : " eilutės");
        
        cout << "| " << count_str << " | " 
             << fixed << setprecision(2) << own_time << " | "
             << sha_time << " | "
             << md5_time << " | "
             << vanesa_time << " |\n";
    }
}

void test_collision_resistance() {
    cout << "\n=== ATSPARUMAS KOLIZIJOMS ===\n\n";
    
    const int NUM_PAIRS = 100000;
    vector<int> lengths = {10, 100, 500, 1000};
    
    cout << "| Vieno string poroje ilgis | Neda (v0.1final) | SHA-256 | MD5 | Vanesė |\n";
    cout << "|:---:|:---:|:---:|:---:|:---:|\n";
    
    omp_set_num_threads(24);
    
    for (int len : lengths) {
        int own_collisions = 0;
        int sha_collisions = 0;
        int md5_collisions = 0;
        int vanesa_collisions = 0;
        
        #pragma omp parallel for reduction(+:own_collisions,sha_collisions,md5_collisions,vanesa_collisions)
        for (int i = 0; i < NUM_PAIRS; ++i) {
            string s1 = random_string(len);
            string s2 = random_string(len);
            
            if (generate_hash(s1) == generate_hash(s2) && s1 != s2) own_collisions++;
            if (sha256_hash(s1) == sha256_hash(s2) && s1 != s2) sha_collisions++;
            if (md5_hash(s1) == md5_hash(s2) && s1 != s2) md5_collisions++;
            if (generate_vanesa_hash(s1) == generate_vanesa_hash(s2) && s1 != s2) vanesa_collisions++;
        }
        
        cout << "| " << len << " | " << own_collisions << " | " << sha_collisions << " | " << md5_collisions << " | " << vanesa_collisions << " |\n";
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
    
    Stats own_stats, sha_stats, md5_stats, vanesa_stats;
    
    #pragma omp parallel
    {
        Stats local_own, local_sha, local_md5, local_vanesa;
        
        #pragma omp for
        for (int i = 0; i < NUM_PAIRS; ++i) {
            string s1 = random_string(STR_LEN);
            string s2 = s1;
            
            // Pakeičiame vieną atsitiktinį simbolį
            int pos = rand() % s2.length();
            s2[pos] = (s2[pos] == 'a') ? 'b' : 'a';
            
            // OwnHash
            string h1_own = generate_hash(s1);
            string h2_own = generate_hash(s2);
            int bit_diff_own = count_bit_differences(h1_own, h2_own);
            int hex_diff_own = count_hex_differences(h1_own, h2_own);
            double bit_pct_own = (bit_diff_own * 100.0) / (h1_own.length() * 8);
            double hex_pct_own = (hex_diff_own * 100.0) / h1_own.length();
            
            local_own.bit_min = min(local_own.bit_min, bit_pct_own);
            local_own.bit_max = max(local_own.bit_max, bit_pct_own);
            local_own.bit_sum += bit_pct_own;
            local_own.hex_min = min(local_own.hex_min, hex_pct_own);
            local_own.hex_max = max(local_own.hex_max, hex_pct_own);
            local_own.hex_sum += hex_pct_own;
            
            // SHA-256
            string h1_sha = sha256_hash(s1);
            string h2_sha = sha256_hash(s2);
            int bit_diff_sha = count_bit_differences(h1_sha, h2_sha);
            int hex_diff_sha = count_hex_differences(h1_sha, h2_sha);
            double bit_pct_sha = (bit_diff_sha * 100.0) / (h1_sha.length() * 8);
            double hex_pct_sha = (hex_diff_sha * 100.0) / h1_sha.length();
            
            local_sha.bit_min = min(local_sha.bit_min, bit_pct_sha);
            local_sha.bit_max = max(local_sha.bit_max, bit_pct_sha);
            local_sha.bit_sum += bit_pct_sha;
            local_sha.hex_min = min(local_sha.hex_min, hex_pct_sha);
            local_sha.hex_max = max(local_sha.hex_max, hex_pct_sha);
            local_sha.hex_sum += hex_pct_sha;
            
            // MD5
            string h1_md5 = md5_hash(s1);
            string h2_md5 = md5_hash(s2);
            int bit_diff_md5 = count_bit_differences(h1_md5, h2_md5);
            int hex_diff_md5 = count_hex_differences(h1_md5, h2_md5);
            double bit_pct_md5 = (bit_diff_md5 * 100.0) / (h1_md5.length() * 8);
            double hex_pct_md5 = (hex_diff_md5 * 100.0) / h1_md5.length();
            
            local_md5.bit_min = min(local_md5.bit_min, bit_pct_md5);
            local_md5.bit_max = max(local_md5.bit_max, bit_pct_md5);
            local_md5.bit_sum += bit_pct_md5;
            local_md5.hex_min = min(local_md5.hex_min, hex_pct_md5);
            local_md5.hex_max = max(local_md5.hex_max, hex_pct_md5);
            local_md5.hex_sum += hex_pct_md5;
            
            // Vanesė
            string h1_vanesa = generate_vanesa_hash(s1);
            string h2_vanesa = generate_vanesa_hash(s2);
            int bit_diff_vanesa = count_bit_differences(h1_vanesa, h2_vanesa);
            int hex_diff_vanesa = count_hex_differences(h1_vanesa, h2_vanesa);
            double bit_pct_vanesa = (bit_diff_vanesa * 100.0) / (h1_vanesa.length() * 8);
            double hex_pct_vanesa = (hex_diff_vanesa * 100.0) / h1_vanesa.length();
            
            local_vanesa.bit_min = min(local_vanesa.bit_min, bit_pct_vanesa);
            local_vanesa.bit_max = max(local_vanesa.bit_max, bit_pct_vanesa);
            local_vanesa.bit_sum += bit_pct_vanesa;
            local_vanesa.hex_min = min(local_vanesa.hex_min, hex_pct_vanesa);
            local_vanesa.hex_max = max(local_vanesa.hex_max, hex_pct_vanesa);
            local_vanesa.hex_sum += hex_pct_vanesa;
        }
        
        #pragma omp critical
        {
            own_stats.bit_min = min(own_stats.bit_min, local_own.bit_min);
            own_stats.bit_max = max(own_stats.bit_max, local_own.bit_max);
            own_stats.bit_sum += local_own.bit_sum;
            own_stats.hex_min = min(own_stats.hex_min, local_own.hex_min);
            own_stats.hex_max = max(own_stats.hex_max, local_own.hex_max);
            own_stats.hex_sum += local_own.hex_sum;
            
            sha_stats.bit_min = min(sha_stats.bit_min, local_sha.bit_min);
            sha_stats.bit_max = max(sha_stats.bit_max, local_sha.bit_max);
            sha_stats.bit_sum += local_sha.bit_sum;
            sha_stats.hex_min = min(sha_stats.hex_min, local_sha.hex_min);
            sha_stats.hex_max = max(sha_stats.hex_max, local_sha.hex_max);
            sha_stats.hex_sum += local_sha.hex_sum;
            
            md5_stats.bit_min = min(md5_stats.bit_min, local_md5.bit_min);
            md5_stats.bit_max = max(md5_stats.bit_max, local_md5.bit_max);
            md5_stats.bit_sum += local_md5.bit_sum;
            md5_stats.hex_min = min(md5_stats.hex_min, local_md5.hex_min);
            md5_stats.hex_max = max(md5_stats.hex_max, local_md5.hex_max);
            md5_stats.hex_sum += local_md5.hex_sum;
            
            vanesa_stats.bit_min = min(vanesa_stats.bit_min, local_vanesa.bit_min);
            vanesa_stats.bit_max = max(vanesa_stats.bit_max, local_vanesa.bit_max);
            vanesa_stats.bit_sum += local_vanesa.bit_sum;
            vanesa_stats.hex_min = min(vanesa_stats.hex_min, local_vanesa.hex_min);
            vanesa_stats.hex_max = max(vanesa_stats.hex_max, local_vanesa.hex_max);
            vanesa_stats.hex_sum += local_vanesa.hex_sum;
        }
    }
    
    cout << "### Lavinos efektas - BIT skirtumas\n\n";
    cout << "| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |\n";
    cout << "|:---:|:---:|:---:|:---:|\n";
    cout << "| Neda (v0.1final) | " << fixed << setprecision(2) << own_stats.bit_max << "% | " 
         << own_stats.bit_min << "% | " << (own_stats.bit_sum / NUM_PAIRS) << "% |\n";
    cout << "| SHA-256 | " << sha_stats.bit_max << "% | " 
         << sha_stats.bit_min << "% | " << (sha_stats.bit_sum / NUM_PAIRS) << "% |\n";
    cout << "| MD5 | " << md5_stats.bit_max << "% | " 
         << md5_stats.bit_min << "% | " << (md5_stats.bit_sum / NUM_PAIRS) << "% |\n";
    cout << "| Vanesė | " << vanesa_stats.bit_max << "% | " 
         << vanesa_stats.bit_min << "% | " << (vanesa_stats.bit_sum / NUM_PAIRS) << "% |\n";
    
    cout << "\n### Lavanos efektas - HEX skirtumas\n\n";
    cout << "| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |\n";
    cout << "|:---:|:---:|:---:|:---:|\n";
    cout << "| Neda (v0.1final) | " << own_stats.hex_max << "% | " 
         << own_stats.hex_min << "% | " << (own_stats.hex_sum / NUM_PAIRS) << "% |\n";
    cout << "| SHA-256 | " << sha_stats.hex_max << "% | " 
         << sha_stats.hex_min << "% | " << (sha_stats.hex_sum / NUM_PAIRS) << "% |\n";
    cout << "| MD5 | " << md5_stats.hex_max << "% | " 
         << md5_stats.hex_min << "% | " << (md5_stats.hex_sum / NUM_PAIRS) << "% |\n";
    cout << "| Vanesė | " << vanesa_stats.hex_max << "% | " 
         << vanesa_stats.hex_min << "% | " << (vanesa_stats.hex_sum / NUM_PAIRS) << "% |\n";
}

int main() {
    cout << "=== PALYGINIMAS SU STANDARTINIAIS HASH ALGORITMAIS ===\n";
    cout << "Testuojame: Neda (v0.1final), SHA-256, MD5, Vanesė\n";
    cout << "Naudojami 24 thread'ai palyginimui\n\n";
    
    test_performance_comparison();
    test_collision_resistance();
    test_avalanche_effect();
    
    cout << "\n=== TESTAI BAIGTI ===\n";
    
    return 0;
}
