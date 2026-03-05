#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#define N_FEATURES 5
#define N_SAMPLES  1000
#define MAX_EPOCHS 10000
#define LR         0.01
#define THRESHOLD  1.0e-2

typedef struct {
    double x[N_FEATURES];
    double y;
} Sample;

void generate_data(Sample *data, int n) {
    double true_w[N_FEATURES] = {2.0, -1.0, 3.0, 0.5, -2.0};
    srand(42);
    for (int i = 0; i < n; i++) {
        double y = 0.0;
        for (int j = 0; j < N_FEATURES; j++) {
            data[i].x[j] = ((double)rand() / RAND_MAX) * 2.0 - 1.0;
            y += true_w[j] * data[i].x[j];
        }
        data[i].y = y + ((double)rand() / RAND_MAX - 0.5) * 0.1;
    }
}

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Datatype sample_type;
    int          blocklengths[2] = {N_FEATURES, 1};
    MPI_Aint     displacements[2];
    MPI_Datatype types[2] = {MPI_DOUBLE, MPI_DOUBLE};

    Sample dummy;
    MPI_Aint base_addr;
    MPI_Get_address(&dummy,       &base_addr);
    MPI_Get_address(&dummy.x[0],  &displacements[0]);
    MPI_Get_address(&dummy.y,     &displacements[1]);
    displacements[0] -= base_addr;
    displacements[1] -= base_addr;

    MPI_Type_create_struct(2, blocklengths, displacements, types, &sample_type);
    MPI_Type_commit(&sample_type);

    Sample *dataset = NULL;
    if (rank == 0) {
        dataset = (Sample *)malloc(N_SAMPLES * sizeof(Sample));
        generate_data(dataset, N_SAMPLES);
    }

    int local_n = N_SAMPLES / size;
    Sample *local_data = (Sample *)malloc(local_n * sizeof(Sample));
    MPI_Scatter(dataset, local_n, sample_type,
                local_data, local_n, sample_type,
                0, MPI_COMM_WORLD);

    /* --- 4-8. Gradient descent loop --- */
    double weights[N_FEATURES];
    for (int j = 0; j < N_FEATURES; j++) weights[j] = 0.0;

    double start = MPI_Wtime();
    int converged = 0;

    for (int epoch = 1; epoch <= MAX_EPOCHS && !converged; epoch++) {
        double local_grad[N_FEATURES];
        double local_mse = 0.0;
        for (int j = 0; j < N_FEATURES; j++) local_grad[j] = 0.0;

        /* 4. Each process computes local gradient and loss */
        for (int i = 0; i < local_n; i++) {
            double pred = 0.0;
            for (int j = 0; j < N_FEATURES; j++)
                pred += weights[j] * local_data[i].x[j];
            double err = pred - local_data[i].y;
            local_mse += err * err;
            for (int j = 0; j < N_FEATURES; j++)
                local_grad[j] += err * local_data[i].x[j];
        }

        /* 5. Aggregate gradients and losses across all processes */
        double global_grad[N_FEATURES], global_mse;
        MPI_Allreduce(local_grad, global_grad, N_FEATURES, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(&local_mse, &global_mse, 1,           MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        global_mse /= N_SAMPLES;

        /* 6. All processes update weight vector synchronously */
        for (int j = 0; j < N_FEATURES; j++)
            weights[j] -= LR * global_grad[j] / N_SAMPLES;

        /* 7. Print every 10 epochs from process 0 */
        if (rank == 0 && epoch % 10 == 0) {
            printf("Epoch %d | Loss (MSE): %f | w[0]: %.4f, w[1]: %.4f\n",
                   epoch, global_mse, weights[0], weights[1]);
        }

        /* 8. Early stopping */
        if (global_mse < THRESHOLD) {
            if (rank == 0)
                printf("Early stopping at epoch %d - loss %f < %.1e\n",
                       epoch, global_mse, THRESHOLD);
            converged = 1;
        }
    }

    double elapsed = MPI_Wtime() - start;
    if (rank == 0)
        printf("Training time: %.3f seconds (MPI)\n", elapsed);

    free(local_data);
    if (rank == 0) free(dataset);
    MPI_Type_free(&sample_type);
    MPI_Finalize();
    return 0;
}
