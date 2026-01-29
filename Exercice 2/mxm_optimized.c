#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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

// Standard matrix multiplication (ijk order) - ORIGINAL
void matrix_multiply_ijk(double **a, double **b, double **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Optimized matrix multiplication (ikj order) - CACHE OPTIMIZED
void matrix_multiply_ikj(double **a, double **b, double **c, int n) {
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double r = a[i][k];
            for (int j = 0; j < n; j++) {
                c[i][j] += r * b[k][j];
            }
        }
    }
}

// Alternative: jik order
void matrix_multiply_jik(double **a, double **b, double **c, int n) {
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            for (int k = 0; k < n; k++) {
                c[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// Alternative: kij order
void matrix_multiply_kij(double **a, double **b, double **c, int n) {
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            double r = a[i][k];
            for (int j = 0; j < n; j++) {
                c[i][j] += r * b[k][j];
            }
        }
    }
}

// Alternative: jki order
void matrix_multiply_jki(double **a, double **b, double **c, int n) {
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < n; k++) {
            double r = b[k][j];
            for (int i = 0; i < n; i++) {
                c[i][j] += a[i][k] * r;
            }
        }
    }
}

// Alternative: kji order
void matrix_multiply_kji(double **a, double **b, double **c, int n) {
    for (int k = 0; k < n; k++) {
        for (int j = 0; j < n; j++) {
            double r = b[k][j];
            for (int i = 0; i < n; i++) {
                c[i][j] += a[i][k] * r;
            }
        }
    }
}

// Function to calculate memory bandwidth
double calculate_bandwidth(int n, double time_sec) {
    double memory_accessed = 4.0 * n * n * n * sizeof(double);
    double bandwidth_bytes_per_sec = memory_accessed / time_sec;
    double bandwidth_gb_per_sec = bandwidth_bytes_per_sec / (1024.0 * 1024.0 * 1024.0);
    return bandwidth_gb_per_sec;
}

// Function to calculate GFLOPS
double calculate_gflops(int n, double time_sec) {
    double operations = 2.0 * n * n * n;
    double gflops = (operations / time_sec) / 1e9;
    return gflops;
}

// Function to measure execution time
typedef void (*multiply_func)(double**, double**, double**, int);

typedef struct {
    const char *name;
    multiply_func func;
    double time;
    double bandwidth;
    double gflops;
} LoopOrder;

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
    printf("     MATRIX MULTIPLICATION - LOOP ORDER OPTIMIZATION            \n");
    printf("=================================================================\n");
    printf("Matrix size: %d x %d\n", n, n);
    printf("Testing all 6 loop permutations: ijk, ikj, jik, jki, kij, kji\n");
    printf("=================================================================\n\n");
    
    // Seed random number generator
    srand(42); // Fixed seed for reproducibility
    
    // Allocate matrices
    printf("Allocating matrices...\n");
    double **a = allocate_matrix(n);
    double **b = allocate_matrix(n);
    double **c = allocate_matrix(n);
    
    // Initialize matrices
    printf("Initializing matrices...\n\n");
    initialize_matrix(a, n);
    initialize_matrix(b, n);
    
    // Define all loop orders
    LoopOrder orders[] = {
        {"ijk (standard)", matrix_multiply_ijk, 0, 0, 0},
        {"ikj (optimized)", matrix_multiply_ikj, 0, 0, 0},
        {"jik", matrix_multiply_jik, 0, 0, 0},
        {"jki", matrix_multiply_jki, 0, 0, 0},
        {"kij", matrix_multiply_kij, 0, 0, 0},
        {"kji", matrix_multiply_kji, 0, 0, 0}
    };
    
    int num_orders = sizeof(orders) / sizeof(orders[0]);
    
    printf("=================================================================\n");
    printf("                      PERFORMANCE RESULTS                        \n");
    printf("=================================================================\n\n");
    
    // Test each loop order
    for (int i = 0; i < num_orders; i++) {
        zero_matrix(c, n);
        
        printf("Testing %s...\n", orders[i].name);
        
        clock_t start = clock();
        orders[i].func(a, b, c, n);
        clock_t end = clock();
        
        orders[i].time = ((double)(end - start)) / CLOCKS_PER_SEC;
        orders[i].bandwidth = calculate_bandwidth(n, orders[i].time);
        orders[i].gflops = calculate_gflops(n, orders[i].time);
        
        printf("  Time: %.4f s | Bandwidth: %.2f GB/s | GFLOPS: %.2f\n\n",
               orders[i].time, orders[i].bandwidth, orders[i].gflops);
    }
    
    // Find best performer
    int best_idx = 0;
    for (int i = 1; i < num_orders; i++) {
        if (orders[i].time < orders[best_idx].time) {
            best_idx = i;
        }
    }
    
    printf("=================================================================\n");
    printf("                      SUMMARY TABLE                              \n");
    printf("=================================================================\n");
    printf("%-18s %12s %15s %12s %10s\n", 
           "Loop Order", "Time (s)", "Bandwidth (GB/s)", "GFLOPS", "Speedup");
    printf("-----------------------------------------------------------------\n");
    
    for (int i = 0; i < num_orders; i++) {
        double speedup = orders[0].time / orders[i].time;
        char marker = (i == best_idx) ? '*' : ' ';
        printf("%c %-16s %12.4f %15.2f %12.2f %9.2fx\n",
               marker, orders[i].name, orders[i].time, 
               orders[i].bandwidth, orders[i].gflops, speedup);
    }
    
    printf("=================================================================\n\n");
    
    printf("=================================================================\n");
    printf("                      ANALYSIS                                   \n");
    printf("=================================================================\n");
    printf("Best Loop Order: %s\n", orders[best_idx].name);
    printf("Best Time:       %.4f seconds\n", orders[best_idx].time);
    printf("Best Bandwidth:  %.2f GB/s\n", orders[best_idx].bandwidth);
    printf("Best GFLOPS:     %.2f\n", orders[best_idx].gflops);
    printf("Speedup vs ijk:  %.2fx\n", orders[0].time / orders[best_idx].time);
    printf("=================================================================\n\n");
    
    printf("Cache Efficiency Explanation:\n");
    printf("------------------------------\n");
    printf("ijk order: Poor cache locality for matrix B (stride-n access)\n");
    printf("ikj order: BEST - Sequential access to both B and C arrays\n");
    printf("jik order: Similar to ijk, poor locality for A and B\n");
    printf("jki order: Moderate performance\n");
    printf("kij order: Good locality, similar to ikj\n");
    printf("kji order: Poor locality for matrix A\n\n");
    
    printf("The '%s' order is optimal because:\n", orders[best_idx].name);
    printf("1. Inner loop (j) accesses B[k][j] and C[i][j] sequentially\n");
    printf("2. Sequential access maximizes cache line utilization\n");
    printf("3. A[i][k] is loaded once and reused for entire row of B\n");
    printf("4. Better spatial and temporal locality\n");
    printf("=================================================================\n");
    
    // Free matrices
    free_matrix(a, n);
    free_matrix(b, n);
    free_matrix(c, n);
    
    return EXIT_SUCCESS;
}
