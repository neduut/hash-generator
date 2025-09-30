# Hash generatorius 

> **UŽDUOTIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

---

## Vartotojo įvestis

* 1. Meniu gali pasirinkti:
  * Generuoti hash
  * Atlikti tyrimą
  * Palyginti su MD5

* 2. Įvestį gali pasirinkti:
  * **Iš failo** (`files/...`)
  * **Įrašyti ranka** (viena eilutė)
* Įvesties klaidoms aptikti naudojami `try/catch` (aiškūs pranešimai, leidžia bandyti iš naujo).
* **Tuščia įvestis** – leidžiama. Tokiu atveju įmaišomas **vienas nulis**, kad hash’as vis tiek būtų deterministiškai sugeneruotas.

---

## HASH’O GENERAVIMO EIGA (v0.12)

1. ASCII → kodai
2. Kartojimas 4 roundus:
   a) Blokavimas (po 4, papildymas)
   b) Apverčiu blokų eilę
   c) Sumaišau elementus priklausomai nuo jų vertės
   d) Sujungiu masyvą
   e) Apverčiu visą masyvą
   f) Sukeičiu puses
   g) ASCII maišymas (kievienas el. paveikia kitus 3)
3. Seed (fiksuotas 32 simbolių string'as)
4. Maišymas su seed
5. Base62 kodavimas
6. 64 simbolių rezultatas

---

## VERSIJA v0.12

### 1 užduotis: patobulint seed maišymą

1) Padariau vietoj sudeties daugybą

**IŠVADA:** pagerėjo greitis :Dd? bet avanache nepakito

2) Priklausomybė nuo elemento vietos

i=0: (i+1) = 1 → elementas dauginamas iš 1
i=1: (i+1) = 2 → elementas dauginamas iš 2
i=2: (i+1) = 3 → elementas dauginamas iš 3
i=50: (i+1) = 51 → elementas dauginamas iš 51
Kad grįžtų į ribas padarau % 256

Dabar tas pats elementas skirtingose vietose duos skirtingą reikšmę

**IŠVADA:** daug reikšmės nedavė, labai minimaliai pagerėjo analanche ir efektyvumas kažkaip

3) x5 - nepritaikiau

Padariau penkiagūbą seed maišymo ciklą.
Pabandžiau x2 ir kitokius skaičius.

**IŠVADA:** avalanche tiek mažai pagerėjo, kad neapsimoka dėl to prarasti efektyvumo.

3) Apvertimas - nepritaikiau

Seed apvertimas po kiekvieno žingsnio.

**IŠVADA:** efektas toks mažas, kad neapsimoka.

### 2 užduotis: pertikrint algortimą

Tikrinau algortimo efektyvumą išimant žingsnius. Tokiu būdų išėmiau nereikalingas vietas, kurios nepridėjo daug naudos, tokiu būdu algoritmas aiškesnis ir trumpesnis.

Išėmiau: 
b) apverciu bloku eile (po 4 elementus)
d) vel sujungiu i viena masyva
e) apverciu visa masyva

**IŠVADA:** praradau tik 0.14% avalanche efekto, bet gavau +111% performance pagerinimą.

### 3 užduotis: palygint su jau egzistuojančiu Hash

Palyginau su MD5

---

## Eksperimentinis tyrimas 

Visi rezultatai rašomi į **`analysis/`** katalogą.
Testams pritaikytas OpenMP su 24 threads.

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
|     128 |                  0.00 |
|     256 |                  1.00 |
|     512 |                  3.00 |

**Komentaras:** algoritmas yra visai efektyvus.

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

## Rezultatų santrauka (2025-09-30)

* ✅ **Ilgis** – visada 64 simboliai
* ✅ **Deterministiškumas** – užtikrintas visais atvejais
* ✅ **Avalanche efektas** – 44.5% bitų pokytis, 75% hex pokytis (puiku!)
* ✅ **Kolizijos** – 0 kolizijų iš 400,000 testų
* ✅ **Efektyvumas** – linijinis augimas, nepablogėjo su ASCII maišytuvu
* ✅ **Negrįžtamumas** – sėkmingai demonstruotas su salt'ais
* ✅ **Paralelizacija** – testai 331k hash/s su 24 threads
* ✅ **ASCII maišytuvas** – dramatiškai pagerino avalanche be greičio praradimo

---

## Atlikti darbai v0.12

* ✅ **Patobulint seed maišymą** - minimaliai
* ✅ **Pertikrint algortimo žingsnius** - optimizavau algoritmą
* ✅**Palyginimas su egzistuojančiais hash generatoriais** - palyginau su MD5


## Tolimesni darbai 

* **Pseudo kodas į README**
* **Pritaikyt OpenMP/OpenCL pagrindiniam algoritmui** - kaži ar būtina
* **Dar pagerint lavinos efektą** - jei sugalvosiu kažką protingo
* **Pagerinti seed maišymą**
* **AI siūlomi patobulinimai** - bet tik versijoj v0.2


---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – 
