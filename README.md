**PAPILDOMA UŽDUOTIS:** palyginti savo hash su kitais hash'ais.

---

## Palyginimas su standartiniais hash algoritmais

### Efektyvumas (laikas milisekundėmis)
*Naudotas files/konstitucija.txt failas, 24 OpenMP thread'ai.*

| Eilučių kiekis | Neda (v0.1final) | SHA-256 | MD5 |
|:---:|:---:|:---:|:---:|
| 1 eilutė | 0.01 | 0.01 | 0.00 |
| 2 eilutės | 0.02 | 0.00 | 0.00 |
| 4 eilutės | 0.02 | 0.00 | 0.00 |
| 8 eilutės | 0.09 | 0.01 | 0.01 |
| 16 eilučių | 0.10 | 0.01 | 0.01 |
| 32 eilutės | 0.18 | 0.02 | 0.01 |
| 64 eilutės | 0.36 | 0.03 | 0.02 |
| 128 eilutės | 0.95 | 0.07 | 0.04 |
| 256 eilutės | 3.20 | 0.25 | 0.16 |
| 512 eilučių | 5.25 | 0.45 | 0.27 |
| Visas failas | 7.44 | 0.68 | 0.41 |

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
| Neda (v0.1final) | 42.21% | 12.00% | 31.24% |
| SHA-256 | 41.60% | 23.63% | 33.01% |
| MD5 | 45.31% | 19.14% | 33.00% |

### Lavanos efektas - HEX skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Neda (v0.1final) | 100.00% | 1.56% | 82.62% |
| SHA-256 | 100.00% | 76.56% | 93.74% |
| MD5 | 100.00% | 68.75% | 93.75% |

---

## Palyginimas su kitų studentų hash algoritmais

### Efektyvumas (laikas milisekundėmis)
*Naudotas files/konstitucija.txt failas, 24 OpenMP thread'ai.*

| Eilučių kiekis | Nerijus | Nikita | Neda (v0.1final) | Miglė | Gustavo | Vanesa |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 eilutė | 0.00 | 0.02 | 0.01 | 0.01 | 28.19 | 0.03 |
| 2 eilutės | 0.00 | 0.01 | 0.02 | 0.02 | 18.32 | 0.03 |
| 4 eilutės | 0.00 | 0.01 | 0.02 | 0.05 | 28.98 | 0.03 |
| 8 eilutės | 0.00 | 0.01 | 0.09 | 0.17 | 32.72 | 0.07 |
| 16 eilučių | 0.01 | 0.01 | 0.10 | 1.02 | 25.90 | 0.08 |
| 32 eilutės | 0.01 | 0.02 | 0.18 | 3.22 | 22.27 | 0.20 |
| 64 eilutės | 0.02 | 0.04 | 0.36 | 14.41 | 44483.41 | 0.30 |
| 128 eilutės | 0.03 | 0.09 | 0.95 | 77.01 | 20.55 | 0.69 |
| 256 eilutės | 0.10 | 0.14 | 3.20 | 385.90 | 18.95 | 1.53 |
| 512 eilučių | 0.17 | 0.33 | 5.25 | 2179.06 | 40.08 | 4.61 |
| Visas failas | 0.27 | 0.44 | 7.44 | 7044.09 | 34.40 | 7.88 |

### Atsparumas kolizijoms

| Vieno string poroje ilgis | Nerijus | Nikita | Neda (v0.1final) | Miglė | Gustavo | Vanesa |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 10 | 0 | 0 | 0 | 0 | 0 | 0 |
| 100 | 0 | 0 | 0 | 0 | 0 | 0 |
| 500 | 0 | 0 | 0 | 0 | 0 | 0 |
| 1000 | 0 | 0 | 0 | 0 | 0 | 0 |

### Lavinos efektas - BIT skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Nerijus | 42.19% | 16.02% | 32.92% |
| Nikita | 42.97% | 0.00% | 32.23% |
| Miglė | 43.55% | 24.02% | 33.00% |
| Neda (v0.1final) | 42.21% | 12.00% | 31.24% |
| Gustavo | 44.53% | 0.00% | 30.65% |
| Vanesa | 43.36% | 0.00% | 23.27% |

### Lavanos efektas - HEX skirtumas

| Hash | Maksimalus skirtumas | Minimalus skirtumas | Vidutinis skirtumas |
|:---:|:---:|:---:|:---:|
| Nerijus | 100.00% | 53.12% | 93.50% |
| Nikita | 100.00% | 0.00% | 91.54% |
| Neda (v0.1final) | 100.00% | 1.56% | 82.62% |
| Miglė | 100.00% | 78.12% | 93.76% |
| Gustavo | 100.00% | 0.00% | 87.79% |
| Vanesa | 100.00% | 0.00% | 66.10% |

### Overall rezultatai

#### Bendras reitingas (pagal balus)

#### Vertinimo kriterijai:
- **Greitis:** ⭐⭐⭐⭐⭐ (<1ms), ⭐⭐⭐⭐ (1-5ms), ⭐⭐⭐ (5-10ms), ⭐⭐ (10-100ms), ⭐ (>100ms)
- **Lavinos BIT:** ⭐⭐⭐⭐⭐ (32-35%), ⭐⭐⭐⭐ (29-32%), ⭐⭐⭐ (26-29%), ⭐⭐ (23-26%), ⭐ (<23%)
- **Lavinos HEX:** ⭐⭐⭐⭐⭐ (>92%), ⭐⭐⭐⭐ (85-92%), ⭐⭐⭐ (75-85%), ⭐⭐ (65-75%), ⭐ (<65%)
- **Kolizijos:** ✅ = 0 kolizijų (100,000 testų)

| Vieta | Hash | Greitis | Lavinos BIT | Lavinos HEX | Kolizijos | Bendras balas |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 1 | **Nerijus** | ⭐⭐⭐⭐⭐ (0.27ms) | ⭐⭐⭐⭐ (32.92%) | ⭐⭐⭐⭐⭐ (93.50%) | ✅ (0) | **14/15** |
| 2 | **Nikita** | ⭐⭐⭐⭐⭐ (0.44ms) | ⭐⭐⭐⭐ (32.23%) | ⭐⭐⭐⭐ (91.54%) | ✅ (0) | **13/15** |
| 3 | **Neda** | ⭐⭐⭐ (7.44ms) | ⭐⭐⭐⭐ (31.24%) | ⭐⭐⭐⭐⭐ (85.62%) | ✅ (0) | **12/15** |
| 4 | **Miglė** | ⭐ (7044ms) | ⭐⭐⭐⭐⭐ (33.00%) | ⭐⭐⭐⭐⭐ (93.76%) | ✅ (0) | **11/15** |
| 5 | **Gustavo** | ⭐⭐ (nestabilus) | ⭐⭐⭐ (30.65%) | ⭐⭐⭐⭐ (87.79%) | ✅ (0) | **9/15** |
| 6 | **Vanesa** | ⭐⭐⭐ (7.88ms) | ⭐⭐ (23.27%) | ⭐⭐ (66.10%) | ✅ (0) | **7/15** |

---

## VERSIJOS

* **v0.1** – pradinė versija su testais (ilgis, deterministiškumas, efektyvumas, kolizijos, lavina, hiding).

* **v0.11** – pritaikyti 4 algoritmo roundai, optimizavimo vėliavėlė pakeista iš O2 į O3,implementuotas OpenMP į testus, pridėti papildomi du maišymai, kurie priklauso nuo kiekvieno elemento vertės.

* **v0.12** – įvairios eksperimentinės užduotys aprašytos `README.md`, seed generavimas su matrica kuri dauginama su gautais duomenim, patobulintas seed maišymas, optimizuotas hash generavimo algoritmas.

* **v0.1final** – galutinis own hasho versijos v0.1 readme su pseudo kodu. Išėmiau dalinimą į blokus, nes nelabai turi prasmės mano algoritme dabar jau. Įdėjau salt'ą.


* **v0.2** – AI patobulinimų versija. Integravau kriptografinius algoritmus: enhanced mixing su magic constants (0x9E3779B9, 0x85EBCA6B), bit rotation optimizacija naudojant C++20 std::rotl,"tThree-in-one mixer" patobulinimas - cascade efektas lavinos patobulinimui, Value-dependent shuffle - dinaminis maišymas pagal element vertes, 5-round processing - papildomas stabilumo raundas.

* **v0.1comparison** – naudoju v0.1final mano hash'o kodą palyginimui su SHA-256 ir kitų studentų hash'ais.