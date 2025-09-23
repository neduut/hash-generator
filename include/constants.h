#pragma once
#include <string>

// base
inline constexpr char BASE62[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// user prompts
inline const std::string ENTER_INPUT_TYPE =
    "\nPasirinkite įvedimo būdą:\n"
    "1 - Rankiniu būdu\n"
    "2 - Iš failo\n";

inline const std::string GENERATE_HASH_PROMPT =
    "\nGeneruoti dar vieną hash (1) ar užbaigti programą (0)? ";

inline const std::string ENTER_DATA =
    "\nĮveskite duomenis:\n";

inline const std::string TRY_AGAIN_ =
    "\nBandykite dar kartą.";

// menu
inline constexpr char MENU_TEXT[] =
    "\nPasirinkite, ką norite daryti:\n"
    "1 - Generuoti hash\n"
    "2 - Atlikti tyrimą\n"
    "0 - Nieko nenoriu daryti\n";

inline constexpr char TEST_MENU_TEXT[] =
    "\nPasirinkite tyrimą:\n"
    "1 - išvedimo dydis laikas\n"
    "2 - deterministiškumas\n"
    "4 - efektyvumas\n"
    "5 - kolizijų paieška\n"
    "6 - lavinos efektas\n"
    "7 - negrįžtamumo demonstracija\n"
    "8 - užbaigti programą\n";

inline constexpr char ENTER_FILE_NAME[] =
    "\nPasirinkite failą:\n"
    "1 - a.txt\n"
    "2 - b.txt\n"
    "3 - empty.txt\n"
    "4 - random_2000_A.txt\n"
    "5 - random_2000_B.txt\n"
    "6 - random_2000_M_base.txt\n"
    "7 - random_2000_M_variant.txt\n"
    "8 - konstitucija.txt\n";

// errors
inline const std::string INVALID_CHOICE   = "\nKlaida: netinkamas pasirinkimas! ";
inline const std::string FILE_OPEN_ERROR  = "\nKlaida: nepavyko atidaryti failo ";
inline const std::string FILE_READ_ERROR  = "\nKlaida: nepavyko nuskaityti failo ";
inline const std::string DATA_READ_ERROR  = "\nKlaida: nepavyko nuskaityti įvesties! ";

// success
inline const std::string FILE_READ_SUCCESS = "\nDuomenys nuskaityti sėkmingai! ";
inline const std::string HASH_SUCCESS      = "\nHash generavimas sėkmingas! ";
inline const std::string TEST_SUCCESS      = "\nTyrimo rezultatai įrašyti į failą: ";
