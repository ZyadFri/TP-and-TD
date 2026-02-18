#include <stdio.h>
#include <omp.h>

static long num_steps = 10000000;
double step;

int main() {
    double pi, sum = 0.0;
    step = 1.0 / (double) num_steps;

    double t0 = omp_get_wtime();

         T   = omp_get_num_threads();

        double local_sum = 0.0;

        for (long i = tid; i < num_steps; i += T) {
            double x = (i + 0.5) * step;
            local_sum += 4.0 / (1.0 + x * x);
        }

        #pragma omp atomic
        sum += local_sum;
    }

    pi = step * sum;

    double t1 = omp_get_wtime();

    printf("pi = %.15f\n", pi);
    printf("time = %f seconds\n", t1 - t0);

    return 0;
}
