#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 1000000

int main() {
    double *A = malloc(N * sizeof(double));
    if (!A) return 1;

    srand(0);
    for (int i = 0; i < N; i++)
        A[i] = (double)rand() / RAND_MAX;

    double sum = 0.0;
    double sumsq = 0.0;   
    double maxv = A[0];
    double mean = 0.0, stddev = 0.0;

    #pragma omp parallel shared(A,sum,sumsq,maxv,mean,stddev)
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                double s = 0.0;
                for (int i = 0; i < N; i++) s += A[i];
                sum = s;
            }

            #pragma omp section
            {
                double m = A[0];
                for (int i = 1; i < N; i++) if (A[i] > m) m = A[i];
                maxv = m;
            }

            #pragma omp section
            {
                double q = 0.0;
                for (int i = 0; i < N; i++) q += A[i] * A[i];
                sumsq = q;
            }
        } 

        #pragma omp single
        {
            mean = sum / N;
            double var = (sumsq / N) - mean * mean; 
            if (var < 0) var = 0; 
            stddev = sqrt(var);
        }
    }

    printf("Sum = %f\n", sum);
    printf("Max = %f\n", maxv);
    printf("StdDev = %f\n", stddev);

    free(A);
    return 0;
}
