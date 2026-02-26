
#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int value;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (1) {
        /* Only rank 0 reads input */
        if (rank == 0) {
            scanf("%d", &value);
        }

        /* Broadcast the value from rank 0 to all other processes */
        MPI_Bcast(&value, 1, MPI_INT, 0, MPI_COMM_WORLD);

        /* If the value is negative, stop */
        if (value < 0) {
            if (rank == 0) {
                printf("Negative value received (%d). Stopping.\n", value);
            }
            break;
        }

        /* Every process prints its rank and the received value */
        printf("Process %d got %d\n", rank, value);

        /* Wait for all processes before reading next input */
        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
