# Hash generatorius 

> **UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## Vartotojo įvestis

* Gali pasirinkti:
  * **Iš failo** (`files/...`)
  * **Įrašyti ranka** (viena eilutė)
* Įvesties klaidoms aptikti naudojami `try/catch` (aiškūs pranešimai, leidžia bandyti iš naujo).
* **Tuščia įvestis** – leidžiama. Tokiu atveju įmaišomas **vienas nulis**, kad hash’as vis tiek būtų deterministiškai sugeneruotas.

---

## HASH’O GENERAVIMO EIGA (v0.11)

1. ASCII → kodai
2. Kartojimas 4 roundus:
   a) Blokavimas (po 4, papildymas)
   b) Apverčiu blokų eilę
   c) Sumaišau elementus priklausomai nuo jų vertės
   d) Sujungiu masyvą
   e) Apverčiu visą masyvą
   f) Sukeičiu puses
   g) ASCII maišymas (kievienas ele. paveikia kitus 3)
3. Seed (fiksuotas 32 simbolių string'as)
4. Maišymas su seed
5. Base62 kodavimas
6. 64 simbolių rezultatas

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


**Patobulinimai maišymo algoritme:**
* Pridėjau bit shifting (>> ir <<) - tai labiau "suplaka" duomenis
* Pridėjau dvigubą loop'ą - for (i) ir for (j)

**Testai:**
* Palyginus hash'us žodžių `"test"` ir `"tast"` (skiriasi tik 1 raide)
* Dabar 39% bitų pasikeičia (buvo tik 1.5%)
* 94% simbolių pasikeičia

**IŠVADA:** hash'ai dabar daug geriau reaguoja į mažus pokyčius 

### 4 užduotis: patobulintas elementų maišymas

**Naujasis algoritmas:**
* **Lyginis skaičius** → šoka **į priekį** per `(5 × vertė × 4)` pozicijas
* **Nelyginis skaičius** → šoka **atgal** per `(3 × vertė × 2)` pozicijas
* `% array_size` - užtikrina, kad naujos pozicijos neišeitų už masyvo ribų

**Kodas:**
```cpp
if (value % 2 == 0) {  // lyginis - stumiu į priekį
    size_t jump = (5 * abs(value) * 4) % n;
    new_pos = (i + jump) % n;
} else {  // nelyginis - stumiu atgal
    size_t jump = (3 * abs(value) * 2) % n;
    new_pos = (i + n - (jump % n)) % n;
}
```

**IŠVADA:** stipresnė difuzija, nes maišymas priklauso individualiai nuo kiekvieno simbolio vertės. Pagerėjo avalanche efektas nuo 1.5% iki 12.6%.

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

### 6 užduotis: papildomas koks nors žingsnis

Pabandžiau pritaikyt algoritmą, kur vienas elementas paveiktų kitus 3 elementus.

**Algoritmo principas:**
* **`val`** = dabartinio elemento vertė pozicijoje `i` (po 4 roundų transformacijų)
* **Targeting:** `target1 = (i + |val|) % array_size` - šoka per elemento vertę
* **Targeting:** `target2 = (i + |val| × 2) % array_size` - šoka per dvigubą vertę  
* **Targeting:** `target3 = (i + |val| × 3) % array_size` - šoka per trigubą vertę
* **Modifikavimas:** target pozicijose pridedama `val`, `val×2`, `val×3`
* **Normalizavimas:** viskas `% 256` (ASCII diapazonas)

**Kodas:**
```cpp
void ascii_mixer(vector<int>& blocks) {
    vector<int> temp = blocks; // kopija
    for (size_t i = 0; i < temp.size(); ++i) {
        int val = temp[i];
        size_t target1 = (i + abs(val)) % blocks.size();
        size_t target2 = (i + abs(val) * 2) % blocks.size();
        size_t target3 = (i + abs(val) * 3) % blocks.size();
        
        blocks[target1] = (blocks[target1] + val) % 256;
        blocks[target2] = (blocks[target2] + val * 2) % 256;
        blocks[target3] = (blocks[target3] + val * 3) % 256;
    }
}
```
**IŠVADA:** ASCII maišytuvas pagerino avalanche efektą, neprarandant greičio.

---

## Eksperimentinis tyrimas 

Visi rezultatai rašomi į **`analysis/`** katalogą.

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

**Principas:** matuoti, kaip greitai algoritmas veikia didėjant įvesties dydžiui.

**Eiga:** pasirenkamas didelis failas (`konstitucija.txt`), skaičiuojama su 1, 2, 4, 8, ... eilutėmis. Testas kartojamas kelis kartus, fiksuojamas vidutinis laikas.

**Rezultatas:** laikai įrašomi į `analysis/perf.csv`:

| Eilutės | Vidutinis laikas (ms) |
|--------:|----------------------:|
|       1 |                  0.00 |
|       2 |                  0.00 |
|       4 |                  0.00 |
|       8 |                  0.00 |
|      16 |                  0.00 |
|      32 |                  0.00 |
|      64 |                  0.00 |
|     128 |                  1.00 |
|     256 |                  3.00 |
|     512 |                  7.60 |

**Komentaras:** algoritmas rodo beveik linijinį augimą, yra visai efektyvus.

---

### 5) Kolizijų paieška

**Principas:** tikrinama, ar dvi skirtingos įvestys gali duoti identišką hash’ą.

**Eiga:** generuojama po 100 000 porų įvairaus ilgio (10, 100, 500, 1000 simbolių) ir lyginami hash’ai.

**Rezultatai (su ASCII maišytuvu ir 24 threads paralelizacija):**

| Ilgis (simbolių) | Porų skaičius | Kolizijų skaičius | Kolizijų dažnis | 
| ---------------: | ------------: | ----------------: | --------------: | 
|               10 |       100,000 |                 0 |       0.000000% |           
|              100 |       100,000 |                 0 |       0.000000% |           
|              500 |       100,000 |                 0 |       0.000000% |            
|             1000 |       100,000 |                 0 |       0.000000% |            

**Komentaras:** kolizijų nepastebėta.

---

### 6) Lavinos efektas

**Principas:** mažas pokytis įvestyje turi sukelti didelį pokytį išvestyje (~50% bitų turėtų pasikeisti).

**Eiga:** testuojamos poros, kurios skiriasi tik vienu simboliu. Skaičiuojama, kiek procentų bitų pasikeičia.

  - **Bitų lygiu:** min=0.0%, max=60.9%, **avg=44.5%**
  - **Hex lygiu:** min=0.0%, max=95.3%, **avg=75.0%**

**Komentaras:** pridėjus ASCII maišymą ir kitą papildomą maišymą (abiejų principas, kad elementai maišosi pagal jų vertę) lavina pagerėjo per 30%. 

---

### 7) Negrįžtamumo demonstracija 

**Principas:** panaudojus papildomą „druską“ (salt), hash’ai turi keistis akivaizdžiai. Tas pats salt turi duoti tą patį hash, o skirtingi – skirtingus.

**Eiga:** palyginami hash’ai, sugeneruoti su įvairiais saltais (`salt1`, `salt2`, `!`, ilgesnis string’as).

**Rezultatas:** tie patys saltai → tie patys hash’ai. Skirtingi saltai → visiškai kitokie hash’ai.

**Komentaras:** demonstracija sėkminga, bet lavinos efektas dar kolkas galėtų būt geresnis.

---

## Rezultatų santrauka (2025-09-30 su ASCII maišytuvu)

* ✅ **Ilgis** – visada 64 simboliai
* ✅ **Deterministiškumas** – užtikrintas visais atvejais
* ✅ **Avalanche efektas** – 44.5% bitų pokytis, 75% hex pokytis (puiku!)
* ✅ **Kolizijos** – 0 kolizijų iš 400,000 testų
* ✅ **Efektyvumas** – linijinis augimas, nepablogėjo su ASCII maišytuvu
* ✅ **Negrįžtamumas** – sėkmingai demonstruotas su salt'ais
* ✅ **Paralelizacija** – testai 331k hash/s su 24 threads
* ✅ **ASCII maišytuvas** – dramatiškai pagerino avalanche be greičio praradimo

---

## Atlikti darbai v0.11

* ✅ **Įdiegti daugiau raundų** - optimizuotas su 4 roundais
* ✅ **Patikrint optimizavimo vėliavėles** - pakeista iš -O2 į -O3
* ✅ **Patobulintas elementų maišymas** - priklauso nuo elemento vertės
* ✅ **OpenMP į testus** - 13.2x greičio pagerinimas
* ✅ **ASCII maišymas** - papildomas maišymo žingsnis

## Tolimesni darbai 

* **Pritaikyt OpenMP/OpenCL pagrindiniam algoritmui** - netinka dėl overhead
* **Dar pagerint lavinos efektą** - jei sugalvosiu kažką protingo
* **Pagerinti seed maišymą** - jei sugalvosiu kažką protingo
* **Palyginimas su egzistuojančiais hash generatoriais** - BONUS
* **AI siūlomi patobulinimai** - bet tik versijoj v0.2


---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.
