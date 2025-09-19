#include "mylib.h"
#include "functions.h"
#include "tests.h"

int main() {
    string user_input = readinput();
    string hashe = generate_hashe(user_input);
    cout << "Sugeneruotas 64 bitų hash: " << hashe << endl;
    //run_tests(user_input, hashe);
    //store_hashe(hashe);

    return 0;
}