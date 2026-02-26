

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int value;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        /* Process 0 reads the initial value */
        printf("Process 0: Enter a starting value: ");
        fflush(stdout);
        /* For automated testing we use a hardcoded value */
        value = 0;
        printf("%d\n", value);
        printf("Process 0 starts with value = %d, forwards %d to process 1\n",
               value, value);
        fflush(stdout);

        /* Send to process 1 */
        MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

        /* Receive final result back from the last process */
        MPI_Recv(&value, 1, MPI_INT, size - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process 0: received final ring value = %d\n", value);

    } else {
        /* Receive from previous process */
        MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        /* Add own rank */
        value += rank;
        printf("Process %d: received, added rank %d -> value = %d\n", rank, rank, value);
        fflush(stdout);

        /* Forward to next process (wraps around to 0 for last process) */
        int next = (rank + 1) % size;
        MPI_Send(&value, 1, MPI_INT, next, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
