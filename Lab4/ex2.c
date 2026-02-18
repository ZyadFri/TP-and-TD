#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define N 1000

void init_matrix(int n, double *A) {
  for (int i=0; i<n; i++)
    for (int j=0; j<n; j++)
      A[i*n + j] = (double)(i + j);
}

void print_matrix(int n, double *A) {
  for (int i=0; i<n; i++) {
    for (int j=0; j<n; j++)
      printf("%6.1f ", A[i*n + j]);
    printf("\n");
  }
}

int main() {
  double *A = (double*)malloc(N*N*sizeof(double));
  double sum = 0.0;

  double start = omp_get_wtime();

  #pragma omp parallel shared(A) reduction(+:sum)
  {
   
    #pragma omp master
    init_matrix(N, A);


    #pragma omp barrier

   
    #pragma omp single
    {
      
      
    }

    
    #pragma omp for
    for (int i = 0; i < N*N; i++) {
      sum += A[i];
    }
  }

  double end = omp_get_wtime();

  printf("Sum = %f\n", sum);
  printf("Time (OpenMP) = %f s\n", end - start);

  free(A);
  return 0;
}
