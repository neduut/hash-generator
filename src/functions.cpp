#include "functions.h"

std::random_device rd;
std::mt19937 mt(rd());

string readinput() {
    int choice;
    vector<string> user_input; 

    cout << "Įvesti ranka (1) ar skaityti duomenis iš failo (2)? ";
    cin >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (choice == 1) {
        string input;
        cout << "Įveskite duomenis: ";
        getline(cin, input);
        user_input.push_back(input);
    } 
    else if (choice == 2) {
        ifstream infile("input.txt");
        if (!infile) {
            cerr << "Klaida atidarant failą!" << endl;
            return "";
        }
        string line;
        while (getline(infile, line)) {
            user_input.push_back(line);
        }
        infile.close();
    } 
    else {
        cout << "Neteisingas pasirinkimas!" << endl;
        return "";
    }

    // sujungiu eilutes i viena string
    string result;
    for (const auto& s : user_input) {
        result += s;
        result += " "; 
    }

    return result;
}

string generate_hashe(string user_input){
    string hashe;

    //pavercia i (hex) bitus (vienas ilgas string)
    for (char c : user_input) {
        hashe += to_string(static_cast<int>(c));
    }

    //suskirsto i blokus po 4 bitus
    vector<string> blocks;
    for (size_t i = 0; i < hashe.length(); i += 4) {
        if(i + 4 > hashe.length()) {
            hashe += string((i + 4) - hashe.length(), '0'); // prideda nulius iki kad gautus pilnas blokas jei neuztenka
        }
        blocks.push_back(hashe.substr(i, 4));
    }

    return hashe;
}

//print_hashe();
//run_tests();
//store_hashe();