#include "agglomerative.h"

int *agglomerative_clustering_single(double **data, size_t height,
                                     double distance_threshold,
                                     double angle_variance_threshold,
                                     double speed_top_threshold,
                                     double speed_min_threshold,
                                     size_t window_size) {
  cluster_t *clusters_array = create_base_array(height);
  size_t cluster_len = height;
  uint8_t updated = TRUE;

  while (updated) {
    updated = find_compatible_clusters(
        data, height, distance_threshold, angle_variance_threshold,
        speed_top_threshold, speed_min_threshold, window_size, clusters_array,
        &cluster_len);
  }

  return get_cluster_array_with_origininal_indices(clusters_array, cluster_len,
                                                   height);
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

cluster_t merge_clusters(cluster_t first, cluster_t second) {
  int *temp = (int *)malloc(sizeof(int) * (first.len + second.len));

#pragma omp parallel for shared(temp, first)
  for (size_t i = 0; i < first.len; i++) {
    temp[i] = first.indices[i];
  }

#pragma omp parallel for shared(temp, second)
  for (size_t i = 0; i < second.len; i++) {
    temp[i + first.len] = second.indices[i];
  }

  cluster_t res = {first.cluster_id, first.len + second.len, temp};

  return res;
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
                                 cluster_t *clusters, size_t *cluster_len) {
  // TODO: Implement this
}

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

int *get_cluster_array_with_origininal_indices(cluster_t *clusters, size_t len,
                                               size_t number_of_points) {
  int *cluster_ids = (int *)malloc(sizeof(int) * number_of_points);

#pragma omp parallel for shared(clusters, cluster_ids) collapse(2)
  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < clusters[i].len; j++) {
      cluster_ids[clusters[i].indices[j]] = clusters[i].cluster_id;
    }
  }

  return cluster_ids;
}