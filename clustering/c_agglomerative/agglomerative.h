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

/// @brief Function to cluster data points with respect to the location, speed,
/// and direction of trajectories
/// @param data array of points
/// @param height number of data points
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param angle_variance_threshold maximum variance between the `window_size`
/// elements of two clusters
/// @param speed_top_threshold maximum speed between two clusters
/// @param speed_min_threshold minimum speed between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @return array containing cluster ids correlated to the indexes of the `data`
/// parameter
int* agglomerative_clustering_single(double** data, size_t height,
                                     double distance_threshold,
                                     double angle_variance_threshold,
                                     double speed_top_threshold,
                                     double speed_min_threshold,
                                     size_t window_size);

/// @brief calculate the distance between two data points using the haversine
/// formula
/// @param first first data point
/// @param second second data point
/// @return the distance in meters
double haversine_distance(double* first, double* second);

/// @brief Add the new cluster to the cluster array and remove the two old ones
/// @param clusters pointer to the cluster array
/// @param len pointer to the length of the cluster array
/// @param new_cluster cluster to add to the array
/// @param first_index index of first cluster to remove
/// @param second_index index of second cluster to remove
void add_cluster_and_remove_old_ones(cluster_t** clusters, size_t* len,
                                     cluster_t new_cluster, size_t first_index,
                                     size_t second_index);

/// @brief print and array of datapoints
/// @param arr array to print
/// @param len length of the array
void print_array(double* arr, size_t len);

/// @brief free cluster indices
/// @param cluster cluster to free
void free_cluster(cluster_t cluster);

/// @brief print array of clusterids
/// @param arr array to print
/// @param len length of the array
void print_array_res(int* arr, size_t len);

/// @brief calculate the speed between two data points
/// @param first first data point
/// @param second second data point
/// @param distance distance between the data points
/// @return the speed in m/s
double calc_speed(double* first, double* second, double distance);

/// @brief calculate the variance between two clusters angles given a window
/// @param data data points
/// @param first first cluster
/// @param second second cluster
/// @param window_size window frame to calc variance on
/// @return the variance between the two clusters
double calc_angle_variance(double** data, cluster_t first, cluster_t second,
                           size_t window_size);

/// @brief calculate the angle between two points
/// @param first_x first point x
/// @param first_y first point y
/// @param second_x second point x
/// @param second_y second point y
/// @return angle between two points in degrees
double angle_degree(double first_x, double first_y, double second_x,
                    double second_y);

/// @brief merge two clusters
/// @param first first cluster to merge
/// @param second second cluster to merge
/// @return the new cluster combining both `first` and `second`
cluster_t merge_clusters(cluster_t first, cluster_t second);

/// @brief create array on given size and fill it with values correlated to it's
/// index
/// @param len length of array to create
/// @return the new array
int* arange(size_t len);

/// @brief Function to find cluster pairs that are valid to the given thresholds
/// @param data array of points
/// @param height number of data points
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param angle_variance_threshold maximum variance between the `window_size`
/// elements of two clusters
/// @param speed_top_threshold maximum speed between two clusters
/// @param speed_min_threshold minimum speed between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @param clusters pointer to the cluster array
/// @param cluster_len pointer to the cluster length
/// @return boolean value indicating if the was a change
uint8_t find_compatible_clusters(double** data, size_t height,
                                 double distance_threshold,
                                 double angle_variance_threshold,
                                 double speed_top_threshold,
                                 double speed_min_threshold, size_t window_size,
                                 cluster_t** clusters, size_t* cluster_len);

/// @brief Function to raorganize the cluster array to a valid clusterid per
/// index
/// @param clusters array of clusters
/// @param len number of clusters in the array
/// @param number_of_points number of points in the original array
/// @return new array with the clusterid
int* get_cluster_array_with_origininal_indices(cluster_t* clusters, size_t len,
                                               size_t number_of_points);

/// @brief Function to create array of clusters of given length where every
/// element is its own cluster
/// @param len length of the cluster (number of initial data points)
/// @return
cluster_t* create_base_array(size_t len);

/// @brief Function to print the array
/// @param arr array to print
/// @param len length of the array
void print_array(double* arr, size_t len);

/// @brief Function to check if two clusters are compatible
/// @param data array of points
/// @param first first cluster
/// @param second second cluster
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param angle_variance_threshold maximum variance between the `window_size`
/// elements of two clusters
/// @param speed_top_threshold maximum speed between two clusters
/// @param speed_min_threshold minimum speed between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @return boolean value indicating if the clusters are compatible
uint8_t check_compatibility(double** data, cluster_t first, cluster_t second,
                            double distance_threshold,
                            double angle_variance_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold, size_t window_size);

/// @brief Function to free all clusters
/// @param clusters cluster array to free
/// @param len length of cluster array to free
void free_all_clusters(cluster_t* clusters, size_t len);
