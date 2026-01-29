#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Matrix size (default)
#ifndef N
#define N 1024
#endif

// Function to allocate a matrix
double** allocate_matrix(int n) {
    double **matrix = (double**)malloc(n * sizeof(double*));
    if (!matrix) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)malloc(n * sizeof(double));
        if (!matrix[i]) {
            fprintf(stderr, "Memory allocation failed\n");
            exit(EXIT_FAILURE);
        }
    }
    return matrix;
}

// Function to free a matrix
void free_matrix(double **matrix, int n) {
    if (!matrix) return;
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Function to initialize a matrix with random values
void initialize_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = (double)(rand() % 100) / 10.0;
        }
    }
}

// Function to initialize a matrix to zero
void zero_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 0.0;
        }
    }
}

// Standard matrix multiplication (for verification)
void matrix_multiply_standard(double **A, double **B, double **C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0.0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// Block matrix multiplication
void matrix_multiply_block(double **A, double **B, double **C, int n, int block_size) {
    // Initialize result matrix to zero
    zero_matrix(C, n);
    
    // Iterate over blocks
    for (int i0 = 0; i0 < n; i0 += block_size) {
        for (int j0 = 0; j0 < n; j0 += block_size) {
            for (int k0 = 0; k0 < n; k0 += block_size) {
                
                // Multiply blocks
                // Process elements within the current block
                for (int i = i0; i < i0 + block_size && i < n; i++) {
                    for (int j = j0; j < j0 + block_size && j < n; j++) {
                        double sum = C[i][j];
                        for (int k = k0; k < k0 + block_size && k < n; k++) {
                            sum += A[i][k] * B[k][j];
                        }
                        C[i][j] = sum;
                    }
                }
            }
        }
    }
}

// Function to verify if two matrices are equal (within tolerance)
int verify_matrices(double **C1, double **C2, int n) {
    double tolerance = 1e-6;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (fabs(C1[i][j] - C2[i][j]) > tolerance) {
                printf("Mismatch at [%d][%d]: %f vs %f\n", i, j, C1[i][j], C2[i][j]);
                return 0;
            }
        }
    }
    return 1;
}

// Function to measure execution time
double measure_time(void (*func)(double**, double**, double**, int, int),
                   double **A, double **B, double **C, int n, int block_size) {
    clock_t start, end;
    start = clock();
    func(A, B, C, n, block_size);
    end = clock();
    return ((double)(end - start)) / CLOCKS_PER_SEC;
}

// Function to calculate memory bandwidth
double calculate_bandwidth(int n, double time_sec) {
    // Number of floating-point operations: 2*n^3 (n^3 multiplications + n^3 additions)
    // Memory accesses: reading A (n^3), reading B (n^3), reading+writing C (2*n^3)
    // Total memory accesses: 4*n^3 * sizeof(double)
    double memory_accessed = 4.0 * n * n * n * sizeof(double);
    double bandwidth_bytes_per_sec = memory_accessed / time_sec;
    double bandwidth_gb_per_sec = bandwidth_bytes_per_sec / (1024.0 * 1024.0 * 1024.0);
    return bandwidth_gb_per_sec;
}

// Function to calculate GFLOPS
double calculate_gflops(int n, double time_sec) {
    // Matrix multiplication: 2*n^3 operations (n^3 multiplications + n^3 additions)
    double operations = 2.0 * n * n * n;
    double gflops = (operations / time_sec) / 1e9;
    return gflops;
}

// Function to print performance results
void print_results(int n, int block_size, double time_sec) {
    double bandwidth = calculate_bandwidth(n, time_sec);
    double gflops = calculate_gflops(n, time_sec);
    
    printf("Block Size: %4d | Time: %8.4f s | Bandwidth: %8.2f GB/s | GFLOPS: %8.2f\n",
           block_size, time_sec, bandwidth, gflops);
}

int main(int argc, char *argv[]) {
    int n = N;
    int block_sizes[] = {8, 16, 32, 64, 128, 256};
    int num_block_sizes = sizeof(block_sizes) / sizeof(block_sizes[0]);
    
    // Parse command-line arguments
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Invalid matrix size\n");
            return EXIT_FAILURE;
        }
    }
    
    printf("=================================================================\n");
    printf("         BLOCK MATRIX MULTIPLICATION PERFORMANCE ANALYSIS        \n");
    printf("=================================================================\n");
    printf("Matrix size: %d x %d\n", n, n);
    printf("=================================================================\n\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Allocate matrices
    printf("Allocating matrices...\n");
    double **A = allocate_matrix(n);
    double **B = allocate_matrix(n);
    double **C = allocate_matrix(n);
    double **C_verify = allocate_matrix(n);
    
    // Initialize matrices
    printf("Initializing matrices...\n");
    initialize_matrix(A, n);
    initialize_matrix(B, n);
    
    printf("\n=================================================================\n");
    printf("                    PERFORMANCE RESULTS                          \n");
    printf("=================================================================\n\n");
    
    // Store best performance
    double best_time = 1e9;
    int best_block_size = 0;
    double best_bandwidth = 0.0;
    double best_gflops = 0.0;
    
    // Test different block sizes
    for (int i = 0; i < num_block_sizes; i++) {
        int block_size = block_sizes[i];
        
        // Skip if block size is larger than matrix size
        if (block_size > n) {
            printf("Block Size: %4d | SKIPPED (larger than matrix size)\n", block_size);
            continue;
        }
        
        // Measure time for block multiplication
        zero_matrix(C, n);
        double time_sec = measure_time(matrix_multiply_block, A, B, C, n, block_size);
        
        // Print results
        print_results(n, block_size, time_sec);
        
        // Track best performance
        if (time_sec < best_time) {
            best_time = time_sec;
            best_block_size = block_size;
            best_bandwidth = calculate_bandwidth(n, time_sec);
            best_gflops = calculate_gflops(n, time_sec);
        }
        
        // Verify correctness for first iteration only
        if (i == 0) {
            printf("\nVerifying correctness...\n");
            matrix_multiply_standard(A, B, C_verify, n);
            if (verify_matrices(C, C_verify, n)) {
                printf("✓ Verification PASSED: Block multiplication is correct\n\n");
            } else {
                printf("✗ Verification FAILED: Results do not match\n\n");
            }
        }
    }
    
    // Print summary
    printf("\n=================================================================\n");
    printf("                      PERFORMANCE SUMMARY                        \n");
    printf("=================================================================\n");
    printf("Matrix Size:         %d x %d\n", n, n);
    printf("Optimal Block Size:  %d\n", best_block_size);
    printf("Best Time:           %.4f seconds\n", best_time);
    printf("Best Bandwidth:      %.2f GB/s\n", best_bandwidth);
    printf("Best Performance:    %.2f GFLOPS\n", best_gflops);
    printf("=================================================================\n\n");
    
    // Analysis explanation
    printf("=================================================================\n");
    printf("                    PERFORMANCE ANALYSIS                         \n");
    printf("=================================================================\n");
    printf("Why Block Size %d is Optimal:\n\n", best_block_size);
    printf("1. Cache Utilization:\n");
    printf("   - Block size %d fits well in L1/L2 cache\n", best_block_size);
    printf("   - Memory required per block: %.2f KB\n", 
           (3.0 * best_block_size * best_block_size * sizeof(double)) / 1024.0);
    printf("   - Reduces cache misses by reusing data in cache\n\n");
    
    printf("2. Memory Access Pattern:\n");
    printf("   - Block multiplication accesses memory in contiguous chunks\n");
    printf("   - Better spatial locality improves cache hit rate\n");
    printf("   - Reduces main memory accesses\n\n");
    
    printf("3. TLB (Translation Lookaside Buffer) Efficiency:\n");
    printf("   - Smaller working set reduces TLB misses\n");
    printf("   - Improves virtual-to-physical address translation\n\n");
    
    printf("4. Trade-offs:\n");
    printf("   - Too small: High loop overhead, poor cache utilization\n");
    printf("   - Too large: Exceeds cache capacity, causes thrashing\n");
    printf("   - Optimal: Balances cache usage and loop overhead\n");
    printf("=================================================================\n");
    
    // Free matrices
    free_matrix(A, n);
    free_matrix(B, n);
    free_matrix(C, n);
    free_matrix(C_verify, n);
    
    return EXIT_SUCCESS;
}