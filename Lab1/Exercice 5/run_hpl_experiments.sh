#!/bin/bash

# HPL Benchmark Automation Script - FIXED VERSION
# This script runs 36 experiments: 4 matrix sizes × 9 block sizes

HPL_DIR="/home/lenovo/ex5/hpl-2.3/bin/Linux"
RESULTS_FILE="hpl_results.csv"
OUTPUT_DIR="hpl_outputs"

# Matrix sizes to test
N_VALUES=(1000 5000 10000 20000)

# Block sizes to test
NB_VALUES=(1 2 4 8 16 32 64 128 256)

cd $HPL_DIR

# Create output directory for logs
mkdir -p $OUTPUT_DIR

# Create CSV header
echo "N,NB,Time(s),GFLOPS,Status" > $RESULTS_FILE

echo "======================================"
echo "Starting HPL Benchmark Experiments"
echo "Total runs: 36 (4 N × 9 NB)"
echo "======================================"
echo ""

run_number=1

for N in "${N_VALUES[@]}"; do
    for NB in "${NB_VALUES[@]}"; do
        echo "Run $run_number/36: N=$N, NB=$NB"
        
        # Modify HPL.dat
        cat > HPL.dat << EOF
HPLinpack benchmark input file
Innovative Computing Laboratory, University of Tennessee
HPL.out      output file name (if any)
6            device out (6=stdout,7=stderr,file)
1            # of problems sizes (N)
$N           Ns
1            # of NBs
$NB          NBs
0            PMAP process mapping (0=Row-,1=Column-major)
1            # of process grids (P x Q)
1            Ps
1            Qs
16.0         threshold
1            # of panel fact
2            PFACTs (0=left, 1=Crout, 2=Right)
1            # of recursive stopping criterium
4            NBMINs (>= 1)
1            # of panels in recursion
2            NDIVs
1            # of recursive panel fact.
1            RFACTs (0=left, 1=Crout, 2=Right)
1            # of broadcast
0            BCASTs (0=1rg,1=1rM,2=2rg,3=2rM,4=Lng,5=LnM)
1            # of lookahead depth
0            DEPTHs (>=0)
2            SWAP (0=bin-exch,1=long,2=mix)
64           swapping threshold
0            L1 in (0=transposed,1=no-transposed) form
0            U  in (0=transposed,1=no-transposed) form
1            Equilibration (0=no,1=yes)
8            memory alignment in double (> 0)
EOF
        
        # Run HPL and save full output
        OUTPUT_FILE="$OUTPUT_DIR/run_N${N}_NB${NB}.log"
        mpirun -np 1 ./xhpl > "$OUTPUT_FILE" 2>&1
        
        # Extract results - Look for the line starting with WR or containing the performance data
        # The output format is: WR11C2R4  N  NB  P  Q  Time  Gflops
        RESULT_LINE=$(grep -E "^WR|^WC" "$OUTPUT_FILE" | tail -1)
        
        if [ -n "$RESULT_LINE" ]; then
            # Extract time and GFLOPS using awk
            TIME=$(echo "$RESULT_LINE" | awk '{print $6}')
            GFLOPS=$(echo "$RESULT_LINE" | awk '{print $7}')
        else
            TIME="ERROR"
            GFLOPS="ERROR"
        fi
        
        # Check for PASSED/FAILED
        if grep -q "PASSED" "$OUTPUT_FILE"; then
            STATUS="PASSED"
        elif grep -q "FAILED" "$OUTPUT_FILE"; then
            STATUS="FAILED"
        else
            STATUS="UNKNOWN"
        fi
        
        # Save to CSV
        echo "$N,$NB,$TIME,$GFLOPS,$STATUS" >> $RESULTS_FILE
        
        echo "  Time: $TIME s, Performance: $GFLOPS GFLOPS, Status: $STATUS"
        echo ""
        
        run_number=$((run_number + 1))
    done
done

echo "======================================"
echo "All experiments completed!"
echo "Results saved to: $RESULTS_FILE"
echo "Individual logs saved to: $OUTPUT_DIR/"
echo "======================================"

# Display summary
echo ""
echo "Summary of Results:"
echo "-------------------"
column -t -s, $RESULTS_FILE

echo ""
echo "You can now run: python3 analyze_hpl_results.py"