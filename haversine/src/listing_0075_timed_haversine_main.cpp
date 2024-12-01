#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <utils.h>
// #include <time.h>
#include "listing_0070_platform_metrics.cpp"

int main(int argc, char **argv){
    // start-up
    if (argc != 2){
        fprintf(stderr, "Usage: read_haversine [Jsonfile] \n");
        return EXIT_FAILURE;
    }
    const char* path = argv[1];
    FILE* f = fopen(path, "r");
    assert(f != NULL);

    // struct timespec start, end;
    // clock_gettime(CLOCK_MONOTONIC, &start);
    u64 os_freq = GetOSTimerFreq();
	u64 os_start = ReadOSTimer();
	u64 os_elapsed_parse = 0;
	u64 os_elapsed_sum = 0;


    // read
    int flag_seek = fseek(f,0,SEEK_END);
    assert(flag_seek == 0);
    long size = ftell(f);
    flag_seek = fseek(f,0,SEEK_SET); // rewind(file);
    printf("Input Size: %ld\n",size);

    char* data = (char*) malloc(size + 1);
    if (data == nullptr){
        fprintf(stderr, "Error allocating memory\n");
        return EXIT_FAILURE;
    }
    data[size] = '\0';
    fread(data,1,size,f);

    // misc-setup
    double x0, y0, x1, y1;
    int count = 0;
    double sum = 0;
    char *ptr = data;

    while( (ptr = strstr(ptr,"x0")) != nullptr){
        // parse
        u64 os_parse_start = ReadOSTimer();
        sscanf(ptr, "x0\":%lf, \"y0\":%lf, \"x1\":%lf, \"y1\":%lf}",&x0,&y0,&x1,&y1);
        u64 os_parse_end = ReadOSTimer();
        os_elapsed_parse += os_parse_end - os_parse_start;

        // sum
        u64 os_sum_start = ReadOSTimer();
        sum += ReferenceHaversine(x0,y0,x1,y1,6372.8);
        u64 os_sum_end = ReadOSTimer();
        os_elapsed_sum += os_sum_end - os_sum_start;

        ++count;
        ++ptr;
    }
    if (count != 0){
        sum /= count;
    }
    fclose(f);
    free(data);

    printf("Pair count: %d\n",count);
    printf("Haversine sum is %lf\n",sum);

	u64 os_end = ReadOSTimer();
    u64 os_elapsed = os_end - os_start;
    f64 total_seconds = (f64)os_elapsed/(f64)os_freq * 1000;
    f64 parse_seconds = (f64)os_elapsed_parse/(f64)os_freq * 1000;
    f64 sum_seconds = (f64)os_elapsed_sum/(f64)os_freq * 1000;

    printf("\n");
	printf("Total time: %.4fms\n", total_seconds);
	printf("  Parse time: %.4fms (%.2f%%)\n", parse_seconds, parse_seconds / total_seconds * 100);
	printf("  Sum time: %.4fms (%.2f%%)\n", sum_seconds, sum_seconds / total_seconds * 100);
    return 0;
}