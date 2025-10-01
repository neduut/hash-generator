#include "ui.h"
#include "constants.h"
#include "ownHash.h"

using namespace std;

static string readinput() {
    try {
        cout << ENTER_INPUT_TYPE;
        string choice_s;
        if (!getline(cin, choice_s)) throw runtime_error(INVALID_CHOICE);
        choice_s.erase(remove(choice_s.begin(), choice_s.end(), ' '), choice_s.end());

        int choice = (choice_s == "1" ? 1 : (choice_s == "2" ? 2 : -1));
        if (choice == -1) throw runtime_error(INVALID_CHOICE);

        vector<string> lines;
        if (choice == 1) {
            cout << ENTER_DATA;
            string s;
            if (!getline(cin, s)) throw runtime_error(DATA_READ_ERROR);
            lines.push_back(s);
        } else {
            cout << ENTER_FILE_NAME;
            string file_choice_s;
            if (!getline(cin, file_choice_s)) throw runtime_error(INVALID_CHOICE);
            file_choice_s.erase(remove(file_choice_s.begin(), file_choice_s.end(), ' '), file_choice_s.end());

            if (file_choice_s == "1") file_choice_s = "a.txt";
            else if (file_choice_s == "2") file_choice_s = "b.txt";
            else if (file_choice_s == "3") file_choice_s = "empty.txt";
            else if (file_choice_s == "4") file_choice_s = "random_2000_A.txt";
            else if (file_choice_s == "5") file_choice_s = "random_2000_B.txt";
            else if (file_choice_s == "6") file_choice_s = "random_2000_M_base.txt";
            else if (file_choice_s == "7") file_choice_s = "random_2000_M_variant.txt";
            else if (file_choice_s == "8") file_choice_s = "konstitucija.txt";
            else throw runtime_error(INVALID_CHOICE);

            ifstream infile(string("files/") + file_choice_s);
            if (!infile) throw runtime_error(FILE_OPEN_ERROR + file_choice_s + ".");
            string line;
            while (getline(infile, line)) lines.push_back(line);
        }

        string result;
        for (size_t i = 0; i < lines.size(); ++i) {
            result += lines[i];
            if (i + 1 < lines.size()) result += ' ';
        }
        return result;
    } catch (const exception& e) {
        throw runtime_error(string("Klaida readinput: ") + e.what());
    }
}

void run_hash_flow() {
    while (true) {
        try {
            string user_input = readinput();
            string hash = generate_hash(user_input);
            cout << "Sugeneruotas 64 simbolių hash: " << hash << endl;
        } catch (const exception& e) {
            cerr << e.what() << endl;
            cout << TRY_AGAIN_ << endl;
            continue;
        }

        while (true) {
            cout << GENERATE_HASH_PROMPT;
            string ans_s;
            if (!getline(cin, ans_s)) {
                cerr << DATA_READ_ERROR << endl;
                return;
            }
            ans_s.erase(remove(ans_s.begin(), ans_s.end(), ' '), ans_s.end());

            if (ans_s == "1") {
                break;               
            } else if (ans_s == "0") {
                cout << RETURN_TO_MENU << endl;
                return;               
            } else {
                cout << TRY_AGAIN_ << endl;
            }
        }
    }
}
