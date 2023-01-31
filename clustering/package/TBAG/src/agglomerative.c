#include "agglomerative.h"

void agglomerative_clustering(double **data, size_t height,
                              double distance_threshold, double time_threshold,
                              double angle_variance_threshold,
                              double speed_top_threshold,
                              double speed_min_threshold, size_t window_size,
                              int *res) {
  cluster_t *clusters_array = (cluster_t *)malloc(0);
  size_t cluster_len = 0;

  for (size_t i = 0; i < height; i++) {
    int cluster_loc = find_closest_compatible_cluster(
        data, height, clusters_array, cluster_len, i, distance_threshold,
        time_threshold, angle_variance_threshold, speed_top_threshold,
        speed_min_threshold, window_size);
    if (cluster_loc != -1) {
      add_to_cluster(clusters_array, (size_t)cluster_loc, i);
    } else {
      add_new_cluster(&clusters_array, &cluster_len, i);
    }
  }

  get_cluster_array_with_origininal_indices(clusters_array, cluster_len, height,
                                            res);
  free_all_clusters(clusters_array, cluster_len);
}

void print_array_res(int *arr, size_t len) {
  printf("{");

  for (size_t i = 0; i < len; i++) {
    printf("%d", arr[i]);

    if (i + 1 != len) {
      printf(", ");
    }
  }

  printf("}\n");
}

double haversine_distance(double *first, double *second) {
  double a = pow(sin((second[LAT] - first[LAT]) / 2), 2) +
             cos(first[LAT]) * cos(second[LAT]) *
                 pow(sin((second[LON] - first[LON]) / 2), 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c;
}

double calc_speed(double *first, double *second, double distance) {
  return distance / (second[EPOCH] - first[EPOCH]);
}

double angle_degree(double first_x, double first_y, double second_x,
                    double second_y) {
  return atan2(second_y - first_y, second_x - first_x) * 180 / PI;
}

double calc_angle_variance(double **data, cluster_t first, size_t second,
                           size_t window_size) {
  size_t first_angle_window = window_size > first.len ? first.len : window_size;

  double *angles = (double *)malloc(sizeof(double) * (first_angle_window));

  for (size_t i = first.len - first_angle_window; i < first_angle_window - 1;
       i++) {
    angles[i - (first.len - first_angle_window)] = angle_degree(
        data[first.indices[i]][LON], data[first.indices[i]][LAT],
        data[first.indices[i + 1]][LON], data[first.indices[i + 1]][LAT]);
  }

  angles[first_angle_window - 1] =
      angle_degree(data[first.indices[first.len - 1]][LON],
                   data[first.indices[first.len - 1]][LAT], data[second][LON],
                   data[second][LAT]);

  double sum = 0;
  for (size_t i = 0; i < first_angle_window; i++) {
    sum += angles[i];
  }

  double mean = sum / first_angle_window;

  double sqDiff = 0;
  for (size_t i = 0; i < first_angle_window; i++) {
    sqDiff += pow(angles[i] - mean, 2);
  }

  free(angles);

  return sqDiff / first_angle_window;
}

void free_cluster(cluster_t cluster) { free(cluster.indices); }

void add_cluster_and_remove_old_ones(cluster_t **clusters, size_t *len,
                                     cluster_t new_cluster, size_t first_index,
                                     size_t second_index) {
  cluster_t *new_clusters =
      (cluster_t *)malloc(sizeof(cluster_t) * ((*len) - 1));
  size_t smaller_index = fmin(first_index, second_index);
  size_t bigger_index = fmax(first_index, second_index);

  for (size_t i = 0; i < bigger_index; i++) {
    if (i == smaller_index) {
      new_clusters[i] = new_cluster;
    } else {
      new_clusters[i] = (*clusters)[i];
    }
  }

  for (size_t i = bigger_index; i < (*len) - 1; i++) {
    new_clusters[i] = (*clusters)[i + 1];
  }

  free_cluster((*clusters)[first_index]);
  free_cluster((*clusters)[second_index]);
  free(*clusters);

  *clusters = new_clusters;
  (*len)--;
}

double cluster_distance(double **data, cluster_t first, size_t second) {
  return haversine_distance(data[first.indices[first.len - 1]], data[second]);
}

uint8_t check_compatibility(double **data, cluster_t first, size_t second,
                            double distance_threshold, double time_threshold,
                            double angle_variance_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold, size_t window_size) {
  double *first_cluster_last_element = data[first.indices[first.len - 1]];
  double *second_element = data[second];
  double distance =
      haversine_distance(first_cluster_last_element, second_element);
  double speed =
      calc_speed(first_cluster_last_element, second_element, distance);
  double angle_variance = calc_angle_variance(data, first, second, window_size);
  double time_diff =
      fabs(second_element[EPOCH] - first_cluster_last_element[EPOCH]);

  return distance < distance_threshold &&
         angle_variance < angle_variance_threshold &&
         speed < speed_top_threshold && speed > speed_min_threshold &&
         time_diff < time_threshold;
}

void free_all_clusters(cluster_t *clusters, size_t len) {
  for (size_t i = 0; i < len; i++) {
    free(clusters[i].indices);
  }

  free(clusters);
}

void get_cluster_array_with_origininal_indices(cluster_t *clusters, size_t len,
                                               size_t number_of_points,
                                               int *res) {
  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0; j < clusters[i].len; j++) {
      res[clusters[i].indices[j]] = i;
    }
  }
}

void add_new_cluster(cluster_t **clusters_array, size_t *cluster_len,
                     size_t index) {
  cluster_t *temp =
      (cluster_t *)malloc(sizeof(cluster_t) * ((*cluster_len) + 1));

  for (size_t i = 0; i < *cluster_len; i++) {
    temp[i] = (*clusters_array)[i];
  }

  cluster_t new_cluster;
  new_cluster.len = 1;
  new_cluster.indices = (size_t *)malloc(sizeof(size_t) * new_cluster.len);
  new_cluster.indices[0] = index;

  temp[*cluster_len] = new_cluster;

  free(*clusters_array);
  *clusters_array = temp;
  (*cluster_len)++;
}

void add_to_cluster(cluster_t *clusters_array, size_t cluster_index,
                    size_t index) {
  size_t *temp = (size_t *)malloc(sizeof(size_t) *
                                  (clusters_array[cluster_index].len + 1));
  for (size_t i = 0; i < clusters_array[cluster_index].len; i++) {
    temp[i] = clusters_array[cluster_index].indices[i];
  }

  temp[clusters_array[cluster_index].len] = index;
  free(clusters_array[cluster_index].indices);

  clusters_array[cluster_index].len++;
  clusters_array[cluster_index].indices = temp;
}

int find_closest_compatible_cluster(
    double **data, size_t height, cluster_t *clusters_array, size_t cluster_len,
    size_t index, double distance_threshold, double time_threshold,
    double angle_variance_threshold, double speed_top_threshold,
    double speed_min_threshold, size_t window_size) {
  int min_index = -1;
  double min_value = INFINITY;

  for (size_t i = 0; i < cluster_len; i++) {
    double distance = cluster_distance(data, clusters_array[i], index);

    if (distance < min_value &&
        check_compatibility(data, clusters_array[i], index, distance_threshold,
                            time_threshold, angle_variance_threshold,
                            speed_top_threshold, speed_min_threshold,
                            window_size)) {
      min_index = i;
      min_value = min_value;
    }
  }

  return min_index;
}