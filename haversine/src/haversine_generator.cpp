#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <utils.h>

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

    FILE *file = fopen("./data/harversine.json", "w");
    if (file == NULL) {
        printf("Unable to create file\n");
        return 1;
    }

    srand(time(NULL));
    f64 data_ptr[NUM_COORDINATES] = {0.0,};
    f64 sum = 0.0;
    f64 x_range1,y_range1,x_range2,y_range2;

    std::mt19937 gen(random_seed);
    if (TYPE_CLUSTER == 1){
        int NUM_CLUSTER = 4;
        std::uniform_int_distribution<> dis(0, NUM_CLUSTER - 1);
        int x_random = dis(gen);
        int y_random = dis(gen);

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
    std::uniform_real_distribution<double> lon_dist(x_range1, x_range2);
    std::uniform_real_distribution<double> lat_dist(y_range1, y_range2);

    fprintf(file, "{\"pairs\":[\n");
    double coeff = double(1.0 / NUM_POINTS);
    for (int i=0;i<NUM_POINTS;++i){
        // generate_data(data_ptr, gen);
        data_ptr[0] = lon_dist(gen);
        data_ptr[1] = lat_dist(gen);
        data_ptr[2] = lon_dist(gen);
        data_ptr[3] = lat_dist(gen);

        fprintf(file, "    {\"x0\":%.15lf, \"y0\":%.15lf, \"x1\":%.15lf, \"y1\":%.15lf}",data_ptr[0],data_ptr[1],data_ptr[2],data_ptr[3]);
        if (i < NUM_POINTS - 1){
            fprintf(file, ",");
        }
        fprintf(file, "\n");
        sum += coeff * ReferenceHaversine(data_ptr[0],data_ptr[1],data_ptr[2],data_ptr[3],6372.8);
    }
    fprintf(file,"],\n");

    fprintf(file, "\"size\":%d,\n",NUM_POINTS);
    fprintf(file, "\"average\":%.15f",sum);

    fprintf(file,"}");
    fclose(file);


    printf("JSON file created successfully\n");
    printf("Expected sum: %lf\n",sum);

    return 0;
}
