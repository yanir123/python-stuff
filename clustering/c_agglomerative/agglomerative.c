#include "agglomerative.h"

int *agglomerative_clustering_single(double **data, size_t height,
                                     double distance_threshold,
                                     double angle_variance_threshold,
                                     double speed_top_threshold,
                                     double speed_min_threshold,
                                     size_t window_size) {
  cluster_t *clusters_array = create_base_array(height);

  uint8_t updated = TRUE;

  while (updated) {
    updated = find_compatible_clusters(
        data, height, distance_threshold, angle_variance_threshold,
        speed_top_threshold, speed_min_threshold, window_size, clusters_array);
  }

  return NULL;
}

double haverine_distance(double *first, double *second) {
  double a = pow(sin((second[LAT] - first[LAT]) / 2), 2) +
             cos(first[LAT]) * cos(second[LAT]) *
                 pow(sin((second[LON] - second[LAT]) / 2), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

double calc_speed(double *first, double *second) {
  return haverine_distance(first, second) / (second[EPOCH] - first[EPOCH]);
}

double angle_degree(double first_x, double first_y, double second_x,
                    double second_y) {
  return atan2(second_y - first_y, second_x - first_x) * 180 / M_PI;
}

double calc_angle_variance(double **first, size_t len, double *second,
                           size_t window_size) {
  size_t angle_window = window_size > len ? len : window_size;

  double *angles = (double *)malloc(sizeof(double) * (angle_window));

#pragma omp parallel for shared(angles)
  for (size_t i = len - angle_window; i < len - 1; i++) {
    angles[i] = angle_degree(first[i][LON], first[i][LAT], first[i + 1][LON],
                             first[i + 1][LAT]);
  }

  angles[angle_window - 1] = angle_degree(
      first[len - 1][LON], first[len - 1][LAT], second[LON], second[LAT]);

  double sum = 0;
#pragma omp parallel for shared(angles)
  for (size_t i = 0; i < angle_window; i++) {
    sum += angles[i];
  }

  double mean = sum / angle_window;

  double sqDiff = 0;
#pragma omp parallel for shared(angles, mean)
  for (size_t i = 0; i < angle_window; i++) {
    sqDiff += pow(angles[i] - mean, 2);
  }

  free(angles);

  return sqDiff;
}

void merge_clusters(int *clusters, size_t len, int first, int second) {
#pragma omp parallel for shared(clusters)
  for (size_t i = 0; i < len; i++) {
    if (clusters[i] == second) {
      clusters[i] = first;
    }
  }
}

int *arange(size_t len) {
  int *arr = (int *)malloc(sizeof(int) * len);

#pragma omp parallel for shared(arr)
  for (size_t i = 0; i < len; i++) {
    arr[i] = i;
  }

  return arr;
}

uint8_t find_compatible_clusters(double **data, size_t height,
                                 double distance_threshold,
                                 double angle_variance_threshold,
                                 double speed_top_threshold,
                                 double speed_min_threshold, size_t window_size,
                                 cluster_t *clusters) {}

cluster_t *create_base_array(size_t len) {
  cluster_t *clusters = (cluster_t *)malloc(sizeof(cluster_t) * len);

#pragma omp parallel for shared(clusters)
  for (size_t i = 0; i < len; i++) {
    clusters[i].len = 1;
    clusters[i].cluster_id = i;
    clusters[i].indices = (int *)malloc(sizeof(int) * 1);
    clusters[i].indices[0] = i;
  }

  return clusters;
}

void free_all_clusters(cluster_t *clusters, size_t len) {
#pragma omp parallel for shared(clusters)
  for (size_t i = 0; i < len; i++) {
    free(clusters[i].indices);
  }

  free(clusters);
}