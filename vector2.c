#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1000000

int measure_time_once(void (*fun)(int*, int, int*), int* vec, int len, int* dest) {
	struct timeval startTime;
	struct timeval endTime;
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru); // start timer
	startTime = ru.ru_utime;

    fun(vec, len, dest);

    getrusage(RUSAGE_SELF, &ru); // end timer

    endTime = ru.ru_utime;
    double tS = startTime.tv_sec * 1000000.0 + (startTime.tv_usec);
    double tE = endTime.tv_sec * 1000000.0 + (endTime.tv_usec);

    return tE - tS;
}

int measure_time(void (*fun)(int*, int, int*), int times, int* vec, int len, int* dest) {
    double total = 0;

    for (int i=0; i < times; i++) total += measure_time_once(fun, vec, len, dest);

    return total / times;
}

int* create_vec(int len) {
    int* vec = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        vec[i] = rand();
    return vec;
}

void compute_total_dest_naive(int* vec, int len, int* dest) {
    *dest = 0;
    for (int i = 0; i < len; i++)
        *dest += *vec++;
}

void compute_total_dest_buffer(int* vec, int len, int* dest) {
    register int tot = 0;
    for (int i = 0; i < len; i++)
        tot += *vec++;
    *dest = tot;
}

int main() {
    srand(time(NULL));

    int* vec = create_vec(BUFFER_SIZE);
    int dest = 0;

    printf("Average time for dest naive: %d\n", measure_time(compute_total_dest_naive, 100, vec, BUFFER_SIZE, &dest));
    printf("Average time for dest buffer: %d\n", measure_time(compute_total_dest_buffer, 100, vec, BUFFER_SIZE, &dest));
}
