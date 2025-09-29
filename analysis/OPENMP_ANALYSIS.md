# OpenMP Analizės Ataskaita

## Eksperimento rezultatai:

### Performance palyginimas pagal thread skaičių:

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

## Išvados:

### ❌ **OpenMP NETINKA šiam algoritmui NET SU MAŽIAU THREADS**

**Aukso taisyklė**: Kuo daugiau threads, tuo BLOGIAU!

**Priežastys:**
1. **Thread creation overhead** - net 1 thread jau 3-7x lėtesnis
2. **Synchronization cost** - `#pragma omp critical` naikina viską
3. **Context switching** - OS turi perpjūti tarp threads
4. **Memory contention** - threads konkuruoja dėl cache
5. **Small workload** - darbo per mažai, kad apsimokėtų threads

### ✅ **Sequential versija - VISADA OPTIMALI**

**Net 1 OpenMP thread** yra lėtesnis nei pure sequential!

## Teorinis palyginimas:

- **2 threads**: +35x overhead vs +0x speedup = **neigiamas**
- **8 threads**: +71x overhead vs +0x speedup = **katastrofa**
- **24 threads**: +159x overhead vs +0x speedup = **nesąmonė**

## Rekomendacija:

**NIEKADA NENAUDOTI OpenMP** šiam algoritmui!

Hash funkcijos turi:
- Mažą computation/communication ratio
- Dažnius data dependencies
- Trumpus loops (64 iteracijos)
- Sekvencinę prigimtį

## Alternatyvos:

1. **SIMD (AVX2/AVX512)** - 4-8x greičiau be threads
2. **Algorithm optimization** - efektyvesni žingsniai  
3. **Memory prefetching** - cache optimizacijos
4. **Compiler intrinsics** - žemo lygio optimizacijos

**Išvada: OpenMP = Performance katastrofa visais atvejais!**