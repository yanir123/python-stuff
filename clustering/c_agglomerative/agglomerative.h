#define _USE_MATH_DEFINES

#include <malloc.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
double haversine_distance(double* first, double* second);
double calc_speed(double* first, double* second, double distance);
double calc_angle_variance(double** data, cluster_t first, cluster_t second,
                           size_t window_size);
double angle_degree(double first_x, double first_y, double second_x,
                    double second_y);
cluster_t merge_clusters(cluster_t first, cluster_t second);
int* arange(size_t len);
uint8_t find_compatible_clusters(double** data, size_t height,
                                 double distance_threshold,
                                 double angle_variance_threshold,
                                 double speed_top_threshold,
                                 double speed_min_threshold, size_t window_size,
                                 cluster_t* clusters, size_t* cluster_len);
int* get_cluster_array_with_origininal_indices(cluster_t* clusters, size_t len,
                                               size_t number_of_points);
cluster_t* create_base_array(size_t len);
void print_array(double* arr, size_t len);
uint8_t check_compatibility(double** data, cluster_t first, cluster_t second,
                            double distance_threshold,
                            double angle_variance_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold, size_t window_size);
void free_all_clusters(cluster_t* clusters, size_t len);
