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
#define WIDTH 4

enum columns { LAT, LON, ALT, EPOCH };

typedef struct cluster_s {
  unsigned int len;
  unsigned int* indices;
} cluster_t;

/// @brief cluster data points with respect to the location, speed,
/// and direction of trajectories
/// @param data array of points
/// @param height number of data points
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param time_threshold maximum time diff allowed between points
/// @param angle_diff_threshold maximum angle difference between the
/// `window_size` elements of two clusters
/// @param speed_diff_threshold maximum speed difference between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @param res result array
void agglomerative_clustering(double** data, unsigned int height,
                              double distance_threshold, double time_threshold,
                              double angle_diff_threshold,
                              double speed_diff_threshold,
                              unsigned int window_size, int* res);

/// @brief calculate the distance between two data points using the haversine
/// formula
/// @param first first data point
/// @param second second data point
/// @return the distance in meters
double haversine_distance(double* first, double* second);

/// @brief print array of clusterids
/// @param arr array to print
/// @param len length of the array
void print_array_res(int* arr, unsigned int len);

/// @brief calculate the speed between two data points
/// @param first first data point
/// @param second second data point
/// @return the speed in m/s
double calc_speed(double* first, double* second);

/// @brief calculate the variance between two clusters angles given a window
/// @param data data points
/// @param first first cluster
/// @param second second cluster
/// @param window_size window frame to calc variance on
/// @param angle_diff_threshold value to return if cluster is small
/// @return the angle difference between the two clusters
double calc_angle_diff(double** data, cluster_t first, unsigned int second,
                       unsigned int window_size, double angle_diff_threshold);

/// @brief calculate the angle between two points
/// @param first first data point
/// @param second second data point
/// @return angle between two points in degrees
double angle_degree(double* first, double* second);

/// @brief raorganize the cluster array to a valid clusterid per index
/// @param clusters array of clusters
/// @param len number of clusters in the array
/// @param number_of_points number of points in the original array
/// @param res result array
void get_cluster_array_with_origininal_indices(cluster_t* clusters,
                                               unsigned int len,
                                               unsigned int number_of_points,
                                               int* res);

/// @brief check if two clusters are compatible
/// @param data array of points
/// @param first first cluster
/// @param second second cluster
/// @param distance_threshold maximum distance between last point od one cluster
/// and first point of second cluster
/// @param time_threshold maximum time diff allowed between points
/// @param angle_diff_threshold maximum angle difference between the
/// `window_size` elements of two clusters
/// @param speed_diff_threshold maximum speed difference between two clusters
/// @param window_size number of elements to check back in the cluster in
/// relation to the angle variance
/// @param res_haversine pointer to result phyisical distance
/// @param res_angle pointer to result angle distance
/// @return boolean value indicating if the clusters are compatible
uint8_t check_compatibility(double** data, cluster_t first, unsigned int second,
                            double distance_threshold, double time_threshold,
                            double angle_diff_threshold,
                            double speed_diff_threshold,
                            unsigned int window_size, double* res_haversine,
                            double* res_angle);

/// @brief free all clusters
/// @param clusters cluster array to free
/// @param len length of cluster array to free
void free_all_clusters(cluster_t* clusters, unsigned int len);

/// @brief add new cluster to the cluster array
/// @param clusters_array pointer to the cluster of existing arrays
/// @param cluster_len pointer to the length of the current cluster array
/// @param index index of the element that will be added as a new cluster
void add_new_cluster(cluster_t** clusters_array, unsigned int* cluster_len,
                     unsigned int index);

/// @brief add new element to existing cluster
/// @param clusters_array array of existing clusters
/// @param cluster_index index of cluster to add element to
/// @param index element to add to cluster
void add_to_cluster(cluster_t* clusters_array, unsigned int cluster_index,
                    unsigned int index);

/// @brief find the closest cluster that is valid according to the user defined
/// thresholds
/// @param data the datapoints
/// @param height the number of datapoints in data
/// @param clusters_array array of existing clusters
/// @param cluster_len number of existing clusters
/// @param index index of the element to find compatibbility with
/// @param distance_threshold maximum distance between cluster and new element
/// @param time_threshold maximum time gap between cluster and new element
/// @param angle_diff_threshold maximum angle difference between cluster and
/// new element
/// @param speed_diff_threshold maximum speed difference cluster and new element
/// @param window_size number of elements from the end of the cluster to refer
/// to
/// @return the index of the most compatible cluster, -1 if none are compatible
int find_closest_compatible_cluster(
    double** data, unsigned int height, cluster_t* clusters_array,
    unsigned int cluster_len, unsigned int index, double distance_threshold,
    double time_threshold, double angle_diff_threshold,
    double speed_diff_threshold, unsigned int window_size);

/// @brief general function to calculate diference from window of cluster to new
/// point
/// @param data data points
/// @param first existing cluster
/// @param second new contendor
/// @param window_size number of element to check back on
/// @param diff_func function to calc the difference
/// @param threshold value to return if cluster is small
/// @return
double calc_diff(double** data, cluster_t first, unsigned int second,
                 unsigned int window_size,
                 double (*diff_func)(double*, double*), double threshold);

/// @brief get the mean plot of range in cluster
/// @param data data points
/// @param cluster cluster to run on
/// @param res_mean mean plot
/// @param start begining of range
/// @param end end of range
void mean_between(double** data, cluster_t cluster, double* res_mean,
                  unsigned int start, unsigned int end);

/// @brief calc speed diff on a moving mean window
/// @param data data points
/// @param first existing cluster to perform window function on
/// @param second new contender point
/// @param window_size the number of data points from the end to run over
/// @param speed_diff_threshold value to return if cluster is small
/// @return the speed diff
double calc_speed_diff(double** data, cluster_t first, unsigned int second,
                       unsigned int window_size, double speed_diff_threshold);