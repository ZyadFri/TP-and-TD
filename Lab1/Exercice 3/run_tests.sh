#!/bin/bash

# Output file
RESULTS_FILE="test_results.txt"

# Function to output to both terminal and file
output() {
    echo "$1" | tee -a "$RESULTS_FILE"
}

# Clear previous results file
> "$RESULTS_FILE"

output "========================================================================"
output "    Block Matrix Multiplication - Compilation and Testing Script"
output "========================================================================"
output ""
output "Date: $(date)"
output "System: $(uname -a)"
output ""

# Get cache information if available
if command -v lscpu &> /dev/null; then
    output "========================================================================"
    output "                      SYSTEM CACHE INFORMATION"
    output "========================================================================"
    lscpu | grep -i cache | tee -a "$RESULTS_FILE"
    output ""
fi

# Compilation
output "========================================================================"
output "                          COMPILATION"
output "========================================================================"
output "Compiling mxm_bloc.c with optimization level -O2..."
gcc -O2 -o mxm_block mxm_bloc.c -lm 2>&1 | tee -a "$RESULTS_FILE"

if [ $? -ne 0 ]; then
    output "✗ Compilation failed!"
    exit 1
fi

output "✓ Compilation successful!"
output ""

# Test different matrix sizes
MATRIX_SIZES=(256 512 1024)

output "========================================================================"
output "                  TESTING DIFFERENT MATRIX SIZES"
output "========================================================================"
output ""
output "Matrix sizes to test: ${MATRIX_SIZES[@]}"
output "Block sizes to test: 8, 16, 32, 64, 128, 256"
output ""

for SIZE in "${MATRIX_SIZES[@]}"
do
    output ""
    output "========================================================================"
    output "                    MATRIX SIZE: ${SIZE}x${SIZE}"
    output "========================================================================"
    output ""
    ./mxm_block $SIZE 2>&1 | tee -a "$RESULTS_FILE"
    output ""
    output ""
done

output "========================================================================"
output "                         TESTING COMPLETE"
output "========================================================================"
output ""
output "Summary:"
output "- Tested matrix sizes: ${MATRIX_SIZES[@]}"
output "- Block sizes tested: 8, 16, 32, 64, 128, 256"
output "- Optimal block size identified for each matrix size"
output ""
output "Results saved to: $RESULTS_FILE"
output ""
output "Next steps:"
output "1. Review the performance results in $RESULTS_FILE"
output "2. Identify the optimal block size for each matrix size"
output "3. Analyze why certain block sizes perform better"
output "4. Consider cache hierarchy (L1, L2, L3) in your analysis"
output "========================================================================"
output ""
output "Test completed at: $(date)"
