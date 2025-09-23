#ifdef _WIN32
#include <windows.h>
#endif

#include "mylib.h"
#include "functions.h"
#include "tests.h"

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
            cout << "Bandykite dar kartą." << endl;
            continue; 
        }

        while (true) {
            cout << "Generuoti dar vieną hash (1) ar užbaigti programą (0)? ";
            string ans_s;
            if (!std::getline(cin, ans_s)) {
                cerr << "Klaida: nepavyko perskaityti pasirinkimo." << endl;
                return 1;
            }
            // ignoruoju tarpus vartotojo ivedime
            ans_s.erase(std::remove(ans_s.begin(), ans_s.end(), ' '), ans_s.end());

            if (ans_s == "1") {
                break;          
            } else if (ans_s == "0") {
                cout << "Programa baigta." << endl;
                return 0;
            } else {
                cout << "Įveskite 1 arba 0." << endl;
            }
        }
    }
}
