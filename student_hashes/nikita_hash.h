#ifndef NIKITA_HASH_H
#define NIKITA_HASH_H

#include <string>

// Nikitos hash funkcija (64 hex simboliai)
std::string generate_nikita_hash(const std::string& input, bool salt = false);

#endif
