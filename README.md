# Hash generatorius (v0.1)

## Apie projektą

**UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## TURINYS

* [Apie projektą](#apie-projektą)
* [Kaip paleisti](#kaip-paleisti)
* [Meniu](#meniu)
* [Vartotojo įvestis](#vartotojo-įvestis)
* [Projekto struktūra](#projekto-struktūra)
* [Hash’o generavimo eiga](#hasho-generavimo-eiga-v01)
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

## Kaip paleisti

```bash
# kompiliacija
make

# programa
./main.exe   # Windows
# arba
./main       # Linux/macOS
```

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

## HASH’O GENERAVIMO EIGA (v0.1)

> **Tikslas:** sukurti **savo** originalų hash’ą, nenaudojant kriptografinių hash’ų bibliotekų.

1. ASCII → kodai
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

## Eksperimentinis tyrimas (testai)

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

**Principas:** Nepriklausomai nuo įvesties ilgio ar turinio, hash’as visada turi būti fiksuoto dydžio – 64 simboliai.

**Eiga:** Generuojami hash’ai iš įvairių įvesčių: tuščios eilutės, vieno simbolio („a“, „b“), ilgesnių ir atsitiktinių failų.

**Rezultatas:** ✅ Visur gauta 64 simbolių eilutė.

---

### 2) Deterministiškumas

**Principas:** Jei įvedama ta pati eilutė, hash’as turi būti identiškas kiekvieną kartą.

**Eiga:** Kartojami bandymai su tomis pačiomis įvestimis (pvz., „a.txt“, „b.txt“, `random_2000_A.txt`).

**Rezultatas:** ✅ Hash’ai identiški visais pakartotiniais paleidimais.

---

### 4) Efektyvumas

**Principas:** Matuoti, kaip algoritmas skaluojasi didėjant įvesties dydžiui.

**Eiga:** Pasirenkamas didelis failas (`konstitucija.txt`), skaičiuojama su 1, 2, 4, 8, ... eilutėmis. Testas kartojamas kelis kartus, fiksuojamas vidutinis laikas.

**Rezultatas:** Laikai įrašomi į `analysis/perf.csv`.  
<img width="2000" height="1114" alt="perf_plot" src="https://github.com/user-attachments/assets/2e27d5ed-ec5b-4318-a16e-e84d05b41039" />


**Komentaras:** Operacijos daugiausia O(n), todėl tikimasi beveik linijinio augimo. Tiksli kreivė priklauso nuo sistemos.

---

### 5) Kolizijų paieška

**Principas:** Tikrinama, ar dvi skirtingos įvestys gali duoti identišką hash’ą.

**Eiga:** Generuojama po 100 000 porų įvairaus ilgio (10, 100, 500, 1000 simbolių) ir lyginami hash’ai.

**Rezultatai:**

| Ilgis (simbolių) | Porų skaičius | Kolizijų skaičius | Kolizijų dažnis |
| ---------------: | ------------: | ----------------: | --------------: |
|               10 |       100000 |                 0 |       0.000000% |
|              100 |       100000 |                 0 |       0.000000% |
|              500 |       100000 |                 0 |       0.000000% |
|             1000 |       100000 |                 0 |       0.000000% |

**Komentaras:** Šiame bandyme kolizijų nepastebėta, bet tai nereiškia, kad jų nėra. Algoritmas nėra kripto saugus, todėl teoriškai kolizijų galima tikėtis su didesniu mastu.

---

### 6) Lavinos efektas

**Principas:** Mažas pokytis įvestyje turi sukelti didelį pokytį išvestyje (~50% bitų turėtų pasikeisti).

**Eiga:** Testuojamos poros, kurios skiriasi tik vienu simboliu. Skaičiuojama, kiek procentų bitų pasikeičia.

**Rezultatai:** Vidutiniškai ~1.5% bitų pokytis, daugiausia 3.125%.

**Komentaras:** Tai yra labai silpnas lavinos efektas. Algoritmo pokyčiai yra per maži, todėl būtina tobulinti difuzijos mechanizmą.

---

### 7) Negrįžtamumo demonstracija 

**Principas:** Panaudojus papildomą „druską“ (salt), hash’ai turi keistis akivaizdžiai. Tas pats salt turi duoti tą patį hash, o skirtingi – skirtingus.

**Eiga:** Palyginami hash’ai, sugeneruoti su įvairiais saltais (`salt1`, `salt2`, `!`, ilgesnis string’as).

**Rezultatas:** ✅ Tie patys saltai → tie patys hash’ai. Skirtingi saltai → visiškai kitokie hash’ai.

**Komentaras:** Demonstracija sėkminga, bet lavinos efektas per silpnas.

---

## Rezultatų santrauka (2025-09-23 sesija)

* ✅ Ilgis – visada 64 simboliai
* ✅ Deterministiškumas – užtikrintas
* ⚠️ Lavina – labai silpna (~1.5% vietoj ~50%)
* ⚠️ Kolizijos – nepastebėtos, bet negarantuota
* ℹ️ Efektyvumas – artimas linijiniam

---

## Tolimesni darbai

* [ ] Įdiegti daugiau raundų
* [ ] Pritaikyt OpenMP/OpenCL
* [ ] Patikrint optimizavimo vėliavėles
* [ ] Naudoti daugiau rotacijų
* [ ] Pagerinti maišymą su seed
* [ ] Negrįžtamumo demonstracija?
* [ ] Papildomos palyginimo BONUS užduotys

---

## Changelog

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).
