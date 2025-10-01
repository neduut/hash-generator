# Hash generatorius 

UŽDUOTIS:** patobulinti savo sukurtą hash su AI įrankiais.

---

## MANO HASH’O GENERAVIMO EIGA 

1. [Paverčiu string į ASCII kodus](#1-string-į-ascii)
2. [Keturi maišymų roundai:](#2-keturi-maišymų-roundai)
   - 2.1 [Sumaišau elementus priklausomai nuo jų vertės](#21-maišymas-priklausantis-nuo-vertės)
   - 2.2 [Super maišymas (kievienas elementas paveikia kitus 3)](#22-trys-viename-maišymas)
   - 2.3 [Padalinu per pusę ir sukeičiu vietom](#23-pusių-apvertimas)
3. [Seed generavimas - matricos daugyba su įvestim](#3-seed-generavimas)
4. [Salt generavimas ir integravimas](#4-salt-generavimas-ir-integravimas)
5. [Maišymas su seed](#5-maišymas-su-seed)
6. [Pavertimas į base62](#6-pavertimas-į-base62)

---

## MANO IR AI PATOBULINTO HASH'O PALYGINIMAS

Visi rezultatai rašomi į **`analysis/comparison.results.txt`** failą.
Testams pritaikytas OpenMP su 24 threads.

### 1) Išvedimo dydis (64 simboliai)

> **Principas:** nepriklausomai nuo įvesties ilgio ar turinio, hash’as visada turi būti fiksuoto dydžio – 64 simboliai.

**Eiga:** generuojami hash’ai iš įvairių įvesčių: tuščios eilutės, vieno simbolio („a“, „b“), ilgesnių ir atsitiktinių failų.



---

### 2) Deterministiškumas

> **Principas:** jei įvedama ta pati eilutė, hash’as turi būti identiškas kiekvieną kartą.

**Eiga:** kartojami bandymai su tomis pačiomis įvestimis (pvz., „a.txt“, „b.txt“, `random_2000_A.txt`).



---

### 4) Efektyvumas

> **Principas:** matuoti, kaip greitai algoritmas veikia didėjant įvesties dydžiui.

**Eiga:** pasirenkamas didelis failas (`konstitucija.txt`), skaičiuojama su 1, 2, 4, 8, ... eilutėmis. Testas kartojamas kelis kartus, fiksuojamas vidutinis laikas.

**Rezultatas:** laikai įrašomi į **`analysis/tests.report.txt`** failą.

<img width="1580" height="980" alt="output" src="https://github.com/user-attachments/assets/aaf5adc7-7fb9-4cab-a9be-e377945ac04f" />




---

### 5) Kolizijų paieška

> **Principas:** tikrinama, ar dvi skirtingos įvestys gali duoti identišką hash’ą.

**Eiga:** generuojama po 100 000 porų įvairaus ilgio (10, 100, 500, 1000 simbolių) ir lyginami hash’ai.



---

### 6) Lavinos efektas

> **Principas:** mažas pokytis įvestyje turi sukelti didelį pokytį išvestyje (~50% bitų turėtų pasikeisti).

**Eiga:** testuojamos poros, kurios skiriasi tik vienu simboliu. Skaičiuojama, kiek procentų bitų pasikeičia.



---

### 7) Negrįžtamumo demonstracija 

> **Principas:** panaudojus papildomą „druską“ (salt), hash’ai turi keistis akivaizdžiai. Tas pats salt turi duoti tą patį hash, o skirtingi – skirtingus.

**Eiga:** palyginami hash’ai, sugeneruoti su įvairiais saltais (`salt1`, `salt2`, `!`, ilgesnis string’as).



---

## APTARIMAS



---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – įvairios eksperimentinės užduotys aprašytos `README.md`, seed generavimas su matrica kuri dauginama su gautais duomenim, patobulintas seed maišymas, optimizuotas hash generavimo algoritmas.

* **v0.1final** – galutinis own hasho versijos v0.1 readme su pseudo kodu. Išėmiau dalinimą į blokus, nes nelabai turi prasmės mano algoritme dabar jau. Įdėjau salt'ą.

* **v0.1comparison** –