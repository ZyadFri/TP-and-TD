
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            printf("Usage: %s <N>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    long long N = atoll(argv[1]);
    if (N <= 0) {
        if (rank == 0)
            printf("N must be positive.\n");
        MPI_Finalize();
        return 1;
    }

    
    long long base  = N / size;
    long long extra = N % size;

    long long local_start = rank * base + (rank < extra ? rank : extra);
    long long local_end   = local_start + base + (rank < extra ? 1 : 0);

    
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    double local_sum = 0.0;
    for (long long i = local_start; i < local_end; i++) {
        double x = (i + 0.5) / (double)N;
        local_sum += 1.0 / (1.0 + x * x);
    }


    double global_sum = 0.0;
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();
    double elapsed = t_end - t_start;

    if (rank == 0) {
        double pi = 4.0 * global_sum / (double)N;
        printf("N=%lld, processes=%d, pi=%.15f, error=%.2e, time=%.6f s\n",
               N, size, pi, fabs(pi - M_PI), elapsed);
    }

    MPI_Finalize();
    return 0;
}
