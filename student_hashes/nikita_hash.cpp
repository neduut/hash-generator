#include "nikita_hash.h"
#include <vector>
#include <sstream>
#include <random>
#include <cstdlib>

using std::vector;
using std::string;
using std::mt19937;
using std::uniform_int_distribution;
using std::ostringstream;

static string nikita_hash_impl(const string& input, bool add_salt) {
    constexpr int HASH_LENGTH = 64;
    vector<int> integerVector;
    integerVector.reserve(input.length() * 2);
    long int uniqueStringNumber = 1;
    int counter = 0;
    string data = input;

    if (add_salt) {
        std::random_device rd;
        mt19937 engine(rd());
        uniform_int_distribution<int> charDistribution(33, 126);
        string s;
        for (int i = 0; i < 10; i++) {
            s += static_cast<char>(charDistribution(engine));
        }
        data.append(s);
    }

    for (char c : data) {
        int castedChar = static_cast<int>(c);
        if (castedChar < 0) castedChar *= -1;
        integerVector.push_back(castedChar);
        if (counter % 4 == 0) uniqueStringNumber = std::abs(uniqueStringNumber) - 255;
        if (counter % 3 == 0) uniqueStringNumber = (uniqueStringNumber + 1) * -1;
        if (counter % 2 == 0) uniqueStringNumber += castedChar + 1;
        else uniqueStringNumber -= castedChar;
        counter++;
    }

    mt19937 engine(uniqueStringNumber + 3);
    uniform_int_distribution<int> indexDistribution(0, HASH_LENGTH - 1);
    uniform_int_distribution<int> randomDistribution(0, 15);
    int hash[HASH_LENGTH] = {0};

    for (auto el : integerVector) {
        hash[indexDistribution(engine)] += el;
    }
    for (int i = 0; i < HASH_LENGTH; i++) {
        hash[i] += randomDistribution(engine);
        hash[i] = hash[i] % 16;
    }

    ostringstream oss;
    for (int i = 0; i < HASH_LENGTH; i++) {
        oss << std::hex << hash[i];
    }
    return oss.str();
}

std::string generate_nikita_hash(const std::string& input, bool salt) {
    return nikita_hash_impl(input, salt);
}
