#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <utils.h>

typedef struct {
    double x0, y0;
    double x1, y1;
} Coordinate;

// char* find_string(char* ptr, int length){
//     if (*ptr == '\0') return nullptr;
//     strcmp

//     ++ptr;
// }

int main(int argc, char **argv){
    if (argc != 2){
        fprintf(stderr, "Usage: read_haversine [Jsonfile] \n");
        return EXIT_FAILURE;
    }
    const char* path = argv[1];
    FILE* f = fopen(path, "r");
    assert(f != NULL);

    // get size
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

    double x0, y0, x1, y1;
    int count = 0;
    double sum = 0;
    char *ptr = data;
    while( (ptr = strstr(ptr,"x0")) != nullptr){
        sscanf(ptr, "x0\":%lf", &x0);

        ptr = strstr(ptr,"y0");
        sscanf(ptr, "y0\":%lf", &y0);

        ptr = strstr(ptr,"x1");
        sscanf(ptr, "x1\":%lf", &x1);

        ptr = strstr(ptr,"y1");
        sscanf(ptr, "y1\":%lf", &y1);

        sum += ReferenceHaversine(x0,y0,x1,y1,6372.8);
        count++;

        // printf("-------------------\n");
        // printf("x0: %.15lf\n",x0);
        // printf("y0: %.15lf\n",y0);
        // printf("x1: %.15lf\n",x1);
        // printf("y1: %.15lf\n",y1);
    }
    // printf("-------------------\n");
    if (count != 0){
        sum /= count;
    }
    printf("Pair count: %d\n",count);
    printf("Haversine sum is %lf\n",sum);

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