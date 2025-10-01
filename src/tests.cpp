#include "mylib.h"
#include "tests.h"
#include "ownHash.h"
#include "constants.h"

#include <filesystem>
#include <unordered_set>
#include <utility>     // std::pair
#include <cstdlib>     // getenv

// Temporary stub function to replace generateMD5 calls
std::string generateMD5(const std::string& input) {
    return generate_hash(input);
}
#include <cstring>     // std::strchr
#include <omp.h>       // OpenMP for parallel tests

#ifdef _WIN32
#include <windows.h>
#endif

namespace fs = std::filesystem;

// ======== Gražesni konsolės pranešimai / spalvos / runneris ========
namespace ui {
    // ANSI spalvos (jei nepalaikoma – bus tiesiog tekstas)
    static constexpr const char* RESET = "\033[0m";
    static constexpr const char* BOLD  = "\033[1m";
    static constexpr const char* DIM   = "\033[2m";
    static constexpr const char* CYAN  = "\033[36m";
    static constexpr const char* GREEN = "\033[32m";
    static constexpr const char* RED   = "\033[31m";
    static constexpr const char* YELL  = "\033[33m";

    inline void enable_ansi_if_needed() {
    #ifdef _WIN32
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut != INVALID_HANDLE_VALUE) {
            DWORD mode = 0;
            if (GetConsoleMode(hOut, &mode)) {
                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hOut, mode);
            }
        }
    #endif
    }

    inline void print_run(const std::string& name) {
        std::cout << BOLD << CYAN << "[ RUN ]" << RESET << " " << name << "...\n";
    }
    inline void print_ok(const std::string& name, long long ms) {
        std::cout << BOLD << GREEN << "[ OK ]" << RESET << " " << name
                  << "  (" << ms << " ms)\n";
    }
    inline void print_fail(const std::string& name, const std::string& why, long long ms) {
        std::cerr << BOLD << RED << "[ FAILED]" << RESET << " " << name
                  << " — " << why << "  (" << ms << " ms)\n";
    }
    inline void print_warn(const std::string& msg) {
        std::cout << BOLD << YELL << "[ WARN ]" << RESET << " " << msg << "\n";
    }
}

// Dabartinio testo pavadinimas (ASSERT makro sužinos, kas krito)
static thread_local const char* g_current_test_name = "";

// Set optimal thread count for maximum performance
static void set_optimal_threads() {
    omp_set_num_threads(24);
}

// --------- bendra konfigūracija / utils ----------

static inline string ANALYSIS_DIR() { return "analysis"; }
static inline string REPORT_PATH()  { return ANALYSIS_DIR() + string("/tests_report.txt"); }

static int env_int(const char* name, int defv) {
    const char* v = std::getenv(name);
    if (!v) return defv;
    try { return std::max(1, stoi(string(v))); } catch (...) { return defv; }
}

static void ensure_analysis_dir() {
    std::error_code ec;
    fs::create_directories(ANALYSIS_DIR(), ec);
}

static bool ensure_dir(const string& dir) {
    std::error_code ec;
    fs::create_directories(dir, ec);
    return !ec;
}

static string now_ts() {
    using clk = std::chrono::system_clock;
    auto t = clk::to_time_t(clk::now());
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return buf;
}

struct Logger {
    ofstream out;
    explicit Logger(const string& path) { out.open(path, std::ios::app); }
    template<class T>
    Logger& operator<<(const T& v) { if (out) out << v; return *this; }
    void flush() { if (out) out.flush(); }
};

// Globalus laiko kintamasis ASSERT makro
static thread_local std::chrono::high_resolution_clock::time_point g_test_start_time;

#define ASSERT_TRUE_RET(expr, msg) do {                                      \
    if (!(expr)) {                                                            \
        auto __t1 = std::chrono::high_resolution_clock::now();                \
        long long __ms = std::chrono::duration_cast<std::chrono::milliseconds>(\
            __t1 - g_test_start_time).count();                                \
        ui::print_fail(g_current_test_name ? g_current_test_name : "testas",  \
                       (msg), __ms);                                          \
        if (g_log) (*g_log) << "FAIL " << (g_current_test_name ? g_current_test_name : "testas") \
                            << " - " << (msg) << " (" << __ms << "ms)\n";     \
        return false;                                                         \
    }                                                                         \
} while(0)

// --------- failų utilitai (punktas #1) ----------

static bool ensure_file_with_contents(const string& path, const string& contents) {
    ofstream f(path, std::ios::binary);
    if (!f) return false;
    f << contents;
    return true;
}

static bool read_all_lines(const string& path, vector<string>& out) {
    ifstream f(path);
    if (!f) return false;
    string line;
    while (getline(f, line)) out.push_back(line);
    return true;
}

static string slurp_file_text(const string& path) {
    ifstream f(path, std::ios::binary);
    if (!f) return {};
    stringstream ss; ss << f.rdbuf();
    return ss.str();
}

static string random_string(std::mt19937_64& rng, size_t n) {
    static const string alphabet =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        " !@#$%^&*()_+-=[]{}|;':,./<>?\n\t";
    uniform_int_distribution<size_t> dist(0, alphabet.size() - 1);
    string s; s.reserve(n);
    for (size_t i=0;i<n;++i) s.push_back(alphabet[dist(rng)]);
    return s;
}

static bool write_random_file(const string& path, size_t n_chars) {
    std::mt19937_64 rng(0xBADC0FFEEULL);
    string s = random_string(rng, n_chars);
    return ensure_file_with_contents(path, s);
}

static bool write_pair_one_diff(const string& base_path, const string& var_path, size_t n_chars) {
    if (n_chars == 0) return false;
    std::mt19937_64 rng(0xFEEDFACEULL);
    string a = random_string(rng, n_chars);
    string b = a;
    size_t mid = n_chars / 2;
    // pakeičiam vidurinį simbolį į kitą
    for (char c = 32; c < 127; ++c) { if (c != a[mid]) { b[mid] = c; break; } }
    bool ok1 = ensure_file_with_contents(base_path, a);
    bool ok2 = ensure_file_with_contents(var_path,  b);
    return ok1 && ok2;
}

static inline int base62_index(char c) {
    const char* p = std::strchr(BASE62, c);
    if (!p) return -1;
    return int(p - BASE62); // 0..61
}

static int popcnt_u32(unsigned v) {
#if defined(__GNUG__) || defined(__clang__)
    return __builtin_popcount(v);
#else
    int c=0; while(v){ v&=v-1; ++c; } return c;
#endif
}

static double hamming_bits_base62(const string& h1, const string& h2) {
    size_t n = std::min(h1.size(), h2.size());
    size_t total_bits = n * 6;
    size_t diff_bits = 0;
    for (size_t i=0;i<n;++i) {
        int a = base62_index(h1[i]);
        int b = base62_index(h2[i]);
        if (a < 0 || b < 0) continue;
        unsigned x = unsigned(a ^ b);
        diff_bits += popcnt_u32(x);
    }
    if (total_bits == 0) return 0.0;
    return 100.0 * double(diff_bits) / double(total_bits);
}

static double hex_level_diff_percent(const string& h1, const string& h2) {
    auto to_hex2 = [](const string& h) {
        string hex; hex.reserve(h.size()*2);
        static const char* HEX = "0123456789ABCDEF";
        for (char c : h) {
            int v = base62_index(c);
            if (v < 0) v = 0;
            hex.push_back(HEX[(v >> 4) & 0xF]);
            hex.push_back(HEX[v & 0xF]);
        }
        return hex;
    };
    string x = to_hex2(h1), y = to_hex2(h2);
    size_t n = std::min(x.size(), y.size());
    size_t diff = 0;
    for (size_t i=0;i<n;++i) if (x[i] != y[i]) ++diff;
    if (n == 0) return 0.0;
    return 100.0 * double(diff) / double(n);
}

// --------- testų implementacijos ----------

static Logger* g_log = nullptr;

// Universalus runneris: iškviečia testą, pamatuoja laiką, atspausdina statusą
template <class F>
static bool run_test(const std::string& name, F&& fn) {
    g_current_test_name = name.c_str();
    ui::print_run(name);
    g_test_start_time = std::chrono::high_resolution_clock::now();
    auto __t0 = g_test_start_time;
    
    bool ok = fn();

    auto __t1 = std::chrono::high_resolution_clock::now();
    long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(__t1 - __t0).count();

    if (ok) {
        ui::print_ok(name, ms);
        if (g_log) (*g_log) << "OK " << name << " (" << ms << "ms)\n";
    } else {
        // jei pats testas jau atspausdino FAIL – čia nebedubliuojam priežasties
        if (g_log) (*g_log) << "FAIL " << name << " (" << ms << "ms)\n";
    }
    return ok;
}

static bool test_1_output_size() {
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 1 - išvedimo dydis\n";

    // vidiniai pavyzdžiai
    vector<string> samples = {
        "", "a", "Labas", string(10, 'x'), string(1234, 'y'), "ąčęėįšųūž"
    };
    for (auto& s : samples) {
        string h = generateMD5(s);
        ASSERT_TRUE_RET(h.size() == 64u, "hash dydis turi būti 64");
        for (char c : h) ASSERT_TRUE_RET(std::strchr(BASE62, c) != nullptr, "hash simboliai turi būti iš BASE62");
    }

    // failai pagal #1 reikalavimą
    vector<string> files_to_check = {
        "files/one_a.txt",
        "files/one_b.txt",
        "files/empty.txt",
        "files/random_2000_A.txt",
        "files/random_2000_B.txt",
        "files/random_2000_M_base.txt",
        "files/random_2000_M_variant.txt"
    };
    for (auto& p : files_to_check) {
        string s = slurp_file_text(p);
        string h = generateMD5(s);
        ASSERT_TRUE_RET(h.size() == 64u, string("hash dydis turi būti 64 (") + p + ")");
        for (char c : h) ASSERT_TRUE_RET(std::strchr(BASE62, c) != nullptr, string("hash simboliai turi būti iš BASE62 (") + p + ")");
    }

    if (g_log) (*g_log) << "OK 1 - išvedimo dydis\n";
    return true;
}

static bool test_2_determinism() {
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 2 - deterministiškumas\n";

    // su atsitiktiniais string’ais
    std::mt19937_64 rng(1234567);
    for (int i=0;i<10;++i) {
        string s = random_string(rng, 200);
        string h1 = generateMD5(s);
        string h2 = generateMD5(s);
        ASSERT_TRUE_RET(h1 == h2, "tas pats įvedimas turi duoti tą patį hash (string)");
    }

    // su failais (konkrečiai #1 reikalavimo rinkinys)
    vector<string> files_to_check = {
        "files/one_a.txt",
        "files/one_b.txt",
        "files/empty.txt",
        "files/random_2000_A.txt",
        "files/random_2000_B.txt",
        "files/random_2000_M_base.txt",
        "files/random_2000_M_variant.txt"
    };
    for (auto& p : files_to_check) {
        string s = slurp_file_text(p);
        string h1 = generateMD5(s);
        string h2 = generateMD5(s);
        ASSERT_TRUE_RET(h1 == h2, string("tas pats FAILAS turi duoti tą patį hash (") + p + ")");
    }

    if (g_log) (*g_log) << "OK 2 - deterministiškumas\n";
    return true;
}

static bool test_4_performance() {
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 4 - efektyvumas\n";

    const string path = string("files/") + "konstitucija.txt";
    vector<string> lines;
    if (!read_all_lines(path, lines) || lines.empty()) {
        cout << " [WARN] Neradau konstitucija.txt - praleista\n";
        if (g_log) (*g_log) << "WARN 4 - nėra konstitucija.txt, praleista\n";
        return true;
    }

    ensure_analysis_dir();

    // Write performance data to the main report file
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "[PERFORMANCE] " << now_ts() << "\n";
    rep << "lines,avg_ms\n";

    vector<size_t> sizes;
    for (size_t n=1; n<=lines.size(); n<<=1) sizes.push_back(n);
    const int REPEATS = 5;

    for (size_t n : sizes) {
        long long acc_ms = 0;
        string input_cat;
        for (int r=0;r<REPEATS;++r) {
            input_cat.clear(); input_cat.reserve(64*n);
            for (size_t i=0;i<n && i<lines.size(); ++i) {
                if (i) input_cat.push_back('\n');
                input_cat += lines[i];
            }
            auto t0 = std::chrono::high_resolution_clock::now();
            volatile string h = generateMD5(input_cat);
            (void)h;
            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - t0).count();
            acc_ms += dt;
        }
        double avg = double(acc_ms) / REPEATS;
        rep << n << "," << fixed << setprecision(2) << avg << "\n";
    }

    if (g_log) (*g_log) << "OK 4 - efektyvumas\n";
    return true;
}

static bool test_5_collisions() {
    const int NUM_PAIRS = env_int("TEST_NUM_PAIRS", 100000);
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 5 - kolizijos (" << NUM_PAIRS << " porų), threads=" << omp_get_max_threads() << "\n";

    vector<size_t> L = {10, 100, 500, 1000};
    std::mt19937_64 rng(777);

    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[COLLISIONS] " << now_ts() << "  NUM_PAIRS=" << NUM_PAIRS << " THREADS=" << omp_get_max_threads() << "\n";

    auto test_start = std::chrono::high_resolution_clock::now();

    for (size_t len : L) {
        auto len_start = std::chrono::high_resolution_clock::now();
        size_t collisions = 0;
        
        // Generate all test data first (thread-safe)
        vector<std::pair<string, string>> test_pairs;
        test_pairs.reserve(NUM_PAIRS);
        for (int i = 0; i < NUM_PAIRS; ++i) {
            test_pairs.emplace_back(random_string(rng, len), random_string(rng, len));
        }
        
        auto compute_start = std::chrono::high_resolution_clock::now();
        
        // Parallel collision detection
        #pragma omp parallel for reduction(+:collisions)
        for (int i = 0; i < NUM_PAIRS; ++i) {
            if (generateMD5(test_pairs[i].first) == generateMD5(test_pairs[i].second)) {
                ++collisions;
            }
        }
        
        auto compute_end = std::chrono::high_resolution_clock::now();
        auto compute_ms = std::chrono::duration_cast<std::chrono::milliseconds>(compute_end - compute_start).count();
        auto len_ms = std::chrono::duration_cast<std::chrono::milliseconds>(compute_end - len_start).count();
        
        double rate = 100.0 * collisions / double(NUM_PAIRS);
        rep  << "L=" << len << ", collisions=" << collisions << ", rate=" << fixed << setprecision(6) << rate 
             << "%, compute_time=" << compute_ms << "ms, total_time=" << len_ms << "ms\n";
    }

    auto test_end = std::chrono::high_resolution_clock::now();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(test_end - test_start).count();
    
    rep << "TOTAL_TIME=" << total_ms << "ms, THREADS=" << omp_get_max_threads() << "\n";

    if (g_log) (*g_log) << "OK 5 - kolizijos\n";
    return true;
}

static bool test_6_avalanche() {
    const int NUM_PAIRS = env_int("TEST_NUM_PAIRS", 100000);
    const size_t LEN = 200;
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 6 - lavina (" << NUM_PAIRS << " porų), threads=" << omp_get_max_threads() << "\n";

    auto test_start = std::chrono::high_resolution_clock::now();

    std::mt19937_64 rng(12345);
    uniform_int_distribution<int> posd(0, int(LEN-1));
    uniform_int_distribution<int> chard(32, 126);

    // Generate all test data first (thread-safe)
    auto data_start = std::chrono::high_resolution_clock::now();
    vector<std::pair<string, string>> test_pairs;
    test_pairs.reserve(NUM_PAIRS);
    for (int i = 0; i < NUM_PAIRS; ++i) {
        string a = random_string(rng, LEN);
        string b = a;
        int pos = posd(rng);
        char newc;
        do { newc = char(chard(rng)); } while (newc == a[pos]);
        b[pos] = newc;
        test_pairs.emplace_back(move(a), move(b));
    }
    auto data_end = std::chrono::high_resolution_clock::now();
    auto data_ms = std::chrono::duration_cast<std::chrono::milliseconds>(data_end - data_start).count();

    double min_bits = 100.0, max_bits = 0.0, sum_bits = 0.0;
    double min_hex  = 100.0, max_hex  = 0.0, sum_hex  = 0.0;

    auto compute_start = std::chrono::high_resolution_clock::now();

    // Parallel avalanche computation with thread-local min/max tracking
    #pragma omp parallel
    {
        double local_min_bits = 100.0, local_max_bits = 0.0, local_sum_bits = 0.0;
        double local_min_hex  = 100.0, local_max_hex  = 0.0, local_sum_hex  = 0.0;
        
        #pragma omp for
        for (int i = 0; i < NUM_PAIRS; ++i) {
            string h1 = generateMD5(test_pairs[i].first);
            string h2 = generateMD5(test_pairs[i].second);

            double db = hamming_bits_base62(h1, h2);
            double dh = hex_level_diff_percent(h1, h2);

            local_min_bits = std::min(local_min_bits, db);
            local_max_bits = std::max(local_max_bits, db);
            local_sum_bits += db;

            local_min_hex = std::min(local_min_hex, dh);
            local_max_hex = std::max(local_max_hex, dh);
            local_sum_hex += dh;

            // Progress removed for cleaner console output
        }
        
        // Combine results from all threads
        #pragma omp critical
        {
            min_bits = std::min(min_bits, local_min_bits);
            max_bits = std::max(max_bits, local_max_bits);
            sum_bits += local_sum_bits;
            
            min_hex = std::min(min_hex, local_min_hex);
            max_hex = std::max(max_hex, local_max_hex);
            sum_hex += local_sum_hex;
        }
    }

    auto compute_end = std::chrono::high_resolution_clock::now();
    auto compute_ms = std::chrono::duration_cast<std::chrono::milliseconds>(compute_end - compute_start).count();

    double avg_bits = sum_bits / NUM_PAIRS;
    double avg_hex  = sum_hex  / NUM_PAIRS;

    auto test_end = std::chrono::high_resolution_clock::now();
    auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(test_end - test_start).count();

    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[AVALANCHE] " << now_ts() << "  NUM_PAIRS=" << NUM_PAIRS << " LEN=" << LEN << " THREADS=" << omp_get_max_threads() << "\n";
    rep << "TIMING: data_gen=" << data_ms << "ms, compute=" << compute_ms << "ms, total=" << total_ms << "ms\n";
    rep << "THROUGHPUT: " << fixed << setprecision(1) << (double(NUM_PAIRS * 2) / compute_ms * 1000.0) << " hash/s\n";
    rep << "bits%  min=" << fixed << setprecision(3) << min_bits
        << "  max=" << max_bits << "  avg=" << avg_bits << "\n";
    rep << "hex%   min=" << min_hex
        << "  max=" << max_hex  << "  avg=" << avg_hex  << "\n";

    if (g_log) (*g_log) << "OK 6 - lavina\n";
    return true;
}

static bool test_7_hiding() {
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 7 - hiding\n";

    const string input = "Tai yra testinis tekstas su diakritiniais ženklais ąčęėįšųūž.";
    vector<string> salts = {
        "salt1", "salt1", "salt2", "ilgesnis_saltas_123", "!" , string(32,'X')
    };

    std::unordered_set<string> hashs;
    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[HIDING] " << now_ts() << "\n";

    for (auto& salt : salts) {
        string h = generateMD5(input + salt);
        hashs.insert(h);
        rep  << "salt=\"" << salt << "\" -> " << h << "\n";
    }
    bool ok = (hashs.size() == size_t(5)); // 5 unikalūs (nes "salt1" du kartus)
    if (!ok) {
        ui::print_fail("hiding testas", "tikėtasi 5 unikalių hash'ų", 0);
        if (g_log) (*g_log) << "FAIL 7 - hiding\n";
        return false;
    }

    if (g_log) (*g_log) << "OK 7 - hiding\n";
    return true;
}

// --------- meniu ir paleidimas iš main() ----------

static void print_menu() {
    cout <<
        "\nPasirink testą (optimizuota 24 threads):\n"
        "0 - visi testai\n"
        "1 - išvedimo dydis\n"
        "2 - deterministiškumas\n"
        "4 - efektyvumas\n"
        "5 - kolizijų paieška (paralelizuota)\n"
        "6 - lavinos efektas (paralelizuota)\n"
        "7 - negrįžtamumo demonstracija\n"
        "q - grįžti\n> ";
}

// Sesijos sargas: pabaigoje parašys footerį su separatoriumi
struct SessionGuard {
    Logger* log;
    ~SessionGuard() {
        if (log) {
            (*log) << "========================================\n";
            log->flush();
        }
    }
};

bool run_tests_menu() {
    // Enable ANSI colors in Windows console
    ui::enable_ansi_if_needed();
    
    // Set optimal thread count for maximum performance
    set_optimal_threads();
    
    ensure_dir("files");
    ensure_analysis_dir();

    // #1: paruošiam reikalaujamus failus
    (void)ensure_file_with_contents("files/one_a.txt", "a\n");
    (void)ensure_file_with_contents("files/one_b.txt", "b\n");
    (void)ensure_file_with_contents("files/empty.txt", "");
    (void)write_random_file("files/random_2000_A.txt", 2000);
    (void)write_random_file("files/random_2000_B.txt", 2000);
    (void)write_pair_one_diff("files/random_2000_M_base.txt",
                              "files/random_2000_M_variant.txt", 2000);

    Logger filelog(REPORT_PATH());
    g_log = &filelog;

    // --- sesijos headeris + separatorius ---
    (*g_log) << "========================================\n";
    (*g_log) << "TEST SESSION START @ " << now_ts() << "\n";
    g_log->flush();

    // auto-footeris pabaigoje
    SessionGuard guard{ g_log };

    string choice;
    while (true) {
        print_menu();
        if (!getline(cin, choice)) {
            cerr << DATA_READ_ERROR << endl;
            (*g_log) << "ABORT: input error\n";
            return false;
        }
        choice.erase(remove(choice.begin(), choice.end(), ' '), choice.end());
        if (choice == "q" || choice == "Q") {
            cout << "Išeinama iš tyrimų meniu.\n";
            g_log->flush();
            return true;
        }

        bool ok = true;
        if (choice == "0") {
            ok &= run_test("išvedimo dydis", test_1_output_size);
            ok &= run_test("deterministiškumas", test_2_determinism);
            ok &= run_test("efektyvumas", test_4_performance);
            ok &= run_test("kolizijos", test_5_collisions);
            ok &= run_test("lavinos efektas", test_6_avalanche);
            ok &= run_test("hiding", test_7_hiding);
        } else if (choice == "1") {
            ok = run_test("išvedimo dydis", test_1_output_size);
        } else if (choice == "2") {
            ok = run_test("deterministiškumas", test_2_determinism);
        } else if (choice == "4") {
            ok = run_test("efektyvumas", test_4_performance);
        } else if (choice == "5") {
            ok = run_test("kolizijos", test_5_collisions);
        } else if (choice == "6") {
            ok = run_test("lavinos efektas", test_6_avalanche);
        } else if (choice == "7") {
            ok = run_test("hiding", test_7_hiding);
        } else {
            cout << "Neteisingas pasirinkimas.\n";
            continue;
        }

        if (ok) {
            ui::print_ok("Visi testai", 0);
            cout << "Detalūs rezultatai įrašyti į " << REPORT_PATH() << "\n";
            (*g_log) << "SUMMARY: OK\n";
        } else {
            ui::print_fail("Testai", "žiūrėk aukščiau", 0);
            cout << "Rezultatai įrašyti į " << REPORT_PATH() << "\n";
            (*g_log) << "SUMMARY: FAIL\n";
        }
        g_log->flush();
    }
}
