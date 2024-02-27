#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1000000

int measure_time_once(int (*fun)(int*, int), int* vec, int len) {
	struct timeval startTime;
	struct timeval endTime;
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru); // start timer
	startTime = ru.ru_utime;

    fun(vec, len);

    getrusage(RUSAGE_SELF, &ru); // end timer

    endTime = ru.ru_utime;
    double tS = startTime.tv_sec * 1000000.0 + (startTime.tv_usec);
    double tE = endTime.tv_sec * 1000000.0 + (endTime.tv_usec);

    return tE - tS;
}

int measure_time(int (*fun)(int*, int), int times, int* vec, int len) {
    double total = 0;

    for (int i=0; i < times; i++) total += measure_time_once(fun, vec, len);

    return total / times;
}

int* create_vec(int len) {
    int* vec = malloc(len * sizeof(int));
    for (int i = 0; i < len; i++)
        vec[i] = rand();
    return vec;
}

int compute_total_naive(int* vec, int len) {
    int tot = 0;
    for (int i = 0; i < len; i++)
        tot += vec[i];
    return tot;
}

int compute_total_ptr(int* vec, int len) {
    int tot = 0;
    for (int i = 0; i < len; i++)
        tot += *vec++;
    return tot;
}

// Loop unrolling v1
int compute_total_unroll(int* vec, int len) {
    int tot = 0;
    for (int i = 0; i < len - 1; i+=2) {
        tot += vec[i];
        tot += vec[i+1];
    }

    if (len % 2 == 1) tot += vec[len-1];

    return tot;
}

// Keep pointer as reference to current index, instead of computing vec+i each time.
// Also i++ instead of i+=2
int compute_total_unroll2(int* vec, int len) {
    int tot = 0;
    int times = len / 2;

    for (int i = 0; i < times; i++) {
        tot += *vec++;
        tot += *vec++;
    }

    if (len % 2 == 1) tot += *vec++;

    return tot;
}

int compute_total_unroll3(int* vec, int len) {
    int tot = 0;
    int* end = vec + len - 1;

    while (vec < end) {
        tot += *vec++;
        tot += *vec++;
    }

    end += 1;
    while (vec < end) tot += *vec++;

    return tot;
}

int compute_total_duff(int* vec, int len) {
    register int tot = 0;
    register int n = (len + 7) / 8;

    switch (len % 8) {
        case 0: do{ tot += *vec++;
        case 7:     tot += *vec++;
        case 6:     tot += *vec++;
        case 5:     tot += *vec++;
        case 4:     tot += *vec++;
        case 3:     tot += *vec++;
        case 2:     tot += *vec++;
        case 1:     tot += *vec++;
                } while(--n > 0);
    }

    return tot;
}

int compute_total_parallel(int* vec, int len) {
    int tot1 = 0;
    int tot2 = 0;
    int times = len / 2;

    for (int i = 0; i < times; i++) {
        tot1 += vec[i];
        tot2 += vec[i+1];
    }

    if (len % 2 == 1) tot1 += vec[len-1];

    return tot1 + tot2;
}

int compute_total_parallel2(int* vec, int len) {
    int tot1 = 0;
    int tot2 = 0;
    int times = len / 2;

    for (int i = 0; i < times; i++) {
        tot1 += *vec++;
        tot2 += *vec++;
    }

    if (len % 2 == 1) tot1 += *vec++;

    return tot1 + tot2;
}

int compute_total_parallel3(int* vec, int len) {
    int tot1 = 0;
    int tot2 = 0;
    int* end = vec + len - 1;

    while (vec < end) {
        tot1 += *vec++;
        tot2 += *vec++;
    }

    end ++;
    while (vec < end) tot1 += *vec++;

    return tot1 + tot2;
}

int main() {
    srand(time(NULL));

    int* vec = create_vec(BUFFER_SIZE);

    printf("Average time for naive: %d\n", measure_time(compute_total_naive, 100, vec, BUFFER_SIZE));
    printf("Average time for ptr: %d\n", measure_time(compute_total_ptr, 100, vec, BUFFER_SIZE));
    printf("Average time for unroll: %d\n", measure_time(compute_total_unroll, 100, vec, BUFFER_SIZE));
    printf("Average time for unroll2: %d\n", measure_time(compute_total_unroll2, 100, vec, BUFFER_SIZE));
    printf("Average time for unroll3: %d\n", measure_time(compute_total_unroll3, 100, vec, BUFFER_SIZE));
    printf("Average time for duff: %d\n", measure_time(compute_total_duff, 100, vec, BUFFER_SIZE));
    printf("Average time for parallel: %d\n", measure_time(compute_total_parallel, 100, vec, BUFFER_SIZE));
    printf("Average time for parallel2: %d\n", measure_time(compute_total_parallel2, 100, vec, BUFFER_SIZE));
    printf("Average time for parallel3: %d\n", measure_time(compute_total_parallel3, 100, vec, BUFFER_SIZE));
}

