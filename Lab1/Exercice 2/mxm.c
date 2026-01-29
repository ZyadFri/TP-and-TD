#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

// Function to zero-initialize a matrix
void zero_matrix(double **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = 0.0;
        }
    }
}

// Standard matrix multiplication (ijk order) - ORIGINAL VERSION
void matrix_multiply_ijk(double **a, double **b, double **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Function to calculate memory bandwidth
double calculate_bandwidth(int n, double time_sec) {
    // Memory accesses: reading A (n^3), reading B (n^3), reading+writing C (2*n^3)
    // Total: 4*n^3 * sizeof(double)
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

int main(int argc, char *argv[]) {
    int n = N;
    
    // Parse command-line arguments
    if (argc > 1) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "Invalid matrix size\n");
            return EXIT_FAILURE;
        }
    }
    
    printf("=================================================================\n");
    printf("     STANDARD MATRIX MULTIPLICATION (ijk order)                 \n");
    printf("=================================================================\n");
    printf("Matrix size: %d x %d\n", n, n);
    printf("Loop order: i-j-k (standard)\n");
    printf("=================================================================\n\n");
    
    // Seed random number generator
    srand(time(NULL));
    
    // Allocate matrices
    printf("Allocating matrices...\n");
    double **a = allocate_matrix(n);
    double **b = allocate_matrix(n);
    double **c = allocate_matrix(n);
    
    // Initialize matrices
    printf("Initializing matrices...\n");
    initialize_matrix(a, n);
    initialize_matrix(b, n);
    zero_matrix(c, n);
    
    printf("Starting matrix multiplication (ijk order)...\n\n");
    
    // Measure time
    clock_t start = clock();
    matrix_multiply_ijk(a, b, c, n);
    clock_t end = clock();
    
    double time_sec = ((double)(end - start)) / CLOCKS_PER_SEC;
    double bandwidth = calculate_bandwidth(n, time_sec);
    double gflops = calculate_gflops(n, time_sec);
    
    // Print results
    printf("=================================================================\n");
    printf("                      PERFORMANCE RESULTS                        \n");
    printf("=================================================================\n");
    printf("Loop Order:      ijk (standard)\n");
    printf("Execution Time:  %.4f seconds\n", time_sec);
    printf("Bandwidth:       %.2f GB/s\n", bandwidth);
    printf("Performance:     %.2f GFLOPS\n", gflops);
    printf("=================================================================\n\n");
    
    // Print sample results for verification
    printf("Sample result (first element): c[0][0] = %.4f\n\n", c[0][0]);
    
    // Free matrices
    free_matrix(a, n);
    free_matrix(b, n);
    free_matrix(c, n);
    
    return EXIT_SUCCESS;
}