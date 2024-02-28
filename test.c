#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>

#define BUFFER_SIZE 1000000000

int measure_time_once(int (*fun)(int), int len) {
	struct timeval startTime;
	struct timeval endTime;
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru); // start timer
	startTime = ru.ru_utime;

    fun(len);

    getrusage(RUSAGE_SELF, &ru); // end timer

    endTime = ru.ru_utime;
    double tS = startTime.tv_sec * 1000000.0 + (startTime.tv_usec);
    double tE = endTime.tv_sec * 1000000.0 + (endTime.tv_usec);

    return tE - tS;
}

int measure_time(int (*fun)(int), int times, int len) {
    double total = 0;

    for (int i=0; i < times; i++) total += measure_time_once(fun, len);

    return total / times;
}

int long_iter(int times) {
    long int i = 0;
    long int t = times;

    for (; i < t; i++);

    return 0;
}

int int_iter(int times) {
    int i = 0;
    int t = times;

    for (; i < t; i++);

    return 0;
}

int main() {
    printf("Average time for int: %d\n", measure_time(int_iter, 3, BUFFER_SIZE));
    printf("Average time for long: %d\n", measure_time(long_iter, 3, BUFFER_SIZE));
}
