#include "mylib.h"
#include "functions.h"
#include "tests.h"

int main() {
    string user_input = readinput();
    string hashe = generate_hashe(user_input);
    print_hashe(hashe);
    run_tests(user_input, hashe);
    store_hashe(hashe);

    return 0;
}