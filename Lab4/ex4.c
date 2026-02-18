#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>

#ifndef N_COLS
#define N_COLS 40000   // n
#endif
#ifndef N_ROWS
#define N_ROWS 600     // m
#endif

static void init_data(int n, int m, double *rhs, double *lhs, double *mat) {
    for (int c = 0; c < n; c++) {
        rhs[c] = 1.0;
        for (int r = 0; r < m; r++) {
            mat[r + (size_t)m * c] = 1.0; // column-major
        }
    }
    for (int r = 0; r < m; r++) lhs[r] = 0.0;
}

/* Version 1: parallel for with implicit barrier */
static void dvmv_v1(int n, int m, double *lhs, const double *rhs, const double *mat) {
    #pragma omp parallel for
    for (int r = 0; r < m; r++) {
        double sum = 0.0;
        for (int c = 0; c < n; c++) {
            sum += mat[r + (size_t)m * c] * rhs[c];
        }
        lhs[r] = sum;
    } // implicit barrier
}

/* Version 2: dynamic + nowait inside a parallel region */
static void dvmv_v2(int n, int m, double *lhs, const double *rhs, const double *mat) {
    #pragma omp parallel
    {
        #pragma omp for schedule(dynamic) nowait
        for (int r = 0; r < m; r++) {
            double sum = 0.0;
            for (int c = 0; c < n; c++) {
                sum += mat[r + (size_t)m * c] * rhs[c];
            }
            lhs[r] = sum;
        }
        // no dependent code here; nowait mainly removes end-of-loop barrier
    }
}

/* Version 3: static + nowait inside a parallel region */
static void dvmv_v3(int n, int m, double *lhs, const double *rhs, const double *mat) {
    #pragma omp parallel
    {
        #pragma omp for schedule(static) nowait
        for (int r = 0; r < m; r++) {
            double sum = 0.0;
            for (int c = 0; c < n; c++) {
                sum += mat[r + (size_t)m * c] * rhs[c];
            }
            lhs[r] = sum;
        }
    }
}

static double checksum(int m, const double *lhs) {
    double s = 0.0;
    for (int r = 0; r < m; r++) s += lhs[r];
    return s;
}

int main(void) {
    const int n = N_COLS;
    const int m = N_ROWS;

    double *mat = (double*)malloc((size_t)m * (size_t)n * sizeof(double));
    double *rhs = (double*)malloc((size_t)n * sizeof(double));
    double *lhs = (double*)malloc((size_t)m * sizeof(double));
    if (!mat || !rhs || !lhs) { fprintf(stderr, "malloc failed\n"); return 1; }

    init_data(n, m, rhs, lhs, mat);

    double flops = 2.0 * (double)m * (double)n;

    // --- Version 1
    double t0 = omp_get_wtime();
    dvmv_v1(n, m, lhs, rhs, mat);
    double t1 = omp_get_wtime();
    double time_v1 = t1 - t0;
    double chk1 = checksum(m, lhs);

    // --- Version 2
    init_data(n, m, rhs, lhs, mat);
    double t2 = omp_get_wtime();
    dvmv_v2(n, m, lhs, rhs, mat);
    double t3 = omp_get_wtime();
    double time_v2 = t3 - t2;
    double chk2 = checksum(m, lhs);

    // --- Version 3
    init_data(n, m, rhs, lhs, mat);
    double t4 = omp_get_wtime();
    dvmv_v3(n, m, lhs, rhs, mat);
    double t5 = omp_get_wtime();
    double time_v3 = t5 - t4;
    double chk3 = checksum(m, lhs);

    int p = omp_get_max_threads();
    printf("Threads = %d\n", p);
    printf("V1 time = %.6f s, MFLOP/s = %.2f, checksum = %.2f\n",
           time_v1, (flops / (time_v1 * 1e6)), chk1);
    printf("V2 time = %.6f s, MFLOP/s = %.2f, checksum = %.2f\n",
           time_v2, (flops / (time_v2 * 1e6)), chk2);
    printf("V3 time = %.6f s, MFLOP/s = %.2f, checksum = %.2f\n",
           time_v3, (flops / (time_v3 * 1e6)), chk3);

    
    free(mat); free(rhs); free(lhs);
    return 0;
}