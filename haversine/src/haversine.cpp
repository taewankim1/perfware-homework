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

const int NUM_POINTS = 10000;
const int NUM_COORDINATES = 4;

int generate_data(f64* result){
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0);
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0);

    result[0] = lon_dist(gen);
    result[1] = lat_dist(gen);
    result[2] = lon_dist(gen);
    result[3] = lat_dist(gen);
    return 1;
}

int main() {
    FILE *file = fopen("harversine.json", "w");
    if (file == NULL) {
        printf("Unable to create file\n");
        return 1;
    }

    srand(time(NULL));
    f64 data_ptr[NUM_COORDINATES] = {0.0,};
    f64 sum = 0.0;
    f64 average = 0.0;


    fprintf(file, "{\"pairs\":[\n");
    for (int i=0;i<NUM_POINTS;++i){
        generate_data(data_ptr);
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
}

