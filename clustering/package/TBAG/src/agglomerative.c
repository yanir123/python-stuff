#include "agglomerative.h"

void agglomerative_clustering(double **data, unsigned int height,
                              double distance_threshold, double time_threshold,
                              double angle_diff_threshold,
                              double speed_top_threshold,
                              double speed_min_threshold,
                              unsigned int window_size, int *res) {
  cluster_t *clusters_array = (cluster_t *)malloc(0);
  unsigned int cluster_len = 0;

  for (unsigned int i = 0; i < height; i++) {
    int cluster_loc = find_closest_compatible_cluster(
        data, height, clusters_array, cluster_len, i, distance_threshold,
        time_threshold, angle_diff_threshold, speed_top_threshold,
        speed_min_threshold, window_size);
    if (cluster_loc != -1) {
      add_to_cluster(clusters_array, (unsigned int)cluster_loc, i);
    } else {
      add_new_cluster(&clusters_array, &cluster_len, i);
    }
  }

  get_cluster_array_with_origininal_indices(clusters_array, cluster_len, height,
                                            res);
  free_all_clusters(clusters_array, cluster_len);
}

void print_array_res(int *arr, unsigned int len) {
  printf("{");

  for (unsigned int i = 0; i < len; i++) {
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
  return sqrt(pow(R * c, 2) + pow(second[ALT] - first[ALT], 2));
}

double calc_speed(double *first, double *second, double distance) {
  return distance / (second[EPOCH] - first[EPOCH]);
}

double angle_degree(double first_x, double first_y, double second_x,
                    double second_y) {
  return atan2(second_y - first_y, second_x - first_x) * 180 / PI;
}

double calc_angle_diff(double **data, cluster_t first, unsigned int second,
                       unsigned int window_size) {
  unsigned int first_angle_window =
      window_size > first.len ? first.len : window_size;

  double general_degree = angle_degree(
      data[first.indices[first.len - first_angle_window]][LON],
      data[first.indices[first.len - first_angle_window]][LAT],
      data[first.indices[first.len]][LON], data[first.indices[first.len]][LAT]);

  double new_degree = angle_degree(
      data[first.indices[first.len - first_angle_window]][LON],
      data[first.indices[first.len - first_angle_window]][LAT],
      data[first.indices[second]][LON], data[first.indices[second]][LAT]);

  return fabs(general_degree - new_degree);
}

void free_cluster(cluster_t cluster) { free(cluster.indices); }

void add_cluster_and_remove_old_ones(cluster_t **clusters, unsigned int *len,
                                     cluster_t new_cluster,
                                     unsigned int first_index,
                                     unsigned int second_index) {
  cluster_t *new_clusters =
      (cluster_t *)malloc(sizeof(cluster_t) * ((*len) - 1));
  unsigned int smaller_index = fmin(first_index, second_index);
  unsigned int bigger_index = fmax(first_index, second_index);

  for (unsigned int i = 0; i < bigger_index; i++) {
    if (i == smaller_index) {
      new_clusters[i] = new_cluster;
    } else {
      new_clusters[i] = (*clusters)[i];
    }
  }

  for (unsigned int i = bigger_index; i < (*len) - 1; i++) {
    new_clusters[i] = (*clusters)[i + 1];
  }

  free_cluster((*clusters)[first_index]);
  free_cluster((*clusters)[second_index]);
  free(*clusters);

  *clusters = new_clusters;
  (*len)--;
}

double cluster_distance(double **data, cluster_t first, unsigned int second) {
  return haversine_distance(data[first.indices[first.len - 1]], data[second]);
}

uint8_t check_compatibility(double **data, cluster_t first, unsigned int second,
                            double distance_threshold, double time_threshold,
                            double angle_diff_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold,
                            unsigned int window_size) {
  double *first_cluster_last_element = data[first.indices[first.len - 1]];
  double *second_element = data[second];
  double distance =
      haversine_distance(first_cluster_last_element, second_element);
  double speed =
      calc_speed(first_cluster_last_element, second_element, distance);
  double angle_variance = calc_angle_diff(data, first, second, window_size);
  double time_diff =
      fabs(second_element[EPOCH] - first_cluster_last_element[EPOCH]);

  return distance < distance_threshold &&
         angle_variance < angle_diff_threshold && speed < speed_top_threshold &&
         speed > speed_min_threshold && time_diff < time_threshold;
}

void free_all_clusters(cluster_t *clusters, unsigned int len) {
  for (unsigned int i = 0; i < len; i++) {
    free(clusters[i].indices);
  }

  free(clusters);
}

void get_cluster_array_with_origininal_indices(cluster_t *clusters,
                                               unsigned int len,
                                               unsigned int number_of_points,
                                               int *res) {
  for (unsigned int i = 0; i < len; i++) {
    for (unsigned int j = 0; j < clusters[i].len; j++) {
      res[clusters[i].indices[j]] = i;
    }
  }
}

void add_new_cluster(cluster_t **clusters_array, unsigned int *cluster_len,
                     unsigned int index) {
  cluster_t *temp =
      (cluster_t *)malloc(sizeof(cluster_t) * ((*cluster_len) + 1));

  for (unsigned int i = 0; i < *cluster_len; i++) {
    temp[i] = (*clusters_array)[i];
  }

  cluster_t new_cluster;
  new_cluster.len = 1;
  new_cluster.indices =
      (unsigned int *)malloc(sizeof(unsigned int) * new_cluster.len);
  new_cluster.indices[0] = index;

  temp[*cluster_len] = new_cluster;

  free(*clusters_array);
  *clusters_array = temp;
  (*cluster_len)++;
}

void add_to_cluster(cluster_t *clusters_array, unsigned int cluster_index,
                    unsigned int index) {
  unsigned int *temp = (unsigned int *)malloc(
      sizeof(unsigned int) * (clusters_array[cluster_index].len + 1));
  for (unsigned int i = 0; i < clusters_array[cluster_index].len; i++) {
    temp[i] = clusters_array[cluster_index].indices[i];
  }

  temp[clusters_array[cluster_index].len] = index;
  free(clusters_array[cluster_index].indices);

  clusters_array[cluster_index].len++;
  clusters_array[cluster_index].indices = temp;
}

int find_closest_compatible_cluster(
    double **data, unsigned int height, cluster_t *clusters_array,
    unsigned int cluster_len, unsigned int index, double distance_threshold,
    double time_threshold, double angle_diff_threshold,
    double speed_top_threshold, double speed_min_threshold,
    unsigned int window_size) {
  int min_index = -1;
  double min_value = INFINITY;

  for (unsigned int i = 0; i < cluster_len; i++) {
    double distance = cluster_distance(data, clusters_array[i], index);

    if (distance < min_value &&
        check_compatibility(data, clusters_array[i], index, distance_threshold,
                            time_threshold, angle_diff_threshold,
                            speed_top_threshold, speed_min_threshold,
                            window_size)) {
      min_index = i;
      min_value = min_value;
    }
  }

  return min_index;
}