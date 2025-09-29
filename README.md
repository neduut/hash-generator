# Hash generatorius (v0.11)

## Apie projektą

**UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## TURINYS

* [Apie projektą](#apie-projektą)
* [Meniu](#meniu)
* [Vartotojo įvestis](#vartotojo-įvestis)
* [Projekto struktūra](#projekto-struktūra)
* [Hash’o generavimo eiga](#hasho-generavimo-eiga-v01)
* [Versija v0.11](#versija-v0.11)
* [Eksperimentinis tyrimas (testai)](#eksperimentinis-tyrimas-testai)
  * [1) Išvedimo dydis](#1-išvedimo-dydis-64-simboliai)
  * [2) Deterministiškumas](#2-deterministiškumas)
  * [4) Efektyvumas](#4-efektyvumas)
  * [5) Kolizijų paieška](#5-kolizijų-paieška)
  * [6) Lavinos efektas](#6-lavinos-efektas)
  * [7) Negrįžtamumo demonstracija](#7-negrįžtamumo-demonstracija-hiding-saltai)
* [Rezultatų santrauka](#rezultatų-santrauka-2025-09-23-sesija)
* [Tolimesni darbai](#tolimesni-darbai-tobulinimo-kryptys)
* [Changelog](#changelog)

---


## Meniu

Programa paleidžia paprastą meniu:

```
Pasirinkite, ką norite daryti:
1 - Generuoti hash
2 - Atlikti tyrimą
0 - Užbaigti programą
```

* **1 – Generuoti hash**: pasirink įvedimo būdą (iš failo arba klaviatūros). Įvesties klaidos gaudomos `try/catch`.
* **2 – Atlikti tyrimą**: atidaromas **testų meniu** (žr. žemiau „Eksperimentinis tyrimas“).

---

## Vartotojo įvestis

* Gali pasirinkti:
  * **Iš failo** (`files/...`)
  * **Įrašyti ranka** (viena eilutė)
* Įvesties klaidoms aptikti naudojami `try/catch` (aiškūs pranešimai, leidžia bandyti iš naujo).
* **Tuščia įvestis** – leidžiama. Tokiu atveju įmaišomas **vienas nulis**, kad hash’as vis tiek būtų deterministiškai sugeneruotas.

---

## Projekto struktūra

**`analysis/` katalogas**
* `perf.csv` – efektyvumo matavimų CSV (**perrašomas** kiekvienos sesijos metu).
* `tests_report.txt` – testų ataskaita (**pildomas**, t. y. *append*).

**`files/` katalogas**
* `a.txt` – 1 simbolis „a“
* `b.txt` – 1 simbolis „b“
* `empty.txt` – tuščias failas (0 baitų)
* `random_2000_A.txt` – 2000 atsitiktinių simbolių
* `random_2000_B.txt` – kitas 2000 atsitiktinių simbolių failas
* `random_2000_M_base.txt` – 2000 atsitiktinių simbolių
* `random_2000_M_variant.txt` – identiškas `*_M_base.txt`, **skiriasi tik 1 vidurinis simbolis** (apie 1000 indeksas nuo 0)

**`src/` katalogas**
* `main.cpp` – programos įėjimo taškas.
* `functions.cpp` – **hash’o generavimo** logika.
* `ui.cpp` – interaktyvus **hash’o generavimo srautas**.
* `tests.cpp` – **eksperimentiniai testai** (rašymas į `analysis/`).

**`include/` katalogas**
* `constants.h` – tekstinės konstantos, `BASE62` simbolių rinkinys.
* `functions.h` – `generate_hash(...)` deklaracijos.
* `mylib.h` – bendri `#include`, `using`.
* `tests.h` – testų API (`run_tests_menu()`).
* `ui.h` – UI srauto API (`run_hash_flow()`).

---

## HASH’O GENERAVIMO EIGA (v0.11)

> **Tikslas:** sukurti **savo** originalų hash’ą, nenaudojant kriptografinių hash’ų bibliotekų.

1. ASCII → kodai
2. viską kartoju keturis roundus
2. Blokavimas (po 4, papildymas)
3. Apverčiu blokų eilę
4. Permutuoju kas antrą bloką
5. Sujungiu masyvą
6. Apverčiu visą masyvą
7. Sukeičiu puses
8. Seed (fiksuotas 32 simbolių string’as)
9. Maišymas su seed
10. Base62 kodavimas
11. 64 simbolių rezultatas

---

## VERSIJA v0.11

### 1 užduotis: įdiegti daugiau roundų

Avalanche efekto analizė pagal roundus su laiko palyginimu:

| Roundai | Bits% avg | Hex% avg | Vertinimas | Avalanche laikas | Efektyvumo santykis |
|---------|-----------|----------|------------|------------------|-------------------|
| **1**   | 1.516     | 2.550    | Per silpnas | ~6s             | Greičiausias |
| **2**   | 2.949     | 4.944    | Silpnas     | ~18s            | Labai greitas |
| **3**   | 4.327     | 7.237    | Vidutinis   | ~24s            | Greitas |
| **4**   | 5.666     | 9.471    | Geras       | ~31s            | **Optimalus** |
| **6**   | 5.615     | 9.391    | Geras       | ~43s            | Geras |
| **8**   | 5.547     | 9.286    | Geras       | ~55s            | Vidutinis |
| **10**  | 5.531     | 9.258    | Geras       | ~68s            | Lėtokas |
| **20**  | 5.558     | 9.291    | Geras       | ~133s           | Lėtas |
| **32**  | 5.453     | 9.139    | Geras       | ~206s           | Labai lėtas |

IŠVADA: pritaikytas 4 roundų patobulinimas

### 2 užduotis: pritaikyt OpenMP/OpenCL

Pasiskaičius pasirinkau OpenMP, nes lengvesnė implementacija, mažiau kodo pakeitimų ir geriau tinka mano algoritmui, nes jis turi daug dependency

`functions.cpp` optimizuotas:
* ASCII konvertavimas 
* Seed maišymas 
* Galutinio hash generavimas 

Atlikti laiko matavimo testai, pilni rezultatai faile: `analysis`/`OPENMP_ANALYSIS.md`
**Input: 100 simbolių**
| Threads | OpenMP (μs) | Sequential (μs) | Speedup | Lėtėjimas |
|---------|-------------|-----------------|---------|-----------|
| 1       | 37.6        | 5.1             | 0.14x   | **7x lėtesnis** |
| 2       | 181.0       | 5.1             | 0.03x   | **35x lėtesnis** |
| 4       | 218.9       | 5.1             | 0.02x   | **43x lėtesnis** |
| 8       | 360.3       | 5.1             | 0.01x   | **71x lėtesnis** |
| 16      | 588.8       | 5.1             | 0.01x   | **115x lėtesnis** |
| 24      | 811.4       | 5.1             | 0.01x   | **159x lėtesnis** |

**Input: 1000 simbolių**
| Threads | OpenMP (μs) | Sequential (μs) | Speedup | Lėtėjimas |
|---------|-------------|-----------------|---------|-----------|
| 1       | 131.9       | 39.2            | 0.30x   | **3x lėtesnis** |
| 2       | 321.8       | 39.2            | 0.12x   | **8x lėtesnis** |
| 4       | 418.3       | 39.2            | 0.09x   | **11x lėtesnis** |
| 8       | 677.2       | 39.2            | 0.06x   | **17x lėtesnis** |
| 16      | 775.4       | 39.2            | 0.05x   | **20x lėtesnis** |
| 24      | 980.9       | 39.2            | 0.04x   | **25x lėtesnis** |

Testuose matyti, jog algoritmas su pritaikytu OpenMP veikia daug lėčiau, nesvarbu ar naudojant porą thread ar 24, ar mažesnį failą ar didesnį. 

**IŠVADA:** OpenMP netinka mano algoritmui (gal pakoreguot algoritmą?)

### 3 užduotis: patikrinti optimizavimo vėliavėles

Atlikti laiko matavimo testai, pilni rezultatai faile: `analysis`/`OPTIMIZATION_FLAGS_ANALYSIS.md)`

| Flag   | 10 chars | 100 chars | 500 chars | 1000 chars | Avg Speedup |
|--------|----------|-----------|-----------|------------|-------------|
| **-O1** | 3.9x     | 5.7x      | 7.7x      | 9.9x       | **6.8x** |
| **-O2** | 5.2x     | 6.3x      | 5.5x      | 5.9x       | **5.7x** |
| **-O3** | 6.7x     | 7.2x      | 8.2x      | 8.1x       | **7.5x** |
| **-Ofast** | 5.1x  | 8.4x      | 4.3x      | 5.1x       | **5.7x** |

**IŠVADA:** pakeičiau iš -O2 į -03

### 4 užduotis: pagerinti seed maišymą

Pakeičiau kaip algoritmas maišo duomenis su seed, kad hash'ai būtų saugesni.

**Patobulinimai maišymo algoritme:**
* Vietoj paprastos sumos dabar naudoju XOR operacijas
* Pridėjau bit shifting (>> ir <<) - tai labiau "suplaka" duomenis
* Pridėjau dvigubą loop'ą - for (i) ir for (j)
* Kiekvienas duomenų elementas dabar paveiks visus seed simbolius, ne tik vieną

**Testai:**
* Palyginus hash'us žodžių `"test"` ir `"tast"` (skiriasi tik 1 raide)
* Dabar 39% bitų pasikeičia (buvo tik 1.5%)
* 94% simbolių pasikeičia

**IŠVADA:** hash'ai dabar daug geriau reaguoja į mažus pokyčius 

### 5 užduotis: OpenMP į testus

Programa labai ilgai atlikinėjo kolizijų testus, todėl baigės kantrybė ir nusprendžiau pabandyt įdėt į testus OpenMP, kad greičiau jie veiktų.

Atlikau laiko matavimo testus su skirtingai threads skaičiais:

Threads | Laikas    | Speedup | Efektyvumas
--------|-----------|---------|------------
1       | 507.7s    | 1.0x    | 100%
4       | 159.0s    | 3.2x    | 80%
12      | 57.4s     | 8.8x    | 73%
18      | 43.7s     | 11.6x   | 65%
24      | 38.4s     | 13.2x   | 55%

**IŠVADA:** nusprendžiau implementuoti 24 threads paraleliniam skaičiavimui.

---

## Eksperimentinis tyrimas 

Visi rezultatai rašomi į **`analysis/`** katalogą.

### Testų meniu

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

### 1) Išvedimo dydis (64 simboliai)

**Principas:** nepriklausomai nuo įvesties ilgio ar turinio, hash’as visada turi būti fiksuoto dydžio – 64 simboliai.

**Eiga:** generuojami hash’ai iš įvairių įvesčių: tuščios eilutės, vieno simbolio („a“, „b“), ilgesnių ir atsitiktinių failų.

**Rezultatas:** visur gauta 64 simbolių eilutė.

---

### 2) Deterministiškumas

**Principas:** jei įvedama ta pati eilutė, hash’as turi būti identiškas kiekvieną kartą.

**Eiga:** kartojami bandymai su tomis pačiomis įvestimis (pvz., „a.txt“, „b.txt“, `random_2000_A.txt`).

**Rezultatas:** hash’ai identiški visais pakartotiniais paleidimais.

---

### 4) Efektyvumas

**Principas:** matuoti, kaip algoritmas skaluojasi didėjant įvesties dydžiui.

**Eiga:** pasirenkamas didelis failas (`konstitucija.txt`), skaičiuojama su 1, 2, 4, 8, ... eilutėmis. Testas kartojamas kelis kartus, fiksuojamas vidutinis laikas.

**Rezultatas:** laikai įrašomi į `analysis/perf.csv`:

| Eilutės | Vidutinis laikas (ms) |
|--------:|----------------------:|
|       1 |                  0.00 |
|       2 |                  0.00 |
|       4 |                  0.00 |
|       8 |                  0.00 |
|      16 |                  1.00 |
|      32 |                  2.20 |
|      64 |                  5.00 |
|     128 |                 12.40 |
|     256 |                 33.60 |
|     512 |                 81.20 |

**Komentaras:** hash generavimas rodo beveik linijinį augimą. Su optimizuotomis compiler flags (-O3) ir 24 threads paralelizacija testams algoritmas veikia efektyviai.

---

### 5) Kolizijų paieška

**Principas:** tikrinama, ar dvi skirtingos įvestys gali duoti identišką hash’ą.

**Eiga:** generuojama po 100 000 porų įvairaus ilgio (10, 100, 500, 1000 simbolių) ir lyginami hash’ai.

**Rezultatai (su 24 threads paralelizacija):**

| Ilgis (simbolių) | Porų skaičius | Kolizijų skaičius | Kolizijų dažnis | Skaičiavimo laikas |
| ---------------: | ------------: | ----------------: | --------------: | -----------------: |
|               10 |       100,000 |                 0 |       0.000000% |              346ms |
|              100 |       100,000 |                 0 |       0.000000% |             2311ms |
|              500 |       100,000 |                 0 |       0.000000% |            11883ms |
|             1000 |       100,000 |                 0 |       0.000000% |            22844ms |

**Bendras testas užtruko:** 39.8s (su 24 threads) vs ~507s (su 1 thread) = **13.2x greičiau**

**Komentaras:** kolizijų nepastebėta, bet tai nereiškia, kad jų nėra. OpenMP paralelizacija dramatiškai pagreitino testų vykdymą.

---

### 6) Lavinos efektas

**Principas:** mažas pokytis įvestyje turi sukelti didelį pokytį išvestyje (~50% bitų turėtų pasikeisti).

**Eiga:** testuojamos poros, kurios skiriasi tik vienu simboliu. Skaičiuojama, kiek procentų bitų pasikeičia.

**Rezultatai (su pagerintais seed maišymu ir 24 threads):**

* **Bitų lygiu:** min=26.8%, max=61.5%, **avg=46.9%**
* **Simbolių lygiu:** min=55.5%, max=97.7%, **avg=80.2%**
* **Greitis:** 41,710 hash/s su paralelizacija
* **Testas užtruko:** 4.8s (duomenų generavimas + skaičiavimai)

**Komentaras:** Po seed maišymo pagerinimų lavinos efektas pagerėjo. Nuo ~1.5% iki **46.9%** bitų - tai jau artima idealiam 50% rezultatui. 

---

### 7) Negrįžtamumo demonstracija 

**Principas:** panaudojus papildomą „druską“ (salt), hash’ai turi keistis akivaizdžiai. Tas pats salt turi duoti tą patį hash, o skirtingi – skirtingus.

**Eiga:** palyginami hash’ai, sugeneruoti su įvairiais saltais (`salt1`, `salt2`, `!`, ilgesnis string’as).

**Rezultatas:** tie patys saltai → tie patys hash’ai. Skirtingi saltai → visiškai kitokie hash’ai.

**Komentaras:** demonstracija sėkminga, bet lavinos efektas dar kolkas galėtų būt geresnis.

---

## Rezultatų santrauka (2025-09-29 sesija)

* ✅ **Ilgis** – visada 64 simboliai
* ✅ **Deterministiškumas** – užtikrintas visais atvejais
* ⚠️ **Lavinos efektas** – 46.9% bitų pokytis, šiaip jau visai nieko
* ✅ **Kolizijos** – kolkas 0 kolizijų
* ✅ **Efektyvumas** – linijinis augimas, optimizuotas su -O3
* ✅ **Negrįžtamumas** – sėkmingai demonstruotas su salt'ais
* ✅ **Paralelizacija** – testai 13.2x greičiau su OpenMP (24 threads)

---

## Atlikti darbai v0.11

* ✅ **Įdiegti daugiau raundų** - optimizuotas su 4 roundais
* ✅ **Patikrint optimizavimo vėliavėles** - pakeista iš -O2 į -O3
* ✅ **Pagerinti maišymą su seed** - dramatiškai pagerintas lavinos efektas
* ✅ **OpenMP į testus** - 13.2x greičio pagerinimas

## Tolimesni darbai 

* **Pritaikyt OpenMP/OpenCL pagrindiniam algoritmui** - netinka dėl overhead
* **Dar pagerint lavinos efektą** - jei sugalvosiu kažką protingo
* **Palyginimas su egzistuojančiais hash generatoriais** - BONUS
* **AI siūlomi patobulinimai** - bet tik versijoj v0.2


---

## Changelog

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3, pagerintas seed maišymas, implementuotas OpenMP į testus.
