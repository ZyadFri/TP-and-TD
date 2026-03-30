#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NTX 200
#define NTY 200
#define MAX_ITER 5000
#define TOLERANCE 1e-6

int ntx, nty;
int sx, ex, sy, ey;

#include "compute.c"

int main(int argc, char **argv)
{
    int rank, nprocs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    ntx = NTX;
    nty = NTY;

    int dims[2] = {0, 0};
    MPI_Dims_create(nprocs, 2, dims);

    int periods[2] = {0, 0};
    MPI_Comm cart_comm;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cart_comm);

    int coords[2];
    MPI_Cart_coords(cart_comm, rank, 2, coords);

    int north, south, east, west;
    MPI_Cart_shift(cart_comm, 0, 1, &north, &south);
    MPI_Cart_shift(cart_comm, 1, 1, &west, &east);

    int local_nx = ntx / dims[0];
    int local_ny = nty / dims[1];

    sx = coords[0] * local_nx + 1;
    ex = sx + local_nx - 1;
    sy = coords[1] * local_ny + 1;
    ey = sy + local_ny - 1;

    double *u, *u_new, *u_exact;
    initialization(&u, &u_new, &u_exact);

    int stride = ey - sy + 3;

    MPI_Datatype row_type, col_type;
    MPI_Type_contiguous(local_ny, MPI_DOUBLE, &row_type);
    MPI_Type_commit(&row_type);
    MPI_Type_vector(local_nx, 1, stride, MPI_DOUBLE, &col_type);
    MPI_Type_commit(&col_type);

    double t_start = MPI_Wtime();
    double global_res = 1.0;
    int iter = 0;

    while (global_res > TOLERANCE && iter < MAX_ITER) {
        MPI_Sendrecv(&u[IDX(sx,   sy)], 1, row_type, north, 0,
                     &u[IDX(ex+1, sy)], 1, row_type, south, 0,
                     cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&u[IDX(ex,   sy)], 1, row_type, south, 1,
                     &u[IDX(sx-1, sy)], 1, row_type, north, 1,
                     cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&u[IDX(sx, sy  )], 1, col_type, west, 2,
                     &u[IDX(sx, ey+1)], 1, col_type, east, 2,
                     cart_comm, MPI_STATUS_IGNORE);
        MPI_Sendrecv(&u[IDX(sx, ey  )], 1, col_type, east, 3,
                     &u[IDX(sx, sy-1)], 1, col_type, west, 3,
                     cart_comm, MPI_STATUS_IGNORE);

        compute(u, u_new);

        double local_res = 0.0;
        for (int i = sx; i <= ex; i++)
            for (int j = sy; j <= ey; j++) {
                double diff = u_new[IDX(i,j)] - u[IDX(i,j)];
                local_res += diff * diff;
            }

        double *tmp = u; u = u_new; u_new = tmp;

        MPI_Allreduce(&local_res, &global_res, 1, MPI_DOUBLE, MPI_SUM, cart_comm);
        global_res = sqrt(global_res);
        iter++;
    }

    double t_end = MPI_Wtime();

    if (rank == 0)
        printf("%d %.6f %d\n", nprocs, t_end - t_start, iter);

    MPI_Type_free(&row_type);
    MPI_Type_free(&col_type);
    free(u); free(u_new); free(u_exact);
    MPI_Comm_free(&cart_comm);
    MPI_Finalize();
    return 0;
}
