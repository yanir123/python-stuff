#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

#define LAT 0
#define LON 1
#define ALT 2
#define EPOCH 3

int* agglomerative_clustering_single(double** data, double distance_threshold, double angle_variance_threshold, double speed_threshold, size_t window_size);
double haverine_distance(double* first, double* second);
double calc_speed(double* first, double* second);
double calc_angle_variance(double** first, double* second, size_t window_size);
void merge_clusters(int* clusters, int first, int second);
