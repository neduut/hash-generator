# Hash generatorius 

UŽDUOTIS:** patobulinti savo sukurtą hash su AI įrankiais.

---

## MANO HASH’O GENERAVIMO EIGA SU AI PATOBULINIMAIS

1. [Paverčiu string į ASCII kodus ir integruoju salt](#1-string-į-ascii)
2. [Penki AI patobulinti maišymų roundai:](#2-penki-maišymų-roundai)
   - 2.1 [Enhanced mixing su magic constants](#21-enhanced-mixing)
   - 2.2 [Three-in-one mixer su cascade efektu](#22-trys-viename-maišymas)
   - 2.3 [Value-dependent shuffle](#23-dinaminis-maišymas)
   - 2.4 [Swap halves - pusių apvertimas](#24-pusių-apvertimas)
3. [Seed generavimas - matricos daugyba su įvestim](#3-seed-generavimas)
4. [Finalinis maišymas su seed](#4-finalinis-maišymas)
5. [Pavertimas į base62 (64 simboliai)](#5-base62-konvertavimas)

---

### PSEUDO KODAS SU AI PATOBULINIMAIS

### 1. String į ASCII
```
function stringToAscii(input):
    arr = empty array
    for each character c in input:
        append ASCII value of c to arr
    
    if arr is empty:
        append 0 to arr
    
    return arr
```

**AI patobulinimas:** Salt įvedamas ankstesniame etape, kad padidintų įtaką visam maišymo procesui.

### 2. Penki maišymų roundai
```
function processRounds(inputArray):
    data = copy of inputArray
    ROUNDS = 5
    
    for r from 0 to ROUNDS-1:
        previous = copy of data
        
        // Keturi skirtingi maišymo žingsniai
        value_dependent_shuffle(previous)
        three_in_one_mixer(previous)
        swap_halves(previous)
        
        // Inter-round mixing
        if r < ROUNDS-1:
            for i from 0 to previous.length-1:
                previous[i] = enhanced_mix(previous[i], r+1, i)
        
        data = previous
    
    return data
```

**AI patobulinimas:** Padidintas roundų skaičius nuo 4 iki 5 - geresniam balansui tarp greičio ir saugumo. Pridėtas inter-round mixing naudojant enhanced_mix funkciją tarp roundų.

### 2.1 Enhanced mixing
```
function enhanced_mix(value, salt, position):
    v = uint32(value)
    s = uint32(salt)
    p = uint32(position)
    
    // Magic konstanta iš MurmurHash3
    v = v XOR (rotl32(s, 7) + p * 0x9E3779B9)
    v = rotl32(v, 13) XOR (v >> 16)
    v = v * 0x85EBCA6B  // FNV-1a konstanta
    v = v XOR (v >> 13)
    v = v * 0xC2B2AE35
    v = v XOR (v >> 16)
    
    return v & 0xFF  // grąžiname 0-255 intervale
```

**AI patobulinimas:** Įdiegta std::rotl iš C++20 efektyvesniam bit rotation, pridėti kriptografiniai algoritmų principai iš MurmurHash3 ir FNV-1a.

### 2.2 "Trys viename" maišymas
```
function three_in_one_mixer(array):
    if array is empty:
        return
    
    temp = copy of array
    
    for i from 0 to temp.length-1:
        sk = temp[i]
        
        // Apskaičiuojame pozicijas, kurias paveiks elementas
        e1 = (i + sk) % array.length
        e2 = (i + sk*2) % array.length
        e3 = (i + sk*3) % array.length
        
        // Nelinijikas maišymas vietoj paprastos sumos
        array[e1] = enhanced_mix(array[e1] + sk, sk, i)
        array[e2] = enhanced_mix(array[e2] + sk*2, sk*2, i+1)
        array[e3] = enhanced_mix(array[e3] + sk*3, sk*3, i+2)
        
        // Feedback loop - cascade efektas
        feedback_pos = (i + array[e1] + array[e2] + array[e3]) % array.length
        array[feedback_pos] = array[feedback_pos] XOR (rotl32(sk, i % 32) & 0xFF)
```

**AI patobulinimas:** Pridėtas cascade efektas - vieno elemento pokyčio įtaka perduodama visai grandinei per feedback loop mechanizmą, stipriai pagerinant lavinos efektą.

### 2.3 Dinaminis maišymas
```
function value_dependent_shuffle(array):
    if array is empty:
        return
    
    temp = copy of array
    n = array.length
    
    for i from 0 to n-1:
        value = temp[i]
        
        // Apskaičiuojame naują poziciją pagal elemento vertę
        if value is even:  // lyginis - keliauja į priekį
            jump = (17 * abs(value) * 7 + i * 23) % n
            new_pos = (i + jump) % n
        else:  // nelyginis - keliauja atgal
            jump = (13 * abs(value) * 11 + i * 19) % n
            new_pos = (i + n - (jump % n)) % n
        
        // Nelinijinis maišymas vietoj paprastos reikšmės perkėlimo
        array[new_pos] = enhanced_mix(value, temp[(i+1) % n], i)
```

**AI patobulinimas:** Pridėtas enhanced_mix stipresniam bit diffusion perkeliant elementus, vietoj paprastos reikšmės perkėlimo naudojamas nelinijinis mixing.

### 2.4 Pusių apvertimas
```
function swap_halves(array):
    n = array.length
    h = n / 2  // antroji pusė bus ilgesnė jei nelyginis dydis
    
    first = array[0...h-1]   // pirma pusė
    second = array[h...n-1]  // antra pusė
    
    // Cross-mixing tarp pusių prieš sukeičiant
    for i from 0 to min(first.length, second.length)-1:
        mix1 = enhanced_mix(first[i], second[i], i)
        mix2 = enhanced_mix(second[i], first[i], i + h)
        first[i] = mix1
        second[i] = mix2
    
    // Sukeitimas
    array.clear()
    array.append(second)
    array.append(first)
```

**AI patobulinimas:** Pridėtas cross-mixing tarp pusių prieš sukeičiant - elementai iš vienos pusės maišomi su elementais iš kitos, sustiprinant kryžminį difuzijos efektą.

### 3. Seed generavimas
```
function generate_seed(inputArray):
    seed = "Kx9mN3vL8qR5wY1pZ7jT2bF6hC4nA0sD"  // pradinis seed
    
    // Sudėtingesnė matrica su pirminiais skaičiais
    matrix = [[17, 29], [23, 31]]
    
    // Apdorojame elementus poromis
    for i from 0 to inputArray.length-2 step 2:
        a = inputArray[i]
        b = inputArray[i+1]
        
        pos1 = i % seed.length
        pos2 = (i+1) % seed.length
        
        // Enhanced mixing matricų operacijoms
        mix1 = enhanced_mix(matrix[0][0] * a + matrix[0][1] * b, a + b, i)
        mix2 = enhanced_mix(matrix[1][0] * a + matrix[1][1] * b, a XOR b, i+1)
        
        seed[pos1] = BASE62[mix1 % 62]
        seed[pos2] = BASE62[mix2 % 62]
    
    // Jei liko vienas elementas nelyginiame masyve
    if inputArray.length is odd:
        last_idx = inputArray.length - 1
        pos = last_idx % seed.length
        mix = enhanced_mix(inputArray[last_idx] * 17, pos * 23, last_idx)
        seed[pos] = BASE62[mix % 62]
    
    return seed
```

**AI patobulinimas:** Pakeista į sudėtingesnę matricą su pirminiais skaičiais (17, 29, 23, 31) ir pridėtas enhanced_mix užtikrinti nelinijiškumą.

### 4. Salt generavimas ir finalinis maišymas su seed
```
function generate_and_apply_salt(inputArray, seed):
    // Salt generavimas
    salt = ""
    if inputArray is not empty:
        input_sum = sum of all elements in inputArray
        
        for i from 0 to 3:
            salt_val = enhanced_mix(
                input_sum * (i + 7),
                inputArray[i % inputArray.length] * 13,
                i
            )
            salt += to_base62(salt_val)
        
        // Salt integravimas į seed
        for i from 0 to salt.length-1:
            salt_ascii = ASCII value of salt[i]
            salt_pos = (input_sum + i * salt_ascii) % seed.length
            mixed_val = enhanced_mix(
                ASCII value of seed[salt_pos] + salt_ascii,
                input_sum,
                i
            )
            seed[salt_pos] = to_base62(mixed_val)
    
    return seed
```

```
function final_mixing_with_seed(array, seed):
    for i from 0 to array.length-1:
        si = i % seed.length  // seed indeksą sukame ratu
        seed_val = ASCII value of seed[si]
        data_val = array[i]
        pos_factor = i + 1
        
        // Enhanced mix vietoj paprastos daugybos
        mixed = enhanced_mix(seed_val * data_val, pos_factor, i)
        seed[si] = to_base62(mixed % 62)
    
    return seed
```

**AI patobulinimas:** Stiprintas seed mixing mechanizmas, naudojant nelinijinius operatorius vietoj paprastos daugybos, pridėta pozicijos įtaka maišymui.

### 5. Pavertimas į base62
```
function finalize_hash(array, seed):
    output = empty string
    
    for i from 0 to 63:  // generuojame 64 simbolius
        a = ASCII value of seed[i % seed.length]  // seed simbolis (ratu)
        b = array[i % array.length]  // masyvo elementas (ratu)
        
        // Enhanced finalization su nelinijiniu maišymu
        final_mix = enhanced_mix(a + b, i * 17, i)
        output.append(to_base62(final_mix % 62))
    
    return output
```

**AI patobulinimas:** Pridėtas enhanced finalization su nelinijiniu maišymu kiekvienam simboliui, naudojant pozicijos įtaką (i * 17) ir unikalų seed-data kombinacijos maišymą.

### Pilnas algoritmas
```
function generate_hash(input):
    // 1. Konvertuojam input į ASCII masyvą
    array = stringToAscii(input)
    
    // 2. Atliekame 5 maišymų roundus
    processedArray = processRounds(array)
    
    // 3. Generuojame seed
    seed = generate_seed(array)
    
    // 4. Generuojame ir integruojame salt
    seed = generate_and_apply_salt(array, seed)
    
    // 4. Finalinis maišymas su seed
    seed = final_mixing_with_seed(processedArray, seed)
    
    // 5. Pavertimas į base62
    hash = finalize_hash(processedArray, seed)
    
    return hash
```

---

## MANO IR AI PATOBULINTO HASH'O PALYGINIMAS

Visi rezultatai rašomi į **`analysis/comparison.results.txt`** failą.
Testams pritaikytas OpenMP su 24 gijomis (threads) maksimaliam našumui.

## 1) Išvedimo dydis (64 simboliai)

**Principas.** Nepriklausomai nuo įvesties, hash'as visada fiksuoto dydžio – 64 simboliai.

**Eiga.** Bandiniai: tuščia eilutė, vienas simbolis ("a", "b"), ilgesnės eilutės ir atsitiktiniai failai.

**Rezultatas.** Visur gauta 64 simbolių eilutė (jokių pokyčių nuo v0.13). 

---

## 2) Deterministiškumas

**Principas.** Ta pati įvestis → identiškas hash'as kiekvieną kartą.

**Eiga.** Kartoti bandymai su tomis pačiomis įvestimis (pvz., „a.txt", „b.txt", `random_2000_A.txt`).

**Rezultatas.** Hash'ai identiški visais pakartotiniais paleidimais (kaip ir v0.13). 

---

## 4) Efektyvumas

**Principas.** Matuoju laiką didėjant įvesties dydžiui ir srautą (hash/s).

<img width="1480" height="880" alt="output3" src="https://github.com/user-attachments/assets/fdb851c2-aae8-49f4-96f4-9f78927ebd58" />

**Komentaras.** Iki ~64 eilučių laikas praktiškai 0 ms; nuo 128–512 eilučių laikas auga nuspėjamai ir proporcingai. Srautas sumažėjo nuo ~738,007 iki ~409,836 hash/s (−44.5%), tačiau tai yra sąmoningas kompromisas dėl geresnių kriptografinių savybių:

- Pridėti nelinijiniai operatoriai (XOR, rotacija, magic konstantos) reikalauja daugiau CPU ciklų
- Enhanced mixing funkcijos iškvietimai visose kritinėse vietose (5-10x daugiau nei anksčiau)
- Cascade efektas su feedback loop three-in-one mixer funkcijoje
- Papildomas raundas (iš 4 į 5) prideda ~25% papildomo darbo

---

## 5) Kolizijų paieška

**Principas.** Tikrinu, ar skirtingos įvestys gali duoti identišką hash'ą.

**Nauja (100,000 porų, 24 gijos):**

| Ilgis | Kolizijos |    Dažnis | Compute time | Total time |
| ----: | --------: | --------: | -----------: | ---------: |
|    10 |         0 | 0.000000% |         69ms |       78ms |
|   100 |         0 | 0.000000% |        250ms |      337ms |
|   500 |         0 | 0.000000% |       1141ms |     1512ms |
|  1000 |         0 | 0.000000% |       2149ms |     2956ms |

**Komentaras.** Kolizijų nenustatyta – atitinka v0.13 rezultatą. Skaičiavimai užtruko ilgiau (anksčiau: 10 eilučių - 42ms, 1000 eilučių - 1321ms), tačiau kolizijų neatsirado net ir 100,000 skirtingų atsitiktinių įvesčių porų. Tai rodo, kad AI patobulintas maišymo algoritmas išlaikė arba net pagerino atsparumą kolizijoms, nors šį teiginį reikėtų tikrinti su daug didesniu testų skaičiumi praktikoje.

---

## 6) Lavinos efektas

**Principas.** Mažas pokytis įvestyje turi sukelti ~50% bitų pokytį išvestyje.

**Nauja (100,000 porų, LEN=200, 24 gijos):**

* **THROUGHPUT:** 409,836.1 hash/s
* **bits%:** min=1.823, max=61.198, **avg=47.632**
* **hex%:**  min=3.125, max=96.875, **avg=80.447**

**Komentaras.** Ryškiausi patobulinimai matomi lavinos efekto testuose:

- Bitų lygio vidurkis pakilo nuo 45.2% iki 47.63% (**+2.43 procentiniai punktai**, artinamės prie idealaus 50%)
- Heksų lygio vidurkis pakilo nuo 76.5% iki 80.45% (**+3.95 procentiniai punktai**)

Šie patobulinimai tiesiogiai kilo iš:
1. Enhanced mixing funkcijos su magic konstantomis (0x9E3779B9, 0x85EBCA6B)
2. Bit rotation optimizacijų su std::rotl
3. Cascade efekto three-in-one mixer funkcijoje
4. Cross-mixing tarp pusių prieš sukeičiant
5. Nelinijinio mixing seed generavimo procese

---

## 7) Negrįžtamumo demonstracija (salt)

**Eiga.** Lyginami hash'ai su įvairiais druskomis („salt1", „salt2", „!", ilgas stringas). Tas pats salt → tas pats hash; skirtingas salt → visiškai kitoks hash.

**Nauji pavyzdžiai:**

```
salt="salt1" -> 63TsVhocJdMcng4mtyy8wLO4DhWZ7Xn6SoXJ77qPr4YnPs0xomkK9zT68QoxNl85
salt="salt1" -> 63TsVhocJdMcng4mtyy8wLO4DhWZ7Xn6SoXJ77qPr4YnPs0xomkK9zT68QoxNl85
salt="salt2" -> HjSZkdcmooSjM88mW7aJkvCiUJ5iBRUTUIxJhE1EoB3p7MzQGOgp7ATcVsdm9kEY
salt="ilgesnis_saltas_123" -> M2vKHGanWlmelzXB024zs8OIRE2cg9cvx8t2dK14idgq636oVTDBG4xj7S0wDZnd
salt="!" -> UCz7TOF2b8svABCsEJkHodEzerUE6I1dz0J5MGx5cu32yQFgmQ8ZdfxAKyUZ8bAY
salt="XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" -> t3ZjbebAQmLfcs0OMZr9zP4gNpZGIW1d6hmjsZh4v5r68gb8oQqPEbdLHcxBRs47
```

**Komentaras.** AI patobulintas salt mechanizmas rodo puikius rezultatus:
1. Tas pats salt garantuotai duoda identišką hash'ą (stabilumas)
2. Net minimaliai pakeitus salt, gaunami visiškai skirtingi hash'ai (stipri išvesties priklausomybė nuo salt)
3. Tiek trumpi, tiek ilgi salt duoda vienodai efektyvų maišymą (salt ilgio nepriklausomybė)

---

## Santrauka ir palyginimas su v0.13

**Kas pagerėjo:**

* **Lavinos efektas (bits%)**: 45.2% → **47.63%** (**+2.43 p. p.**) — artėjame prie idealaus 50% bitų pokyčio
* **Lavinos efektas (hex%)**: 76.5% → **80.45%** (**+3.95 p. p.**) — pagerėjimas beveik 4 procentiniais punktais
* **Atsparumas kolizijoms:** išliko **0 kolizijų** per 100,000 porų testuotų įvesčių, visuose ilgiuose
* **Algoritmo stiprumas:** pridėtos kriptografinės konstantos ir maišymo metodai iš pripažintų hash algoritmų (MurmurHash3, FNV-1a)
* **Atsparumas kriptoanalizei:** sudėtingesnis maišymas su cross-mixing tarp etapų daro algoritmo analizę sunkesnę

**Kas suprastėjo:**

* **Srautas (hash/s):** ~738,007 → **~409,836** (**−44.5%**) — beveik 2x lėtesnis dėl sudėtingesnio maišymo
* **Compute time didesniems duomenims:** +62.7% ilgesnis skaičiavimas 1000 eilučių testui (nuo 1321ms iki 2149ms)
* **Bendras algoritmo sudėtingumas:** kodas tapo sudėtingesnis dėl papildomų nelinijinių operacijų ir mechanizmų

**Išvada.** Naujoji AI patobulinta versija ženkliai pagerino maišymo savybes ir lavinos efektą, išlaikydama kritines hash algoritmo savybes (deterministiškumas, fiksuotas ilgis, nulinės kolizijos). Pasiektas kompromisas tarp greičio ir saugumo - algoritmas tapo ~2x lėtesnis, tačiau ~11.5% pagerėjo lavinos efektas (nuo 42.7% iki 47.63% viso pakeitimo metu). Greitį ateityje būtų galima didinti šiomis priemonėmis:

* Leisti sukonfigūruoti „mixing intensity" (pvz., mažesnis nelinijinių iteracijų skaičius „fast" režime)
* Naudoti adaptyvų thread'ų parinkimą pagal įvesties dydį (mažoms įvestims užtenka mažiau gijų)
* Pridėti „SIMD‑friendly" transformacijas karščiausiose kilpose (AVX/AVX2 instrukcijos)
* Selektyviai taikyti enhanced_mix funkciją tik kritiškiausiuose taškuose

---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – įvairios eksperimentinės užduotys aprašytos `README.md`, seed generavimas su matrica kuri dauginama su gautais duomenim, patobulintas seed maišymas, optimizuotas hash generavimo algoritmas.

* **v0.1final** – galutinis own hasho versijos v0.1 readme su pseudo kodu. Išėmiau dalinimą į blokus, nes nelabai turi prasmės mano algoritme dabar jau. Įdėjau salt'ą.

* **v0.2** – AI patobulinimų versija. Integravau kriptografinius algoritmus: enhanced mixing su magic constants (0x9E3779B9, 0x85EBCA6B), bit rotation optimizacija naudojant C++20 std::rotl,"tThree-in-one mixer" patobulinimas - cascade efektas lavinos patobulinimui, Value-dependent shuffle - dinaminis maišymas pagal element vertes, 5-round processing - papildomas stabilumo raundas.
