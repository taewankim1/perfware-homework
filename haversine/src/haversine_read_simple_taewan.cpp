#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <utils.h>
#include <time.h>
#include <math.h>

f64 read_long_to_string_from_json(char* ptr){
    f64 integer = 0.0;
    f64 fraction = 0.0;
    f64 sign = 1;
    if (*ptr == '-'){
        sign = -1;
        ptr++;
    }
    while(*ptr != '.' && *ptr != ',' && *ptr != '}')
    {
        integer *= 10;
        integer += (*ptr - '0');
        ptr++;
    }
    ptr++;

    double divisor = 1.0;
    while (*ptr >= '0' && *ptr <= '9') {
        divisor *= 10.0;
        fraction += (*ptr - '0') / divisor;
        ptr++;
    }

    // if (*ptr == ',' || *ptr == '}') 
    return sign * (integer + fraction);
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: read_haversine [Jsonfile] \n");
        return EXIT_FAILURE;
    }
    const char *path = argv[1];
    FILE *f = fopen(path, "r");
    assert(f != NULL);

    // Declare variables for time measure.
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Get the full size of the json file.
    int flag_seek = fseek(f, 0, SEEK_END);
    assert(flag_seek == 0);
    const long size_json = ftell(f);
    flag_seek = fseek(f, 0, SEEK_SET);
    printf("Input Size: %ld\n", size_json);

    // Allocate the memory to read the entire json file.
    char *data = (char *)malloc(size_json + 1);
    if (data == nullptr)
    {
        fprintf(stderr, "Error allocating memory\n");
        return EXIT_FAILURE;
    }
    // Put the null at the last element.
    data[size_json] = '\0';

    // Read the file and save the data.
    fread(data, 1, size_json, f);
    f64 x0, y0, x1, y1;
    int count = 0;
    double sum = 0;
    char *ptr = data;

    while (*ptr != '\0')
    {
        // Find 'x'.
        while (*ptr != 'x' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr == 'x'){
            ptr += 4;
            x0 = read_long_to_string_from_json(ptr);
            // printf("val: %.15lf\n",x0);
        }
        else break;
        while (*ptr != 'y' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr == 'y'){
            ptr += 4;
            y0 = read_long_to_string_from_json(ptr);
            // printf("val: %.15lf\n",y0);
        }
        else break;
        while (*ptr != 'x' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr == 'x'){
            ptr += 4;
            x1 = read_long_to_string_from_json(ptr);
            // printf("val: %.15lf\n",x1);
        }
        else break;
        while (*ptr != 'y' && *ptr != '\0')
        {
            ptr++;
        }
        if (*ptr == 'y'){
            ptr += 4;
            y1 = read_long_to_string_from_json(ptr);
            // printf("val: %.15lf\n",y1);
        }
        else break;
        sum += ReferenceHaversine(x0,y0,x1,y1,6372.8);
        count++;
    }

    printf("-------------------\n");
    if (count != 0){
        sum /= count;
    }
    printf("Pair count: %d\n",count);
    printf("Haversine sum is %0.15lf\n",sum);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed_time_sec = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Elapsed time sec is %lf\n",elapsed_time_sec);

    printf("\n");
    printf("Validation:\n");
    double reference_sum = 0.0;
    ptr = strstr(data,"average");
    if (ptr != nullptr){
        sscanf(ptr, "average\":%lf", &reference_sum);
        printf("Reference sum is %lf\n",reference_sum);
        printf("Difference is %lf\n",sum - reference_sum);
    }

    fclose(f);
    free(data);
    return 0;
}
