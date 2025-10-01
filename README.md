# Hash generatorius ## HASH'O GENERAVIMO EIGA 

1. [Paverčiu string į ASCII kodus](#String-į-ASCII)
2. [Keturi maišymų roundai:](#Keturi-maišymų-roundai)
   2.1 [Sumaišau elementus priklausomai nuo jų vertės](#Maišymas,-priklausantis-nuo-vertės)
   2.2 [Super maišymas (kievienas elementas paveikia kitus 3)](#"Trys-viename"-maišymas)
   2.3 [Padalinu per pusę ir sukeičiu vietom](#Pusių-apvertimas)
3. [Seed generavimas - matricos daugyba su įvestim](Seed-generavimas)
4. [Salt generavimas ir integravimas](Salt-generavimas-ir-integravimas)
5. [Maišymas su seed](#Maišymas-su-seed)
6. [Pavertimas į base62](#Pavertimas-į-base62)TIS:** sukurti **savo originalų** hash’o generatorių, atlikti **testus** ir iteratyviai **tobulinti**.

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

## HASH’O GENERAVIMO EIGA 

1. [Paverčiu string į ASCII kodus](#String-į-ASCII)
2. [Keturi maišymų roundai:](#Keturi-maišymų-roundai)
   2.1 [Sumaišau elementus priklausomai nuo jų vertės](#Maišymas,-priklausantis-nuo-vertės)
   2.2 [Super maišymas (kievienas elementas paveikia kitus 3)](#"Trys-viename"-maišymas)
   2.3 [Padalinu per pusę ir sukeičiu vietom](#Pusių-apvertimas)
3. [Seed generavimas - matricos daugyba su įvestim](Seed-generavimas)
4. [Maišymas su seed](#Maišymas-su-seed)
5. [Pavertimas į base62](#Pavertimas-į-base62)

---

## PSEUDO KODAS

### 1. String į ASCII
```
FUNKCIJA generate_hash(user_input):
    current = tuščias_masyvas
    
    UŽ kiekvieną simbolį c IŠ user_input:
        pridėti ASCII(c) į current
    
    JEI current yra tuščias:
        pridėti 0 į current  // apsauga nuo tuščios įvesties
```

### 2. Keturi maišymų roundai
```
    ROUNDS = 4
    data = current  // kopija originalių duomenų
    
    UŽ r NUO 0 IKI ROUNDS:
        previous = data  // dabartinio round'o duomenys
        
        // Round'o žingsniai:
        value_dependent_shuffle(previous)  // 2.1
        three_in_one_mixer(previous)       // 2.2  
        swap_halves(previous)              // 2.3
        
        data = previous  // kitas round'as naudos šiuos duomenis
```

### 2.1 Maišymas, priklausantis nuo vertės
```
FUNKCIJA value_dependent_shuffle(previous):
    temp = previous  // kopija, kad nesugadinti originalius
    n = previous.size()
    
    UŽ i NUO 0 IKI n:
        value = temp[i]
        
        JEI value yra lyginis:
            jump = (17 * |value| * 7 + i * 23) % n
            new_pos = (i + jump) % n        // šoka į priekį
        KITAIP:
            jump = (13 * |value| * 11 + i * 19) % n  
            new_pos = (i + n - jump) % n    // šoka atgal
        
        previous[new_pos] = value  // perkelia elementą
```

### 2.2 "Trys viename" maišymas
```
FUNKCIJA three_in_one_mixer(previous):
    temp = previous  // kopija originalių verčių
    
    UŽ i NUO 0 IKI temp.size():
        sk = temp[i]
        
        // Vienas elementas paveiks tris kitus:
        e1 = (i + sk) % previous.size()
        e2 = (i + sk * 2) % previous.size()  
        e3 = (i + sk * 3) % previous.size()
        
        // Modifikuoja tris pozicijas:
        previous[e1] = (previous[e1] + sk) % 256
        previous[e2] = (previous[e2] + sk * 2) % 256
        previous[e3] = (previous[e3] + sk * 3) % 256
```

### 2.3 Pusių apvertimas
```
FUNKCIJA swap_halves(previous):
    n = previous.size()
    h = n / 2  // integer division: 5/2=2
    
    first = previous[0...h-1]      // pirma pusė
    second = previous[h...n-1]     // antra pusė
    
    previous = second + first      // sukeičia vietomis
    // Pvz: [A,B,C,D,E] -> [C,D,E,A,B]
```

### 3. Seed generavimas
```
FUNKCIJA generate_seed(current):
    seed = "Kx9mN3vL8qR5wY1pZ7jT2bF6hC4nA0sD"  // bazinis seed
    matrix = [[7, 13], [11, 5]]  // 2x2 transformacijos matrica
    
    // Porinis apdorojimas su matrica:
    UŽ i NUO 0 IKI current.size() ŽINGSNIU 2:
        JEI i+1 < current.size():
            a = current[i]
            b = current[i+1]
            
            pos1 = i % seed.size()
            pos2 = (i+1) % seed.size()
            
            // Matricos daugyba ir base62 konvertavimas:
            seed[pos1] = BASE62[(matrix[0][0]*a + matrix[0][1]*b) % 62]
            seed[pos2] = BASE62[(matrix[1][0]*a + matrix[1][1]*b) % 62]
    
    // Nelyginio masyvo atvejis:
    JEI current.size() % 2 == 1:
        last_idx = current.size() - 1
        pos = last_idx % seed.size()
        seed[pos] = BASE62[(current[last_idx] * 17 + pos * 23) % 62]
    
    GRĄŽINTI seed
```

### 4. Salt generavimas ir integravimas
```
    // Generuoju 4 simbolių salt iš input charakteristikų
    salt = tuščias_string
    input_sum = suma visų current elementų
    
    UŽ i NUO 0 IKI 4:
        salt_val = (input_sum * (i + 7) + current[i % current.size()] * 13) % 62
        salt += BASE62[salt_val]
    
    // Integruoju salt į seed
    UŽ kiekvieną simbolį s IŠ salt:
        salt_ascii = ASCII(s)
        salt_pos = (input_sum + i * salt_ascii) % seed.size()
        new_val = (seed[salt_pos] + salt_ascii + input_sum) % 62
        seed[salt_pos] = BASE62[new_val]
```

### 5. Maišymas su seed
```
    seed = generate_seed(current)  // gauti input-priklausomą seed
    
    // Seed ir duomenų maišymas:
    UŽ i NUO 0 IKI previous.size():
        si = i % seed.size()  // cikliškas seed indeksas
        
        // Sudėtinga formulė su pozicijos poveikiu:
        rez = (ASCII(seed[si]) * previous[i] * (i+1)) % 256
        
        // Atnaujinti seed:
        seed[si] = BASE62[rez % 62]
```

### 6. Pavertimas į base62
```
    output = tuščias_string
    
    // Generuoti 64 simbolių hash:
    UŽ i NUO 0 IKI 64:
        a = ASCII(seed[i % seed.size()])      // seed simbolis (cikliškai)
        b = previous[i % previous.size()]     // duomenų elementas (cikliškai)
        
        // Galutinė formulė su pozicijos poveikiu:
        v = (a + b + i * 17) % 62
        
        output += BASE62[v]  // pridėti base62 simbolį
    
    GRĄŽINTI output  // 64 simbolių hash
```



## EKSPERIMENTINIS TYRIMAS

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

## HASHO APTARIMAS

### Kas gerai:
- **Avalanche testas** gavosi 44.3%, tai arti 50% kas yra geras rezultatas
- **Kolizijų neradau** per 400k testus, tai gerai
- **Greitas algoritmas** - 790k hash'ų per sekundę
- **Dirba su bet kokiu tekstu** - ir trumpu ir ilgu

### Kas galėtų būt geriau:
- **Nežinau ar tikrai saugus** - niekas dar netikrino ar nėra skylių
- **Modulo operacijos** gal sukuria kokių nors pattern'ų
- **64 simbolių hash** gal per trumpas rimtesnėms aplikacijoms

### Išvados:
Algoritmas atrodo gerai failu tikrinimui ar paprastoms užduotims. Bet rimtai kriptografijai geriau naudot jau patvirtintus algoritmus kaip SHA-256.

Galima tobulint: padaryti ilgesnį hash'ą, testuot prieš žinomas atakas.

**Automatinis salt jau pridėtas** - algoritmas dabar generuoja dinaminį salt iš input charakteristikų!



---

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

* **v0.1final** – galutinis own hasho versijos v0.1 readme su pseudo kodu. Išėmiau dalinimą į blokus, nes nelabai turi prasmės mano algoritme dabar jau. Įdėjau salt'ą.