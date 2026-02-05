#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1000000
#define ITERATIONS 100  // Multiple iterations for better timing accuracy

int main() {
    double *a = malloc(N * sizeof(double));
    volatile double sum = 0.0;
    double start, end;
    int i, iter;
    
    // Initialize array
    for (i = 0; i < N; i++)
        a[i] = 1.0;
    
    // Choose your unroll factor here by uncommenting one section:
    
    // ========== U = 1 (Baseline) ==========
   
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i++) {
            sum += a[i];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=1: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
  
    
    // ========== U = 2 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 2) {
            sum += a[i] + a[i+1];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=2: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 3 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 3) {
            sum += a[i] + a[i+1] + a[i+2];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=3: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 4 ==========
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 4) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=4: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    
    // ========== U = 5 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 5) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + a[i+4];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=5: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 6 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 6) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + a[i+4] + a[i+5];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=6: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 7 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 7) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + a[i+4] + a[i+5] + a[i+6];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=7: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 8 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 8) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + 
                   a[i+4] + a[i+5] + a[i+6] + a[i+7];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=8: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 16 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 16) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + 
                   a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                   a[i+8] + a[i+9] + a[i+10] + a[i+11] + 
                   a[i+12] + a[i+13] + a[i+14] + a[i+15];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=16: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    // ========== U = 32 ==========
    /*
    start = (double)clock() / CLOCKS_PER_SEC;
    for (iter = 0; iter < ITERATIONS; iter++) {
        sum = 0.0;
        for (i = 0; i < N; i += 32) {
            sum += a[i] + a[i+1] + a[i+2] + a[i+3] + 
                   a[i+4] + a[i+5] + a[i+6] + a[i+7] +
                   a[i+8] + a[i+9] + a[i+10] + a[i+11] + 
                   a[i+12] + a[i+13] + a[i+14] + a[i+15] +
                   a[i+16] + a[i+17] + a[i+18] + a[i+19] + 
                   a[i+20] + a[i+21] + a[i+22] + a[i+23] +
                   a[i+24] + a[i+25] + a[i+26] + a[i+27] + 
                   a[i+28] + a[i+29] + a[i+30] + a[i+31];
        }
    }
    end = (double)clock() / CLOCKS_PER_SEC;
    printf("U=32: Sum = %f, Time = %f ms\n", sum, (end - start) * 1000 / ITERATIONS);
    */
    
    free(a);
    return 0;
}