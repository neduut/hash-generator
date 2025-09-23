# Hash generatorius (v0.1)

**UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## Kaip paleisti

```bash
# kompiliacija
make

# programa
./main.exe   # Windows
# arba
./main       # jei Linux/macOS
```

### Meniu
Programa paleidžia paprastą meniu:

```
Pasirinkite, ką norite daryti:
1 - Generuoti hash
2 - Atlikti tyrimą
0 - Užbaigti programą
```

- **1 – Generuoti hash**: pasirink įvedimo būdą (iš failo arba klaviatūros). Įvesties klaidos gaudomos `try/catch`.
- **2 – Atlikti tyrimą**: atidaromas testų meniu (žr. „Eksperimentinis tyrimas“).

---

## Vartotojo įvestis

- Gali pasirinkti:
  - **Iš failo** (`files/...`)
  - **Įrašyti ranka** (viena eilutė)
- Įvesties klaidoms aptikti naudojami **`try/catch`** (aiškūs pranešimai, leidžia bandyti iš naujo).
- **Tuščia įvestis** – leidžiama. Tokiu atveju įmaišomas **vienas nulis**, kad hash’as vis tiek būtų deterministiškai sugeneruotas.

---

## Projekto išplanavimas

**1. `analysis` katalogas**
- **`perf.csv`** – efektyvumo matavimų CSV (perrašomas kiekvienos sesijos metu).
- **`tests_report.txt`** – testų ataskaita (pildomas, t. y. **append**).

**2. `files` katalogas**
- **`a.txt`** – 1 simbolis „a“ ]
- **`b.txt`** – 1 simbolis „b“ 
- **`empty.txt`** – tuščias failas (0 baitų)
- **`random_2000_A.txt`** – 2000 atsitiktinių simbolių (a-zA-Z0-9 ir kt.)
- **`random_2000_B.txt`** – kitas 2000 atsitiktinių simbolių failas
- **`random_2000_M_base.txt`** – 2000 atsitiktinių simbolių
- **`random_2000_M_variant.txt`** – identiškas `*_M_base.txt`, **skiriasi tik 1 vidurinis simbolis** (ties ~1000 indeksu nuo 0)

**3. `src` katalogas**
- **`main.cpp`** – programos įėjimo taškas (pagrindinis meniu, valdymo ciklas).
- **`functions.cpp`** – **hash’o generavimo** logika ir pagalbinės funkcijos.
- **`ui.cpp`** – interaktyvus **hash’o generavimo srautas** („įvedimas → rezultatas → klaidų tvarkymas“).
- **`tests.cpp`** – **eksperimentiniai testai** ir **tyrimų meniu** (rašymas į `analysis/`).

**4. `include` katalogas**
- **`constants.h`** – tekstinės konstantos (meniu, pranešimai), BASE62 simbolių rinkinys ir pan.
- **`functions.h`** – `generate_hashe(...)` deklaracijos.
- **`mylib.h`** – bendri `#include`, `using` ir utilitai (patogumui).
- **`tests.h`** – testų API (pvz., `run_tests_menu()`).
- **`ui.h`** – UI srauto API (pvz., `run_hash_flow()`).

---

## HASHO GENERAVIMO EIGA (v0.1)

> **Tikslas:** sukurti **savo** originalų hash’ą, nenaudojant „tikrų“ kriptografinių hash’ų implementacijų.

1. **ASCII**: kiekvieną simbolį paverčiu jo **ASCII kodu**.
2. **Blokavimas**: padalinu skaičius į **blokus po 4**.
   - Jei paskutinis blokas trumpesnis – **papildau** elementais iš pradžios, **mod 64**.
   - Jei **įvestis tuščia**, į `ascii_vals` įdedu **vieną nulį**, kad transformacijos veiktų stabiliai.
3. **Apverčiu** visų blokų **eilę** (blokų tvarką).
4. **Permutuoju** **kiekvieną antrą bloką** pagal formulę:
   `new_index = (index * 3 + 1) % block_size`
5. **Sujungiu** blokus atgal į **vieną masyvą**.
6. **Apverčiu** visą masyvą.
7. **Perpus ir sukeičiu** puses vietomis (antroji pusė → pirma).
8. **Seed**: fiksuotas pradinis „sėklos“ string’as  
   `A1b2C3d4E5f6G7h8I9j0K1l2M3n4O5p6` *(32 simboliai)*.
9. **Maišymas** su seed:
   - Perbėgu visą masyvą ir kiekvieną elementą **pridedu** prie atitinkamo seed simbolio (cikliškai).
   - `naujas_seed_char = to_base62( (ASCII(seed_char) + element) % 62 )`
10. **Base62**: rezultatas konvertuojamas į **0-9, a-z, A-Z** simbolius (mod 62).
11. **Išvedimas**: suformuoju **64 simbolių** hash’ą.

> Pastaba: čia **ne kriptografiškai saugus** hash’as. Tai **mokomasis** „savo“ generatorius, su kuriuo atliekami tyrimai (lavinos efektas, kolizijos ir pan.).

---

## EKSPERIMENTINIS TYRIMAS

Visi rezultatai rašomi į **`analysis/`** katalogą.

**Testų meniu:**
```
0 - visi testai
1 - išvedimo dydis
2 - deterministiškumas
4 - efektyvumas
5 - kolizijų paieška
6 - lavinos efektas
7 - negrįžtamumo demonstracija
q - grįžti
```

**Kas tikrinama:**
1. **Išvedimo dydis** – nepriklausomai nuo įvedimo, rezultatas **visada 64 simbolių** (Base62).
2. **Deterministiškumas** – tas pats įvedimas → **tas pats** hash’as.
3. **Efektyvumas**:
   - Naudojamas **`files/konstitucija.txt`**.
   - Matuojamas laikas su **1, 2, 4, 8, ...** eilutėmis.
   - Kartojama **po kelis kartus**, skaičiuojamas **vidurkis**.
   - Rezultatai rašomi į **`analysis/perf.csv`** *(perrašomas kiekvienos sesijos metu)*.
4. **Kolizijų paieška**:
   - Generuojamos po **100 000** atsitiktinių **porų** ilgiams **10, 100, 500, 1000**.
   - Skaičiuojama, kiek porų turi **tą patį hash’ą** (kolizijos).
   - Įrašoma į **`analysis/tests_report.txt`** *(pildomas)*.
5. **Lavinos efektas**:
   - Generuojama **100 000** porų, kurios skiriasi **vienu simboliu**.
   - Matuojamas skirtingumas **bitų** (per Base62→6 bitų) ir „**hex**“ lygmeniu.
   - Rodomos **min / max / avg** reikšmės.
   - Įrašoma į **`analysis/tests_report.txt`**.
6. **Negrįžtamumo demonstracija** (**hiding**, puzzle-friendliness):
   - Skaičiuojama **`HASH(input + salt)`** su keliais „druskomis“.
   - Parodoma, kad skirtingi `salt` duoda **skirtingus** hash’us (išskyrus tyčia pakartotą `salt1`).

**Failai:**
- **`analysis/tests_report.txt`** – **kaupiama** testų ataskaita (append).
- **`analysis/perf.csv`** – **perrašomas** kiekvienos sesijos metu.


