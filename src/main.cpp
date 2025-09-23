#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include "constants.h"
#include "ui.h"
#include "tests.h"
#include "mylib.h"

using namespace std;

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    while (true) {
        cout << MENU_TEXT;
        string choice;
        if (!getline(cin, choice)) {
            cerr << DATA_READ_ERROR << endl;
            return 1;
        }
        choice.erase(remove(choice.begin(), choice.end(), ' '), choice.end());

        if (choice == "1") {
            run_hash_flow();
        } else if (choice == "2") {
            bool ok = run_tests_menu();
            if (!ok) cerr << TESTS_FAILED_NOTE;
            cout << RETURN_TO_MENU;
        } else if (choice == "0") {
            cout << PROGRAM_FINISHED;
            return 0;
        } else {
            cout << TRY_AGAIN_ << endl;
        }
    }
}
