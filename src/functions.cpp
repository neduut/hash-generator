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
    // paverčia į skaičius ir blokus
    string hashe;
    for (char c : user_input) {
        hashe += to_string(static_cast<int>(c));
    }

    vector<string> blocks;
    for (size_t i = 0; i < hashe.length(); i += 4) {
        if(i + 4 > hashe.length()) {
            hashe += string((i + 4) - hashe.length(), '0');
        }
        blocks.push_back(hashe.substr(i, 4));
    }

    // sumaišo blokus pastoviai
    mt19937 gen(42);
    for (int i = blocks.size() - 1; i > 0; i--) {
        uniform_int_distribution<> dis(0, i);
        int j = dis(gen);
        swap(blocks[i], blocks[j]);
    }

    // sumaišo kiekvieno bloko simbolius
    for (auto& block : blocks) {
        mt19937 gen(42);
        for (int i = block.size() - 1; i > 0; i--) {
            uniform_int_distribution<> dis(0, i);
            int j = dis(gen);
            swap(block[i], block[j]);
        }
    }

    // **sulipdo visus blokus į vieną string**
    hashe.clear();
    for (const auto& block : blocks) {
        hashe += block;
    }

    // **čia sukuriamas tikras 64 bitų hash** iš sumaišyto stringo
    unsigned long long hash = 1;
    for (char c : hashe) {          // naudok hashe, o ne user_input
        hash = hash ^ (unsigned long long)c;
        hash = hash * 31;
    }

    // pavertimas į hex stringą (16 simbolių = 64 bitai)
    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hash;
    return ss.str();
}

//run_tests();
//store_hashe();