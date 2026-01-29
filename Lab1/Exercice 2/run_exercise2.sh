#!/bin/bash

RESULTS_FILE="exercise2_results.txt"

# Function to output to both terminal and file
output() {
    echo "$1" | tee -a "$RESULTS_FILE"
}

# Clear previous results
> "$RESULTS_FILE"

output "========================================================================"
output "         Exercise 2: Optimizing Matrix Multiplication"
output "              Loop Order Performance Analysis"
output "========================================================================"
output ""
output "Date: $(date)"
output "System: $(uname -a)"
output ""

if command -v lscpu &> /dev/null; then
    output "========================================================================"
    output "                      SYSTEM CACHE INFORMATION"
    output "========================================================================"
    lscpu | grep -i cache | tee -a "$RESULTS_FILE"
    output ""
fi

output "========================================================================"
output "                          COMPILATION"
output "========================================================================"

# Compile standard version
output "Compiling mxm.c (standard ijk order)..."
gcc -O2 -o mxm mxm.c -lm 2>&1 | tee -a "$RESULTS_FILE"

if [ $? -ne 0 ]; then
    output "✗ Compilation of mxm.c failed!"
    exit 1
fi
output "✓ mxm.c compiled successfully!"
output ""

# Compile optimized version
output "Compiling mxm_optimized.c (all loop orders)..."
gcc -O2 -o mxm_optimized mxm_optimized.c -lm 2>&1 | tee -a "$RESULTS_FILE"

if [ $? -ne 0 ]; then
    output "✗ Compilation of mxm_optimized.c failed!"
    exit 1
fi
output "✓ mxm_optimized.c compiled successfully!"
output ""

# Test matrix sizes
MATRIX_SIZES=(256 512 1024)

output "========================================================================"
output "                  TESTING DIFFERENT MATRIX SIZES"
output "========================================================================"
output ""

for SIZE in "${MATRIX_SIZES[@]}"
do
    output ""
    output "========================================================================"
    output "                    MATRIX SIZE: ${SIZE}x${SIZE}"
    output "========================================================================"
    output ""
    
    output "--- Running Standard Version (ijk order) ---"
    ./mxm $SIZE 2>&1 | tee -a "$RESULTS_FILE"
    output ""
    
    output "--- Running Optimized Version (all loop orders) ---"
    ./mxm_optimized $SIZE 2>&1 | tee -a "$RESULTS_FILE"
    output ""
    output ""
done

output "========================================================================"
output "                         TESTING COMPLETE"
output "========================================================================"
output ""
output "Results saved to: $RESULTS_FILE"
output ""
output "Summary:"
output "- Tested matrix sizes: ${MATRIX_SIZES[@]}"
output "- Compared 6 loop permutations: ijk, ikj, jik, jki, kij, kji"
output "- Performance metrics: Time, Bandwidth, GFLOPS, Speedup"
output ""
output "Key Findings:"
output "- Loop order significantly impacts performance"
output "- ikj and kij orders typically perform best"
output "- Sequential memory access improves cache utilization"
output "- Speedup can be 2-10x depending on matrix size"
output "========================================================================"
output ""
output "Test completed at: $(date)"