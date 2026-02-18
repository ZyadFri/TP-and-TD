#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main() {
    const int m = 1000;
    const int n = 1000;

    double *a = (double*) malloc((size_t)m * n * sizeof(double));
    double *b = (double*) malloc((size_t)n * m * sizeof(double));
    double *c = (double*) malloc((size_t)m * m * sizeof(double));

    if (!a || !b || !c) {
        printf("Allocation failed\n");
        return 1;
    }

    // Init A
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            a[i * n + j] = (i + 1) + (j + 1);

    // Init B
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            b[i * m + j] = (i + 1) - (j + 1);

    // (Optional) init C (not needed if we overwrite C with sum)
    // #pragma omp parallel for collapse(2)
    // for (int i = 0; i < m; i++)
    //     for (int j = 0; j < m; j++)
    //         c[i * m + j] = 0.0;

    // Print actual threads (once)
    #pragma omp parallel
    {
        #pragma omp single
        printf("Threads used: %d\n", omp_get_num_threads());
    }

    double start_time = omp_get_wtime();

    // Matmul
    #pragma omp parallel for collapse(2) schedule(runtime)
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * m + j];
            }
            c[i * m + j] = sum;
        }
    }

    double end_time = omp_get_wtime();

    printf("Matrix multiplication time: %f seconds\n", end_time - start_time);
    printf("Sample output: c[0][0] = %f\n", c[0]);

    free(a);
    free(b);
    free(c);
    return 0;
}
