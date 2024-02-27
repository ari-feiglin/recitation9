#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>

#define BUFFER_SIZE 10000

int measure_time_once(void (*fun)(char*), char* str) {
	struct timeval startTime;
	struct timeval endTime;
	struct rusage ru;

	getrusage(RUSAGE_SELF, &ru); // start timer
	startTime = ru.ru_utime;

    fun(str);

    getrusage(RUSAGE_SELF, &ru); // end timer

    endTime = ru.ru_utime;
    double tS = startTime.tv_sec * 1000000.0 + (startTime.tv_usec);
    double tE = endTime.tv_sec * 1000000.0 + (endTime.tv_usec);

    return tE - tS;
}

int measure_time(void (*fun)(char*), int times, char* str) {
    double total = 0;

    for (int i=0; i < times; i++) total += measure_time_once(fun, str);

    return total / times;
}

char* create_str(int len) {
    char* str = malloc(len + 1);
    for (int i = 0; i < len; i++)
        str[i] = rand() % ('z' - 'a') + 'a';
    str[len] = 0;
    return str;
}

void to_uppercase_naive(char* str) {
    for (int i = 0; i < strlen(str); i++) {
        if ('a' <= str[i] && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
}

void to_uppercase_moved(char* str) {
    int n = strlen(str);
    for (int i = 0; i < n; i++) {
        if ('a' <= str[i] && str[i] <= 'z')
            str[i] += 'A' - 'a';
    }
}

void to_uppercase_ptr(char* str) {
    while (*str != 0) {
        if ('a' <= *str && *str <= 'z')
            *str += 'A' - 'a';
        *str++;
    }
}

int main() {
    srand(time(NULL));

    char* str = create_str(BUFFER_SIZE);

    printf("Average time for naive: %d\n", measure_time(to_uppercase_naive, 100, str));
    printf("Average time for moved: %d\n", measure_time(to_uppercase_moved, 100, str));
    printf("Average time for ptr: %d\n", measure_time(to_uppercase_ptr, 100, str));
    return 0;
}

