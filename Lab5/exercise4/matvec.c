

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

void matrixVectorMult(double *A, double *b, double *x, int N) {
    for (int i = 0; i < N; ++i) {
        x[i] = 0.0;
        for (int j = 0; j < N; ++j) {
            x[i] += A[i * N + j] * b[j];
        }
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 2) {
        if (rank == 0)
            printf("Usage: %s <matrix_size>\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    if (N <= 0) {
        if (rank == 0)
            printf("Matrix size must be positive.\n");
        MPI_Finalize();
        return 1;
    }

    int *sendcounts = malloc(size * sizeof(int));  /* rows per process */
    int *displs     = malloc(size * sizeof(int));  /* row offsets */
    int *recvcounts = malloc(size * sizeof(int));  /* elements per process */
    int *rdispls    = malloc(size * sizeof(int));  /* element offsets */

    int base_rows  = N / size;
    int extra_rows = N % size;
    int offset = 0;

    for (int i = 0; i < size; i++) {
        int rows = base_rows + (i < extra_rows ? 1 : 0);
        sendcounts[i] = rows * N;   /* number of doubles to scatter */
        recvcounts[i] = rows;       /* number of result elements to gather */
        displs[i]     = offset;
        rdispls[i]    = offset / N;
        offset       += rows * N;
    }

    int local_rows = base_rows + (rank < extra_rows ? 1 : 0);

    /* Allocate local arrays */
    double *local_A = malloc(local_rows * N * sizeof(double));
    double *b       = malloc(N * sizeof(double));
    double *local_x = malloc(local_rows * sizeof(double));

    double *A        = NULL;
    double *x_par    = NULL;
    double *x_serial = NULL;

    if (rank == 0) {
        A        = malloc((long)N * N * sizeof(double));
        x_par    = malloc(N * sizeof(double));
        x_serial = malloc(N * sizeof(double));

        srand48(42);
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++)
                A[i * N + j] = drand48();
        for (int i = 0; i < N; i++)
            b[i] = drand48();
    }

    /* ---- Timed region ---- */
    MPI_Barrier(MPI_COMM_WORLD);
    double t_start = MPI_Wtime();

    /* Broadcast vector b to all processes */
    MPI_Bcast(b, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    /* Scatter rows of A (each process gets local_rows rows) */
    MPI_Scatterv(A, sendcounts, displs, MPI_DOUBLE,
                 local_A, local_rows * N, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    /* Local matrix-vector multiplication */
    for (int i = 0; i < local_rows; i++) {
        local_x[i] = 0.0;
        for (int j = 0; j < N; j++)
            local_x[i] += local_A[i * N + j] * b[j];
    }

    /* Gather results back to rank 0 */
    MPI_Gatherv(local_x, local_rows, MPI_DOUBLE,
                x_par, recvcounts, rdispls, MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    double t_end = MPI_Wtime();
    double elapsed = t_end - t_start;

    if (rank == 0) {
        printf("N=%d, processes=%d, time=%.6f s\n", N, size, elapsed);

        /* Verify correctness */
        matrixVectorMult(A, b, x_serial, N);
        double max_err = 0.0;
        for (int i = 0; i < N; i++) {
            double diff = fabs(x_par[i] - x_serial[i]);
            if (diff > max_err) max_err = diff;
        }
        printf("Max error vs serial: %e\n", max_err);

        free(A);
        free(x_par);
        free(x_serial);
    }

    free(local_A);
    free(b);
    free(local_x);
    free(sendcounts);
    free(displs);
    free(recvcounts);
    free(rdispls);

    MPI_Finalize();
    return 0;
}
