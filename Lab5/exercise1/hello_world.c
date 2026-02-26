

#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* ---- Part 1: Every process prints Hello World ---- */
    printf("[Part 1] Hello World\n");

    /* Wait so Part 1 output is flushed before Part 2 */
    MPI_Barrier(MPI_COMM_WORLD);

    /* ---- Part 2: Each process prints its rank and total size ---- */
    printf("[Part 2] I am process %d out of %d total processes\n", rank, size);

    MPI_Barrier(MPI_COMM_WORLD);

    /* ---- Part 3: Only rank 0 prints ---- */
    if (rank == 0) {
        printf("[Part 3] Only process 0 is printing this message.\n");
    }

    /*
     * Part 4 - What happens without MPI_Finalize?
     *
     * If we omit MPI_Finalize(), the MPI environment is never properly shut down.
     * This means:
     *   - Internal MPI buffers may not be flushed, so some output might be lost.
     *   - MPI resources (communicators, memory, network connections) are never freed.
     *   - Some MPI implementations (like Open MPI) will print an error or warning
     *     about the missing finalize call.
     *   - The program may hang waiting for other processes to synchronize.
     *   - Behavior is technically UNDEFINED by the MPI standard.
     * In practice on Open MPI you often get a warning like:
     *   "It looks like MPI_Finalize was not called before MPI_COMM_WORLD was freed."
     */

    MPI_Finalize();
    return 0;
}
