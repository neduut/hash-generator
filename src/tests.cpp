#include "mylib.h"
#include "tests.h"
#include "functions.h"
#include "constants.h"

#include <filesystem>
#include <unordered_set>
#include <cstdlib> // getenv
#include <cstring> // std::strchr

namespace fs = std::filesystem;

// --------- bendra konfigūracija / utils ----------

static inline string ANALYSIS_DIR() { return "analysis"; }
static inline string REPORT_PATH()  { return ANALYSIS_DIR() + string("/tests_report.txt"); }
static inline string PERF_CSV()     { return ANALYSIS_DIR() + string("/perf.csv"); }

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

#define ASSERT_TRUE_RET(expr, msg) do { \
    if (!(expr)) { \
        cerr << "[FAIL] " << msg << "\n"; \
        if (g_log) (*g_log) << "[FAIL] " << msg << "\n"; \
        return false; \
    } \
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

static bool test_1_output_size() {
    cout << "[RUN] 1 - išvedimo dydis ...\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 1 - išvedimo dydis\n";

    // vidiniai pavyzdžiai
    vector<string> samples = {
        "", "a", "Labas", string(10, 'x'), string(1234, 'y'), "ąčęėįšųūž"
    };
    for (auto& s : samples) {
        string h = generate_hashe(s);
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
        string h = generate_hashe(s);
        ASSERT_TRUE_RET(h.size() == 64u, string("hash dydis turi būti 64 (") + p + ")");
        for (char c : h) ASSERT_TRUE_RET(std::strchr(BASE62, c) != nullptr, string("hash simboliai turi būti iš BASE62 (") + p + ")");
    }

    cout << "OK 1 - išvedimo dydis\n";
    if (g_log) (*g_log) << "OK 1 - išvedimo dydis\n";
    return true;
}

static bool test_2_determinism() {
    cout << "[RUN] 2 - deterministiškumas ...\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 2 - deterministiškumas\n";

    // su atsitiktiniais string’ais
    std::mt19937_64 rng(1234567);
    for (int i=0;i<10;++i) {
        string s = random_string(rng, 200);
        string h1 = generate_hashe(s);
        string h2 = generate_hashe(s);
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
        string h1 = generate_hashe(s);
        string h2 = generate_hashe(s);
        ASSERT_TRUE_RET(h1 == h2, string("tas pats FAILAS turi duoti tą patį hash (") + p + ")");
    }

    cout << "OK 2 - deterministiškumas\n";
    if (g_log) (*g_log) << "OK 2 - deterministiškumas\n";
    return true;
}

static bool test_4_performance() {
    cout << "[RUN] 4 - efektyvumas ... (rašau į " << PERF_CSV() << ")\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 4 - efektyvumas\n";

    const string path = string("files/") + "konstitucija.txt";
    vector<string> lines;
    if (!read_all_lines(path, lines) || lines.empty()) {
        cout << "[WARN] Neradau files/konstitucija.txt arba jis tuščias – praleidžiu.\n";
        if (g_log) (*g_log) << "WARN 4 - nėra konstitucija.txt, praleista\n";
        return true;
    }

    ensure_analysis_dir();

    // CSV perrašomas kiekvienos sesijos metu
    ofstream csv(PERF_CSV(), std::ios::trunc);
    csv << "lines,avg_ms\n";

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
            volatile string h = generate_hashe(input_cat);
            (void)h;
            auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::high_resolution_clock::now() - t0).count();
            acc_ms += dt;
        }
        double avg = double(acc_ms) / REPEATS;
        cout << "  " << n << " eilučių -> " << fixed << setprecision(2) << avg << " ms\n";
        csv << n << "," << fixed << setprecision(2) << avg << "\n";
    }

    cout << "OK 4 - efektyvumas (CSV parašytas)\n";
    if (g_log) (*g_log) << "OK 4 - efektyvumas (CSV parašytas)\n";
    return true;
}

static bool test_5_collisions() {
    const int NUM_PAIRS = env_int("TEST_NUM_PAIRS", 100000);
    cout << "[RUN] 5 - kolizijų paieška ... (porų: " << NUM_PAIRS << " kiekvienam ilgiui)\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 5 - kolizijos (" << NUM_PAIRS << " porų)\n";

    vector<size_t> L = {10, 100, 500, 1000};
    std::mt19937_64 rng(777);

    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[COLLISIONS] " << now_ts() << "  NUM_PAIRS=" << NUM_PAIRS << "\n";

    for (size_t len : L) {
        size_t collisions = 0;
        for (int i=0;i<NUM_PAIRS;++i) {
            string s1 = random_string(rng, len);
            string s2 = random_string(rng, len);
            if (generate_hashe(s1) == generate_hashe(s2)) ++collisions;
        }
        double rate = 100.0 * collisions / double(NUM_PAIRS);
        cout << "  L=" << len << ": " << collisions << " kolizijų (" << fixed << setprecision(6) << rate << "%)\n";
        rep  << "L=" << len << ", collisions=" << collisions << ", rate=" << fixed << setprecision(6) << rate << "%\n";
    }

    cout << "OK 5 - kolizijos\n";
    if (g_log) (*g_log) << "OK 5 - kolizijos\n";
    return true;
}

static bool test_6_avalanche() {
    const int NUM_PAIRS = env_int("TEST_NUM_PAIRS", 100000);
    const size_t LEN = 200;
    cout << "[RUN] 6 - lavinos efektas ... (porų: " << NUM_PAIRS << ", ilgis=" << LEN << ")\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 6 - lavina (" << NUM_PAIRS << " porų)\n";

    std::mt19937_64 rng(12345);
    uniform_int_distribution<int> posd(0, int(LEN-1));
    uniform_int_distribution<int> chard(32, 126);

    double min_bits = 100.0, max_bits = 0.0, sum_bits = 0.0;
    double min_hex  = 100.0, max_hex  = 0.0, sum_hex  = 0.0;

    for (int i=0;i<NUM_PAIRS;++i) {
        string a = random_string(rng, LEN);
        string b = a;
        int pos = posd(rng);
        char newc;
        do { newc = char(chard(rng)); } while (newc == a[pos]);
        b[pos] = newc;

        string h1 = generate_hashe(a);
        string h2 = generate_hashe(b);

        double db = hamming_bits_base62(h1, h2);
        double dh = hex_level_diff_percent(h1, h2);

        min_bits = std::min(min_bits, db);
        max_bits = std::max(max_bits, db);
        sum_bits += db;

        min_hex = std::min(min_hex, dh);
        max_hex = std::max(max_hex, dh);
        sum_hex += dh;

        if ((i+1) % 20000 == 0) {
            cout << "  progreso: " << (i+1) << "/" << NUM_PAIRS << "\n";
        }
    }

    double avg_bits = sum_bits / NUM_PAIRS;
    double avg_hex  = sum_hex  / NUM_PAIRS;

    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[AVALANCHE] " << now_ts() << "  NUM_PAIRS=" << NUM_PAIRS << " LEN=" << LEN << "\n";
    rep << "bits%  min=" << fixed << setprecision(3) << min_bits
        << "  max=" << max_bits << "  avg=" << avg_bits << "\n";
    rep << "hex%   min=" << min_hex
        << "  max=" << max_hex  << "  avg=" << avg_hex  << "\n";

    cout << "OK 6 - lavina\n";
    if (g_log) (*g_log) << "OK 6 - lavina\n";
    return true;
}

static bool test_7_hiding() {
    cout << "[RUN] 7 - negrįžtamumo demonstracija (HASH(input + salt)) ...\n";
    if (g_log) (*g_log) << "[" << now_ts() << "] RUN 7 - hiding\n";

    const string input = "Tai yra testinis tekstas su diakritiniais ženklais ąčęėįšųūž.";
    vector<string> salts = {
        "salt1", "salt1", "salt2", "ilgesnis_saltas_123", "!" , string(32,'X')
    };

    std::unordered_set<string> hashes;
    ensure_analysis_dir();
    ofstream rep(REPORT_PATH(), std::ios::app);
    rep << "\n[HIDING] " << now_ts() << "\n";

    for (auto& salt : salts) {
        string h = generate_hashe(input + salt);
        hashes.insert(h);
        cout << "  salt=\"" << salt << "\" -> " << h << "\n";
        rep  << "salt=\"" << salt << "\" -> " << h << "\n";
    }
    bool ok = (hashes.size() == size_t(5)); // 5 unikalūs (nes "salt1" du kartus)
    if (!ok) {
        cout << "[FAIL] tikėtasi 5 unikalių hash'ų\n";
        if (g_log) (*g_log) << "FAIL 7 - hiding\n";
        return false;
    }

    cout << "OK 7 - hiding\n";
    if (g_log) (*g_log) << "OK 7 - hiding\n";
    return true;
}

// --------- meniu ir paleidimas iš main() ----------

static void print_menu() {
    cout <<
        "\nPasirink testą:\n"
        "0 - visi testai\n"
        "1 - išvedimo dydis\n"
        "2 - deterministiškumas\n"
        "4 - efektyvumas\n"
        "5 - kolizijų paieška\n"
        "6 - lavinos efektas\n"
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
            ok &= test_1_output_size();
            ok &= test_2_determinism();
            ok &= test_4_performance();
            ok &= test_5_collisions();
            ok &= test_6_avalanche();
            ok &= test_7_hiding();
        } else if (choice == "1") {
            ok = test_1_output_size();
        } else if (choice == "2") {
            ok = test_2_determinism();
        } else if (choice == "4") {
            ok = test_4_performance();
        } else if (choice == "5") {
            ok = test_5_collisions();
        } else if (choice == "6") {
            ok = test_6_avalanche();
        } else if (choice == "7") {
            ok = test_7_hiding();
        } else {
            cout << "Neteisingas pasirinkimas.\n";
            continue;
        }

        if (ok) {
            cout << "SUMMARY: OK\n";
            (*g_log) << "SUMMARY: OK\n";
        } else {
            cout << "SUMMARY: FAIL\n";
            (*g_log) << "SUMMARY: FAIL\n";
        }
        g_log->flush();
    }
}
