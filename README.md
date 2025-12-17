**PAPILDOMA UŽDUOTIS:** palyginti savo hash su kitais hash'ais.

---

## Palyginimas su standartiniais hash algoritmais

### Efektyvumas (laikas milisekundėmis)
*Naudotas files/konstitucija.txt failas.*

| Eilučių kiekis | Neda (v0.1final) | SHA-256 | MD5 |
|:---:|:---:|:---:|:---:|
| 1 eilutė | 0.03 | 0.03 | 0.00 |
| 2 eilutės | 0.02 | 0.01 | 0.00 |
| 4 eilutės | 0.03 | 0.00 | 0.00 |
| 8 eilutės | 0.06 | 0.01 | 0.00 |
| 16 eilučių | 0.11 | 0.01 | 0.01 |
| 32 eilutės | 0.19 | 0.02 | 0.01 |
| 64 eilutės | 0.46 | 0.04 | 0.06 |
| 128 eilutės | 1.16 | 0.12 | 0.07 |
| 256 eilutės | 2.63 | 0.23 | 0.13 |
| 512 eilučių | 5.62 | 0.40 | 0.25 |
| Visas failas (789 eiltės)| 7.71 | 0.70 | 0.40 |


### Atsparumas kolizijoms

| Vieno string poroje ilgis | Neda (v0.1final) | SHA-256 | MD5 |
|:---:|:---:|:---:|:---:|
| 10 | 0 | 0 | 0 |
| 100 | 0 | 0 | 0 |
| 500 | 0 | 0 | 0 |
| 1000 | 0 | 0 | 0 |

### Lavinos efektas - BIT skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Neda (v0.1final) | 42.19% | 0.00% | 29.24% |
| SHA-256 | 42.77% | 23.63% | 33.00% |
| MD5 | 47.66% | 20.31% | 33.02% |

### Lavanos efektas - HEX skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Neda (v0.1final) | 100.00% | 0.00% | 82.62% |
| SHA-256 | 100.00% | 78.12% | 93.76% |
| MD5 | 100.00% | 71.88% | 93.75% |

---

## Palyginimas tarp skirtingų studentų hash'ų
(Nesinaudojant AI)

### Efektyvumas (laikas milisekundėmis)

| Eilučių kiekis | Neda | Miglė | Vanesė | Justė | Tėja | Nerijus | Nikita |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 eilutė | - | - | - | - | - | - | - |
| 2 eilutės | - | - | - | - | - | - | - |
| 4 eilutės | - | - | - | - | - | - | - |
| 8 eilutės | - | - | - | - | - | - | - |
| 16 eilučių | - | - | - | - | - | - | - |
| 32 eilutės | - | - | - | - | - | - | - |
| 64 eilutės | - | - | - | - | - | - | - |
| 128 eilutės | - | - | - | - | - | - | - |
| 256 eilutės | - | - | - | - | - | - | - |
| 512 eilučių | - | - | - | - | - | - | - |
| Visas failas (789 eiltės) | - | - | - | - | - | - | - |

### Atsparumas kolizijoms

| Vieno string poroje ilgis | Neda | Miglė | Vanesė | Justė | Tėja | Nerijus | Nikita |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 10 | 0 | - | - | - | - | - | - |
| 100 | 0 | - | - | - | - | - | - |
| 500 | 0 | - | - | - | - | - | - |
| 1000 | 0 | - | - | - | - | - | - |

### Lavanos efektas - BIT skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Neda | - | - | - |
| Miglė | - | - | - |
| Vanesė | - | - | - |
| Justė | - | - | - |
| Tėja | - | - | - |
| Nerijus ir Ignas | - | - | - |
| Nikita | - | - | - |

### Lavanos efektas - HEX skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Neda | - | - | - |
| Miglė | - | - | - |
| Vanesė | - | - | - |
| Justė | - | - | - |
| Tėja | - | - | - |
| Nerijus ir Ignas | - | - | - |
| Nikita | - | - | - |


---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – įvairios eksperimentinės užduotys aprašytos `README.md`, seed generavimas su matrica kuri dauginama su gautais duomenim, patobulintas seed maišymas, optimizuotas hash generavimo algoritmas.

* **v0.1final** – galutinis own hasho versijos v0.1 readme su pseudo kodu. Išėmiau dalinimą į blokus, nes nelabai turi prasmės mano algoritme dabar jau. Įdėjau salt'ą.

* **v0.1comparison** – naudoju v0.1final mano hash'o kodą palyginimui su SHA-256 ir kitų studentų hash'ais.

* **v0.2** – AI patobulinimų versija. Integravau kriptografinius algoritmus: enhanced mixing su magic constants (0x9E3779B9, 0x85EBCA6B), bit rotation optimizacija naudojant C++20 std::rotl,"tThree-in-one mixer" patobulinimas - cascade efektas lavinos patobulinimui, Value-dependent shuffle - dinaminis maišymas pagal element vertes, 5-round processing - papildomas stabilumo raundas.

* **v0.1comparison** – naudoju v0.1final mano hash'o kodą palyginimui su SHA-256 ir kitų studentų hash'ais.