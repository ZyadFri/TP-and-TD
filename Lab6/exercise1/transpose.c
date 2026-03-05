#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size < 2) {
        printf("Need at least 2 processes\n");
        MPI_Finalize();
        return 1;
    }

    if (rank == 0) {
        double a[4][5];
        for (int i = 0; i < 4; i++)
            for (int j = 0; j < 5; j++)
                a[i][j] = i * 5 + j + 1;

        printf("Process 0 - Matrix a:\n");
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 5; j++)
                printf("%d ", (int)a[i][j]);
            printf("\n");
        }

        MPI_Datatype col_type, trans_type;
        MPI_Type_vector(4, 1, 5, MPI_DOUBLE, &col_type);
        MPI_Type_commit(&col_type);
        MPI_Type_create_hvector(5, 1, sizeof(double), col_type, &trans_type);
        MPI_Type_commit(&trans_type);

        MPI_Send(a, 1, trans_type, 1, 0, MPI_COMM_WORLD);

        MPI_Type_free(&col_type);
        MPI_Type_free(&trans_type);

    } else if (rank == 1) {
        double at[5][4];
        MPI_Recv(at, 20, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("Process 1 - Matrix transposee at:\n");
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 4; j++)
                printf("%d ", (int)at[i][j]);
            printf("\n");
        }
    }

    MPI_Finalize();
    return 0;
}
