#ifdef _WIN32
#include <windows.h>
#endif

#include "mylib.h"
#include "functions.h"
#include "tests.h"
#include "constants.h"

int main() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8); 
        SetConsoleCP(CP_UTF8);       
    #endif

    while (true) {
        try {
            string user_input = readinput();
            string hashe = generate_hashe(user_input);
            cout << "Sugeneruotas 64 simbolių hash: " << hashe << endl;
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
            cout << TRY_AGAIN_ << endl;
            continue; 
        }

        while (true) {
            cout << GENERATE_HASH_PROMPT;
            string ans_s;
            if (!std::getline(cin, ans_s)) {
                cerr << DATA_READ_ERROR << endl;
                return 1;
            }
            // ignoruoju tarpus vartotojo ivedime
            ans_s.erase(std::remove(ans_s.begin(), ans_s.end(), ' '), ans_s.end());

            if (ans_s == "1") {
                break;          
            } else if (ans_s == "0") {
                cout << "Programa baigta." << endl;
                return 0;
            }
        }
    }
}
