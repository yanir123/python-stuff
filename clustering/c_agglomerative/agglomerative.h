#include <malloc.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdlib.h>

// #define LAT 0
// #define LON 1
// #define ALT 2
// #define EPOCH 3
#define R 6371000
#define FALSE 0
#define TRUE !FALSE

enum columns { LAT, LON, ALT, EPOCH };

typedef struct cluster_s {
  int cluster_id;
  size_t len;
  int* indices;
} cluster_t;

int* agglomerative_clustering_single(double** data, size_t height,
                                     double distance_threshold,
                                     double angle_variance_threshold,
                                     double speed_top_threshold,
                                     double speed_min_threshold,
                                     size_t window_size);
double haverine_distance(double* first, double* second);
double calc_speed(double* first, double* second);
double calc_angle_variance(double** first, size_t len, double* second,
                           size_t window_size);
double angle_degree(double first_x, double first_y, double second_x,
                    double second_y);
void merge_clusters(int* clusters, size_t len, int first,
                    int second);  // TODO: change this to work with the type
int* arange(size_t len);          // IDK if needed
uint8_t find_compatible_clusters(double** data, size_t height,
                                 double distance_threshold,
                                 double angle_variance_threshold,
                                 double speed_top_threshold,
                                 double speed_min_threshold, size_t window_size,
                                 cluster_t* clusters);  // TODO: implement this
int* get_cluster_array_with_origininal_indices(
    cluster_t* clusters, size_t len);  // TODO: implement this
cluster_t* create_base_array(size_t len);
void free_all_clusters(cluster_t* clusters, size_t len);
