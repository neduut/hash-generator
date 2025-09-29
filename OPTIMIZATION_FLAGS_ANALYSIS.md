# Compiler Optimization Flags Analizė

## Performance palyginimas pagal optimization flags:

### Rezultatai (μs per hash):

| Flag   | 10 chars | 100 chars | 500 chars | 1000 chars | Aprašymas |
|--------|----------|-----------|-----------|------------|-----------|
| **-O0** | 8.86     | 31.91     | 125.47    | 253.27     | No optimization |
| **-O1** | 2.26     | 5.56      | 16.30     | 25.59      | Basic optimization |
| **-O2** | 1.71     | 5.09      | 22.66     | 42.80      | Standard optimization |
| **-O3** | 1.32     | 4.46      | 15.38     | 31.43      | Aggressive optimization |
| **-Ofast** | 1.74  | 3.81      | 29.20     | 50.15      | Fast math optimizations |

## Speedup palyginimas su -O0:

| Flag   | 10 chars | 100 chars | 500 chars | 1000 chars | Avg Speedup |
|--------|----------|-----------|-----------|------------|-------------|
| **-O1** | 3.9x     | 5.7x      | 7.7x      | 9.9x       | **6.8x** |
| **-O2** | 5.2x     | 6.3x      | 5.5x      | 5.9x       | **5.7x** |
| **-O3** | 6.7x     | 7.2x      | 8.2x      | 8.1x       | **7.5x** |
| **-Ofast** | 5.1x  | 8.4x      | 4.3x      | 5.1x       | **5.7x** |

## Išvados:

### 🏆 **-O3 - GERIAUSIAS PASIRINKIMAS**
- **Vidutinis speedup**: 7.5x vs -O0
- **Konsistentas**: geras visoms input sizes
- **Agresyvūs optimizacijos**: loop unrolling, function inlining
- **Saugus**: nesugadina algoritmo deterministiškumo

### 🥈 **-O1 - ANTRA VIETA**
- **Vidutinis speedup**: 6.8x vs -O0  
- **Stabilus**: geras balansas tarp greičio ir kompiliavimo laiko
- **Patikimas**: bazinės optimizacijos be rizikos

### 🥉 **-O2 - TREČIA VIETA (current)**
- **Vidutinis speedup**: 5.7x vs -O0
- **Standartinis**: įprastas pasirinkimas production kod
- **Saugus**: plačiai naudojamas, patikimas

### ❌ **-Ofast - VENGTI**
- **Nepastovus**: kartais greitas, kartais lėtas
- **Rizikingas**: keičia floating point matematikos elgesį
- **Nenuspėjamas**: gali pakeisti algoritmo rezultatus

## Rekomendacijos:

### Produkcijai:
```makefile
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -O3
```

### Development:
```makefile
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -O1 -g
```

### Benchmarking:
```makefile
CXXFLAGS = -Iinclude -Wall -Wextra -std=c++17 -O3 -DNDEBUG
```

## Išvada:

**Pakeisti į -O3** - tai duos ~30% performance boost palyginus su current -O2!

Optimizacijos flag'ai turi **DIDELĘ įtaką** - skirtumas tarp -O0 ir -O3 yra **7.5x**!