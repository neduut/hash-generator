#include "mylib.h"
#include "functions.h"
#include "tests.h"

int main() {
    while (true) {
        try {
            string user_input = readinput();
            string hashe = generate_hashe(user_input);
            cout << "Sugeneruotas 64 simboliu hash: " << hashe << endl;
            break; // jei ivestis gera
        } catch (const std::exception& e) {
            cerr << e.what() << endl;
            cout << "Bandykite dar kartą.\n";
        }
    }
    return 0;
}
