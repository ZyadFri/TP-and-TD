#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define GENERATIONS 10
#define GLOBAL_NX   16
#define GLOBAL_NY   16

static int rank, nprocs;
static MPI_Comm cart_comm;
static int dims[2], coords[2];
static int north, south, east, west;
static int local_nx, local_ny;

static int IDX(int i, int j, int cols) { return i * cols + j; }

void init_grid(int *grid, int rows, int cols)
{
    srand(42 + rank);
    for (int i = 1; i <= rows; i++)
        for (int j = 1; j <= cols; j++)
            grid[IDX(i, j, cols + 2)] = rand() % 2;
}

void exchange_halos(int *grid, int rows, int cols)
{
    int stride = cols + 2;
    MPI_Datatype row_type, col_type;

    MPI_Type_contiguous(cols, MPI_INT, &row_type);
    MPI_Type_commit(&row_type);
    MPI_Type_vector(rows, 1, stride, MPI_INT, &col_type);
    MPI_Type_commit(&col_type);

    MPI_Sendrecv(&grid[IDX(1, 1, stride)], 1, row_type, north, 0,
                 &grid[IDX(rows + 1, 1, stride)], 1, row_type, south, 0,
                 cart_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&grid[IDX(rows, 1, stride)], 1, row_type, south, 1,
                 &grid[IDX(0, 1, stride)], 1, row_type, north, 1,
                 cart_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&grid[IDX(1, 1, stride)], 1, col_type, west, 2,
                 &grid[IDX(1, cols + 1, stride)], 1, col_type, east, 2,
                 cart_comm, MPI_STATUS_IGNORE);
    MPI_Sendrecv(&grid[IDX(1, cols, stride)], 1, col_type, east, 3,
                 &grid[IDX(1, 0, stride)], 1, col_type, west, 3,
                 cart_comm, MPI_STATUS_IGNORE);

    MPI_Type_free(&row_type);
    MPI_Type_free(&col_type);
}

void update(int *grid, int *new_grid, int rows, int cols)
{
    int stride = cols + 2;
    for (int i = 1; i <= rows; i++) {
        for (int j = 1; j <= cols; j++) {
            int live = grid[IDX(i-1,j-1,stride)] + grid[IDX(i-1,j,stride)] + grid[IDX(i-1,j+1,stride)]
                     + grid[IDX(i,  j-1,stride)]                            + grid[IDX(i,  j+1,stride)]
                     + grid[IDX(i+1,j-1,stride)] + grid[IDX(i+1,j,stride)] + grid[IDX(i+1,j+1,stride)];
            int cell = grid[IDX(i,j,stride)];
            if (cell == 1)
                new_grid[IDX(i,j,stride)] = (live == 2 || live == 3) ? 1 : 0;
            else
                new_grid[IDX(i,j,stride)] = (live == 3) ? 1 : 0;
        }
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    dims[0] = dims[1] = 0;
    MPI_Dims_create(nprocs, 2, dims);

    int periods[2] = {1, 1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);
    MPI_Cart_coords(cart_comm, rank, 2, coords);
    MPI_Cart_shift(cart_comm, 0, 1, &north, &south);
    MPI_Cart_shift(cart_comm, 1, 1, &west, &east);

    local_nx = GLOBAL_NX / dims[0];
    local_ny = GLOBAL_NY / dims[1];

    int stride = local_ny + 2;
    int size = (local_nx + 2) * stride;
    int *grid     = calloc(size, sizeof(int));
    int *new_grid = calloc(size, sizeof(int));

    init_grid(grid, local_nx, local_ny);

    for (int g = 0; g < GENERATIONS; g++) {
        exchange_halos(grid, local_nx, local_ny);
        update(grid, new_grid, local_nx, local_ny);
        int *tmp = grid; grid = new_grid; new_grid = tmp;
    }

    if (rank == 0) {
        printf("Rank 0 - Generation %d:\n", GENERATIONS);
        for (int i = 1; i <= local_nx; i++) {
            for (int j = 1; j <= local_ny; j++)
                printf("%d ", grid[IDX(i, j, stride)]);
            printf("\n");
        }
    }

    free(grid);
    free(new_grid);
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
