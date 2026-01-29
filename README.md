Lab exercises on memory optimization and cache performance.

## What's Inside

- **Exercice 1/** - Memory stride analysis (how access patterns affect performance)
- **Exercice 2/** - Matrix multiplication loop optimization (3.4x speedup from reordering loops)
- **Exercice 3/** - Block matrix multiplication (finding optimal block sizes)
- **Exercice 4/** - Memory leak detection with Valgrind
- **Exercice 5/** - HPL benchmark testing

## Quick Results

- Exercise 1: Sequential access is 6x faster than strided
- Exercise 2: ikj loop order beats standard ijk by 3.43x
- Exercise 3: Block size 32 works best for my system
- Exercise 4: Fixed 2 memory leaks (40 bytes total)
- Exercise 5: Got 52.84 GFLOPS at 88.1% efficiency



## Files

- `TP1_Brief_Report.tex` - Quick summary of all exercises
- Each exercise folder has detailed analysis and code

## System Used

- Intel Core i7-1255U
- 16GB RAM
- GCC with -O2 optimization
- Ubuntu on WSL2
