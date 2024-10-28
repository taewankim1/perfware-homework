#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <utils.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef double f64;

const int NUM_COORDINATES = 4;
int NUM_POINTS = 10;
int TYPE_CLUSTER = 0;

int main(int argc, char **argv){
    if (argc != 4){
        fprintf(stderr, "Usage: haversine [uniform/cluster] [randomseed] [num of points]\n");
        return EXIT_FAILURE;
    }
    if (strcmp(argv[1], "uniform") == 0) {
        TYPE_CLUSTER = 0;
    }
    else if(strcmp(argv[1], "cluster") == 0) {
        TYPE_CLUSTER = 1;
    }
    int random_seed = std::atoi(argv[2]);
    NUM_POINTS = std::atoi(argv[3]);

    FILE *file = fopen("harversine.json", "w");
    if (file == NULL) {
        printf("Unable to create file\n");
        return 1;
    }

    srand(time(NULL));
    f64 data_ptr[NUM_COORDINATES] = {0.0,};
    f64 sum = 0.0;
    f64 average = 0.0;
    f64 x_range1,y_range1,x_range2,y_range2;

    // std::random_device rd;
    std::mt19937 gen(random_seed);
    if (TYPE_CLUSTER == 1){
        int NUM_CLUSTER = 4;
        std::uniform_int_distribution<> dis(0, NUM_CLUSTER - 1);
        int x_random = dis(gen);
        int y_random = dis(gen);
        printf("x_random: %d, y_random: %d\n",x_random,y_random);
        x_range1 = -180.0 + 360 / NUM_CLUSTER * x_random;
        x_range2 = -180.0 + 360 / NUM_CLUSTER * (x_random+1);
        y_range1 = -90.0 + 180 / NUM_CLUSTER * y_random;
        y_range2 = -90.0 + 180 / NUM_CLUSTER * (y_random+1);
    }
    else{
        x_range1 = -180.0;
        x_range2 = 180.0;
        y_range1 = -90.0;
        y_range2 = 90.0;
    }
    std::uniform_real_distribution<> lon_dist(x_range1, x_range2);
    std::uniform_real_distribution<> lat_dist(y_range1, y_range2);

    fprintf(file, "{\"pairs\":[\n");
    for (int i=0;i<NUM_POINTS;++i){
        // generate_data(data_ptr, gen);
        data_ptr[0] = lon_dist(gen);
        data_ptr[1] = lat_dist(gen);
        data_ptr[2] = lon_dist(gen);
        data_ptr[3] = lat_dist(gen);

        fprintf(file, "    {\"x0\":%lf, \"y0\":%lf, \"x1\":%lf, \"y1\":%lf}",data_ptr[0],data_ptr[1],data_ptr[2],data_ptr[3]);
        if (i < NUM_POINTS - 1){
            fprintf(file, ",");
        }
        fprintf(file, "\n");
        sum += ReferenceHaversine(data_ptr[0],data_ptr[1],data_ptr[2],data_ptr[3],6372.8);
    }
    fprintf(file, "]}\n");
    fclose(file);

    average = sum / NUM_POINTS;

    printf("JSON file created successfully\n");
    printf("Expected sum: %lf\n",average);

    return 0;
    return EXIT_SUCCESS;
}
