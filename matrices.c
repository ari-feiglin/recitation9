#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFFER_SIZE 100

int measure_time_once(void (*fun)(int*, int*, int*, int), int* m1, int* m2, int* res, int len) {
	struct timeval startTime;
	struct timeval endTime;
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru); // start timer
	startTime = ru.ru_utime;

    fun(m1, m2, res, len);

    getrusage(RUSAGE_SELF, &ru); // end timer

    endTime = ru.ru_utime;
    double tS = startTime.tv_sec * 1000000.0 + (startTime.tv_usec);
    double tE = endTime.tv_sec * 1000000.0 + (endTime.tv_usec);

    return tE - tS;
}

int measure_time(void (*fun)(int*, int*, int*, int), int times, int* m1, int* m2, int* res, int len) {
    double total = 0;

    for (int i=0; i < times; i++) total += measure_time_once(fun, m1, m2, res, len);

    return total / times;
}

int* create_mat(int len) {
    int* mat = malloc(len * len * sizeof(int));
    for (int i = 0; i < len * len; i++)
        mat[i] = rand();
    return mat;
}

void mult_mats_naive(int* m1, int* m2, int* res, int len) {
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            res[i * len + j] = 0;
            for (int k = 0; k < len; k++)
                res[i * len + j] += m1[i * len + k] * m2[k * len + j];
        }
    }
}

void mult_mats_copy(int* m1, int* m2, int* res, int len) {
    int* col = malloc(len * sizeof(int));

    for (int j = 0; j < len; j++) {
        for (int k = 0; k < len; k++) col[k] = m2[k * len + j];
        for (int i = 0; i < len; i++) {
            res[i * len + j] = 0;
            for (int k = 0; k < len; k++)
                res[i * len + j] += m1[i * len + k] * col[k];
        }
    }
}

void mult_mats_copy2(int* m1, int* m2, int* res, int len) {
    int* col = malloc(len * sizeof(int));
    int s = 0;

    for (int j = 0; j < len; j++) {
        for (int k = 0; k < len; k++) col[k] = m2[k * len + j];
        for (int i = 0; i < len; i++) {
            int ilen = i * len;
            for (int k = 0; k < len; k++)
                s += m1[ilen + k] * col[k];
            res[ilen + j] = s;
        }
    }
}

int main() {
    srand(time(NULL));

    int* m1 = create_mat(BUFFER_SIZE);
    int* m2 = create_mat(BUFFER_SIZE);
    int* res = malloc(BUFFER_SIZE * BUFFER_SIZE * sizeof(int));

    printf("Average time for naive: %d\n", measure_time(mult_mats_naive, 100, m1, m2, res, BUFFER_SIZE));
    printf("Average time for copy: %d\n", measure_time(mult_mats_copy, 100, m1, m2, res, BUFFER_SIZE));
    printf("Average time for copy2: %d\n", measure_time(mult_mats_copy2, 100, m1, m2, res, BUFFER_SIZE));
}

