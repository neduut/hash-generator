# Hash generatorius 

**UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## Vartotojo įvestis

1. Meniu gali pasirinkti:
  * Generuoti hash
  * Atlikti tyrimą
  * Palyginti su MD5

2. Įvestį gali pasirinkti:
  * **Iš failo** (`files/...`)
  * **Įrašyti ranka** (viena eilutė)
* Įvesties klaidoms aptikti naudojami `try/catch` (aiškūs pranešimai, leidžia bandyti iš naujo).
* **Tuščia įvestis** – leidžiama. Tokiu atveju įmaišomas **vienas nulis**, kad hash’as vis tiek būtų deterministiškai sugeneruotas.

---

## HASH’O GENERAVIMO EIGA (v0.12)

1. ASCII → kodai
2. Kartojimas 4 roundus:
   a) Blokavimas (po 4, papildymas)
   b) Sumaišau elementus priklausomai nuo jų vertės
   c) Super maišymas (kievienas el. paveikia kitus 3)
   d) padalinu per pusę ir sukeičiu vietom
3. Seed generavimas - matricos daugyba su įvestim
4. Maišymas su seed
5. Base62 kodavimas
6. 64 simbolių rezultatas

---

## VERSIJA v0.12

---

### 1 užduotis: patobulint seed maišymą

**1) Padariau vietoj sudėties daugybą**

**IŠVADA:** pagerėjo greitis :Dd? bet avanache nepakito

**2) Priklausomybė nuo elemento vietos**

* i=0: (i+1) = 1 → elementas dauginamas iš 1
* i=1: (i+1) = 2 → elementas dauginamas iš 2
* i=2: (i+1) = 3 → elementas dauginamas iš 3
* i=50: (i+1) = 51 → elementas dauginamas iš 51
Kad grįžtų į ribas padarau % 256

Dabar tas pats elementas skirtingose vietose duos skirtingą reikšmę

**IŠVADA:** daug reikšmės nedavė, labai minimaliai pagerėjo analanche ir efektyvumas kažkaip

**3) x5 - nepritaikiau**

Padariau penkiagūbą seed maišymo ciklą.
Pabandžiau x2 ir kitokius skaičius.

**IŠVADA:** avalanche tiek mažai pagerėjo, kad neapsimoka dėl to prarasti efektyvumo.

**4) Apvertimas - nepritaikiau**

Seed apvertimas po kiekvieno žingsnio.

**IŠVADA:** efektas toks mažas, kad neapsimoka.

---

### 2 užduotis: pertikrint algortimą

Tikrinau algortimo efektyvumą išimant žingsnius. Tokiu būdų išėmiau nereikalingas vietas, kurios nepridėjo daug naudos, tokiu būdu algoritmas aiškesnis ir trumpesnis.

Išėmiau: 
b) apverčiu blokų eilę (po 4 elementus)
d) vėl sujungiu į vieną masyvą
e) apverčiu visą masyvą

**IŠVADA:** praradau tik 0.14% avalanche efekto, bet gavau +111% performance pagerinimą.

---

### 3 užduotis: įdėti kažką gal su matrica 

Įdėjau daugyba su 4x4 matrica kaip papildomą žingsnį kur vyksta 4 roundai.
Gal reiktų įdėt OpenMP į tai? Reik pabandyt.

```cpp
int matrix[4][4] = {
    {7, 13, 5, 11},
    {9, 3, 17, 6},
    {4, 15, 8, 12},
    {14, 2, 10, 16}
};
```

**Algoritmas:**
1. Imami 4 elementus iš masyvo 
2. Atliekama matricos daugyba:
   - naujas[0] = (7*a + 13*b + 5*c + 11*d) % 256
   - naujas[1] = (9*a + 3*b + 17*c + 6*d) % 256  
   - naujas[2] = (4*a + 15*b + 8*c + 12*d) % 256
   - naujas[3] = (14*a + 2*b + 10*c + 16*d) % 256
3. Jei lieka elementų mažiau nei 4, jie maišomi paprastai: `(elementas * 19 + pirmas_elementas * 23) % 256`

**IŠVADA:** su 4x4 avalanche nepagerėjo, reik pabandyt su didesne.

Bandysim su 1000x1000 matrica :Dd čia jau tikrai reikės OpenMP.
Teko padaryt ribojimą, kad matricos dydis max toks kiek elementų masyve, bet ir maksimalus matricos dydis būtų 1000x1000, kad nesprogtų kompas jei būtų labai didelė įvestis, todėl deja jei masyvo dydis >1000, tai likę elementai bus maišomi paprastai. (Čia būtų galima vėliau pasidomėt kaip išspręst šitą reikalą)
Taigi, matricos dydis prisitaiko prie duomenų.

Matricos parametras
```cpp
size_t n = std::min(blocks.size(), (size_t)1000);
```
Matricos daugyba su OpenMP
```cpp
#pragma omp parallel for schedule(dynamic)
  for (size_t i = 0; i < n; ++i) {
      int sum = 0;
      for (size_t j = 0; j < n; ++j) {
      if (j < temp.size()) {
        sum += matrix[i][j] * temp[j];
        }
      }
      if (i < blocks.size()) {
        blocks[i] = sum % 256;
      }
  }
```

**IŠVADA:** Nu man rodos čia reiktų OpenCL net ne OpenMP :Ddd šia kartui šią gražią 1000x1000 matricą išimsim. 

Bet nusprendžiau dar pabandyt su 10X1O matrica.

**IŠVADA:** greitis visiškai susigadino. 

Bandau dar 3x3 matricą.

**MATRICOS DYDŽIŲ PALYGINIMAS:**

| Matricos dydis | Throughput | Compute time | Total time | Avalanche bits% | Išvada |
|----------------|------------|--------------|------------|-----------------|---------|
| **BE matricos** | 743k hash/s | 269ms | 3017ms | 44.308% | Baseline |
| **3x3 matrica** | **769k hash/s** | 260ms | 3264ms | **44.481%** | ✅ **OPTIMALUS** |
| **4x4 matrica** | 862k hash/s | 232ms | 3052ms | 43.862% | Greitas bet avalanche pablogėjo |
| **10x10 matrica** | 107k hash/s | 1863ms | 9292ms | 44.476% | ❌ Per lėtas |

**(ne)GALUTINĖ IŠVADA:** paliksiu 3x3 matricą dėl grožio (originalumo :Dd), bet šiaip ir avalanche geriausias ir greičio labai nepagadino, tai vis šis tas.

Nusprendžiau sugrįžti prie šitos idėjos ir pabandyti įdėti matricų daugybą į kitą vietą - į seed generavimą. Kad ne visą laik naudotų tą patį seed, o kad jis priklausytų nuo duomenų.

Įdėjau mini matricą daugybai.

---

### 4 užduotis: jau sugeneruoto seed dar vienas permaišymas - nepritaikiau

Kiekvienas iš 64 simbolių paveikia visus kitus 63:
* kas ketvirtam nuo i+1 pridės savo reikšmę mod 256
* kas ketvirtam nuo i+2 atims savo reikšmę mod 256
Ir taip viska sukasi rastu per visus elementus išskyrus i, bet prieš keičiant kitą elementą visą laik prie i pridedu seną jo reikšmę.

**IŠVADA:** nepritaikiau, nes neapsimoka. `super3mixer` yra pagrindinis algoritmas kuris paveikia avalanche, šiam momentui nesugalvoju dar kažko kas galėtų jį pralenk ir pagerint avalanche. Viskas kitas neduoda jokios prasmės šalia `super3mixer`.

---

## Eksperimentinis tyrimas 

Visi rezultatai rašomi į **`analysis/tests.report.txt`** failą.
Testams pritaikytas OpenMP su 24 threads.

### 1) Išvedimo dydis (64 simboliai)

> **Principas:** nepriklausomai nuo įvesties ilgio ar turinio, hash’as visada turi būti fiksuoto dydžio – 64 simboliai.

**Eiga:** generuojami hash’ai iš įvairių įvesčių: tuščios eilutės, vieno simbolio („a“, „b“), ilgesnių ir atsitiktinių failų.

**Rezultatas:** visur gauta 64 simbolių eilutė.

---

### 2) Deterministiškumas

> **Principas:** jei įvedama ta pati eilutė, hash’as turi būti identiškas kiekvieną kartą.

**Eiga:** kartojami bandymai su tomis pačiomis įvestimis (pvz., „a.txt“, „b.txt“, `random_2000_A.txt`).

**Rezultatas:** hash’ai identiški visais pakartotiniais paleidimais.

---

### 4) Efektyvumas

> **Principas:** matuoti, kaip greitai algoritmas veikia didėjant įvesties dydžiui.

**Eiga:** pasirenkamas didelis failas (`konstitucija.txt`), skaičiuojama su 1, 2, 4, 8, ... eilutėmis. Testas kartojamas kelis kartus, fiksuojamas vidutinis laikas.

**Rezultatas:** laikai įrašomi į **`analysis/tests.report.txt`** failą.

<img width="1580" height="980" alt="output" src="https://github.com/user-attachments/assets/aaf5adc7-7fb9-4cab-a9be-e377945ac04f" />


**Komentaras:** algoritmas yra visai efektyvus.

---

### 5) Kolizijų paieška

> **Principas:** tikrinama, ar dvi skirtingos įvestys gali duoti identišką hash’ą.

**Eiga:** generuojama po 100 000 porų įvairaus ilgio (10, 100, 500, 1000 simbolių) ir lyginami hash’ai.


| Ilgis (simbolių) | Porų skaičius | Kolizijų skaičius | Kolizijų dažnis |
| ---------------: | ------------: | ----------------: | --------------: |
|               10 |       100,000 |                 0 |       0.000000% |
|              100 |       100,000 |                 0 |       0.000000% |
|              500 |       100,000 |                 0 |       0.000000% |
|             1000 |       100,000 |                 0 |       0.000000% |

         

**Komentaras:** kolizijų nepastebėta.

---

### 6) Lavinos efektas

> **Principas:** mažas pokytis įvestyje turi sukelti didelį pokytį išvestyje (~50% bitų turėtų pasikeisti).

**Eiga:** testuojamos poros, kurios skiriasi tik vienu simboliu. Skaičiuojama, kiek procentų bitų pasikeičia.

**Rezultatai (su 3x3 matrica, 100k porų, 24 threads):**
  - **Throughput:** 772,200 hash/s
  - **Compute time:** 259ms 
  - **Bitų lygiu:** min=0.0%, max=60.9%, **avg=44.5%**
  - **Hex lygiu:** min=0.0%, max=96.1%, **avg=75.1%**

**Komentaras:** pridėjus ASCII maišymą ir kitą papildomą maišymą (abiejų principas, kad elementai maišosi pagal jų vertę) lavina pagerėjo per 30%. 

---

### 7) Negrįžtamumo demonstracija 

> **Principas:** panaudojus papildomą „druską“ (salt), hash’ai turi keistis akivaizdžiai. Tas pats salt turi duoti tą patį hash, o skirtingi – skirtingus.

**Eiga:** palyginami hash’ai, sugeneruoti su įvairiais saltais (`salt1`, `salt2`, `!`, ilgesnis string’as).

**Rezultatas:** tie patys saltai → tie patys hash’ai. Skirtingi saltai → visiškai kitokie hash’ai.

**Komentaras:** demonstracija sėkminga, bet lavinos efektas dar kolkas galėtų būt geresnis.

---

## Rezultatų santrauka (2025-09-30)

* ✅ **Ilgis** – visada 64 simboliai
* ✅ **Deterministiškumas** – užtikrintas visais atvejais
* ✅ **Avalanche efektas** – 44.5% bitų pokytis, 75.1% hex pokytis 
* ✅ **Kolizijos** – 0 kolizijų dar vis nerasta
* ✅ **Efektyvumas** – linijinis augimas, 772k hash/s su matrica
* ✅ **Negrįžtamumas** – sėkmingai demonstruotas su salt'ais
* ✅ **Paralelizacija** – testai su 24 threads OpenMP
* ✅ **3x3 matrica** – optimalus balansas tarp avalanche ir greičio

---

## Atlikti darbai v0.12

* ✅ **Patobulint seed maišymą** - minimaliai, priklausomybė nuo elemento vietos
* ✅ **Pertikrint algortimo žingsnius** - optimizavau algoritmą
* ✅ **Kažką su matricom** - padariau daugybą su mini matrica seed'o generavime
* ✅ **Sugeneruoto hash dar vienas permaišymas** - nepritaikiau


## Tolimesni darbai 

* **Palygint su jau egzistuojančiu Hash**
* **Palyginimas su MD5 į README**
* **Pseudo kodas į README**
* **Diagramos į README**
* **AI siūlomi patobulinimai** - bet tik versijoj v0.2


---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – įvairios eksperimentinės užduotys aprašytos `README.md`, seed generavimas su matrica kuri dauginama su gautais duomenim, patobulintas seed maišymas, optimizuotas hash generavimo algoritmas.
