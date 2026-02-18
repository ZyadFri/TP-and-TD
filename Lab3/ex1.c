#include <stdio.h>
#include <omp.h>

int main() {
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int nt  = omp_get_num_threads();

        #pragma omp critical
        {
            printf("Hello from the rank %d thread\n", tid);
        }

        #pragma omp single
        {
            printf("Parallel execution of hello_world with %d threads\n", nt);
        }
    }
    return 0;
}
