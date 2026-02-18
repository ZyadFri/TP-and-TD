#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

/* Range versions so we can split work safely */
static double task_light_range(int start, int end) {
    double x = 0.0;
    for (int i = start; i < end; i++) {
        x += sin(i * 0.001);
    }
    return x;
}

static double task_moderate_range(int start, int end) {
    double x = 0.0;
    for (int i = start; i < end; i++) {
        x += sqrt(i * 0.5) * cos(i * 0.001);
    }
    return x;
}

static double task_heavy_range(int start, int end) {
    double x = 0.0;
    for (int i = start; i < end; i++) {
        x += sqrt(i * 0.5) * cos(i * 0.001) * sin(i * 0.0001);
    }
    return x;
}

int main(int argc, char** argv) {
    int N = 2000000;             
    if (argc > 1) N = atoi(argv[1]);

    omp_set_num_threads(4);

   
    int light_start = 0, light_end = N;
    int mod_start   = 0, mod_end   = 5 * N;

    int h0 = 0;
    int h1 = 7 * N;
    int h2 = 14 * N;
    int h3 = 20 * N;

    double result = 0.0;

    double t0 = omp_get_wtime();

    #pragma omp parallel sections reduction(+:result)
    {
        #pragma omp section
        {
            result += task_light_range(light_start, light_end);
            result += task_moderate_range(mod_start, mod_end);
        }

        #pragma omp section
        {
            result += task_heavy_range(h0, h1);
        }

        #pragma omp section
        {
            result += task_heavy_range(h1, h2);
        }

        #pragma omp section
        {
            result += task_heavy_range(h2, h3);
        }
    }

    double t1 = omp_get_wtime();

    printf("Result checksum = %.10f\n", result);
    printf("Time (4 threads, balanced sections) = %f seconds\n", t1 - t0);

    return 0;
}
