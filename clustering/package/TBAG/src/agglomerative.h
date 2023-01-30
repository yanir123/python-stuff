#define _USE_MATH_DEFINES

#include <malloc.h>
#include <math.h>
// #include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define R 6371000
#define FALSE 0
#define TRUE !FALSE
#define PI 3.14159265359

enum columns { LAT, LON, ALT, EPOCH };

typedef struct cluster_s {
  size_t len;
  size_t* indices;
} cluster_t;

/// @brief cluster data points with respect to the location, speed,
/// and direction of trajectories
/// @param data array of points
/// @param height number of data points
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param time_threshold maximum time diff allowed between points
/// @param angle_variance_threshold maximum variance between the `window_size`
/// elements of two clusters
/// @param speed_top_threshold maximum speed between two clusters
/// @param speed_min_threshold minimum speed between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @param res result array
void agglomerative_clustering(double** data, size_t height,
                              double distance_threshold, double time_threshold,
                              double angle_variance_threshold,
                              double speed_top_threshold,
                              double speed_min_threshold, size_t window_size,
                              int* res);

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
double calc_angle_variance(double** data, cluster_t first, size_t second,
                           size_t window_size);

/// @brief calculate the angle between two points
/// @param first_x first point x
/// @param first_y first point y
/// @param second_x second point x
/// @param second_y second point y
/// @return angle between two points in degrees
double angle_degree(double first_x, double first_y, double second_x,
                    double second_y);

/// @brief raorganize the cluster array to a valid clusterid per index
/// @param clusters array of clusters
/// @param len number of clusters in the array
/// @param number_of_points number of points in the original array
void get_cluster_array_with_origininal_indices(cluster_t* clusters, size_t len,
                                               size_t number_of_points,
                                               int* res);

/// @brief check if two clusters are compatible
/// @param data array of points
/// @param first first cluster
/// @param second second cluster
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param time_threshold maximum time diff allowed between points
/// @param angle_variance_threshold maximum variance between the `window_size`
/// elements of two clusters
/// @param speed_top_threshold maximum speed between two clusters
/// @param speed_min_threshold minimum speed between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @return boolean value indicating if the clusters are compatible
uint8_t check_compatibility(double** data, cluster_t first, size_t second,
                            double distance_threshold, double time_threshold,
                            double angle_variance_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold, size_t window_size);

/// @brief free all clusters
/// @param clusters cluster array to free
/// @param len length of cluster array to free
void free_all_clusters(cluster_t* clusters, size_t len);

/// @brief check the distance between two clusters
/// @param data
/// @param first
/// @param second
/// @return
double cluster_distance(double** data, cluster_t first, size_t second);

/// @brief add new cluster to the cluster array
/// @param clusters_array pointer to the cluster of existing arrays
/// @param cluster_len pointer to the length of the current cluster array
/// @param index index of the element that will be added as a new cluster
void add_new_cluster(cluster_t** clusters_array, size_t* cluster_len,
                     size_t index);

/// @brief add new element to existing cluster
/// @param clusters_array array of existing clusters
/// @param cluster_index index of cluster to add element to
/// @param index element to add to cluster
void add_to_cluster(cluster_t* clusters_array, size_t cluster_index,
                    size_t index);

/// @brief find the closest cluster that is valid according to the user defined
/// thresholds
/// @param data the datapoints
/// @param height the number of datapoints in data
/// @param clusters_array array of existing clusters
/// @param cluster_len number of existing clusters
/// @param index index of the element to find compatibbility with
/// @param distance_threshold maximum distance between cluster and new element
/// @param time_threshold maximum time gap between cluster and new element
/// @param angle_variance_threshold maximum angle variance between cluster and
/// new element
/// @param speed_top_threshold maximum speed between cluster and new element
/// @param speed_min_threshold minimum speed between cluster and new element
/// @param window_size number of elements from the end of the cluster to refer
/// to
/// @return the index of the most compatible cluster, -1 if none are compatible
int find_closest_compatible_cluster(
    double** data, size_t height, cluster_t* clusters_array, size_t cluster_len,
    size_t index, double distance_threshold, double time_threshold,
    double angle_variance_threshold, double speed_top_threshold,
    double speed_min_threshold, size_t window_size);