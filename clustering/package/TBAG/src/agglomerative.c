#include "agglomerative.h"

void agglomerative_clustering(double **data, unsigned int height,
                              double distance_threshold, double time_threshold,
                              double angle_diff_threshold,
                              double speed_diff_threshold,
                              unsigned int window_size, int *res) {
  cluster_t *clusters_array = (cluster_t *)malloc(0);
  unsigned int cluster_len = 0;

  for (unsigned int i = 0; i < height; i++) {
    int cluster_loc = find_closest_compatible_cluster(
        data, height, clusters_array, cluster_len, i, distance_threshold,
        time_threshold, angle_diff_threshold, speed_diff_threshold,
        window_size);
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

double calc_speed(double *first, double *second) {
  double time_diff = (second[EPOCH] - first[EPOCH]);

  if (time_diff == 0) {
    return 0;
  }

  return haversine_distance(first, second) / time_diff;
}

double angle_degree(double *first, double *second) {
  return atan2(second[LAT] - first[LAT], second[LON] - first[LON]) * 180 / PI;
}

double calc_speed_diff(double **data, cluster_t first, unsigned int second,
                       unsigned int window_size) {
  return calc_diff(data, first, second, window_size, calc_speed);
}

double calc_angle_diff(double **data, cluster_t first, unsigned int second,
                       unsigned int window_size) {
  return calc_diff(data, first, second, window_size, angle_degree);
}

void mean_between(double **data, cluster_t cluster, double *res_mean,
                  unsigned int start, unsigned int end) {
  for (unsigned int i = start; i < end; i++) {
    for (unsigned int j = 0; j < WIDTH; j++) {
      res_mean[j] += data[cluster.indices[i]][j] / (end - start);
    }
  }
}

double calc_diff(double **data, cluster_t first, unsigned int second,
                 unsigned int window_size,
                 double (*diff_func)(double *, double *)) {
  unsigned int first_angle_window =
      window_size > first.len ? first.len : window_size;

  unsigned int start = first.len - first_angle_window;
  unsigned int end = first.len - 1;
  unsigned int middle = start + (end - start) / 2;

  double first_half_mean[WIDTH] = {0};
  double second_half_mean[WIDTH] = {0};

  mean_between(data, first, first_half_mean, start, middle);
  mean_between(data, first, second_half_mean, middle, end);

  double general_diff = diff_func(first_half_mean, second_half_mean);

  double new_diff = diff_func(first_half_mean, data[second]);

  return fabs(general_diff - new_diff);
}

uint8_t check_compatibility(double **data, cluster_t first, unsigned int second,
                            double distance_threshold, double time_threshold,
                            double angle_diff_threshold,
                            double speed_diff_threshold,
                            unsigned int window_size, double *res_haversine,
                            double *res_angle) {
  double *first_cluster_last_element = data[first.indices[first.len - 1]];
  double *second_element = data[second];
  *res_haversine =
      haversine_distance(first_cluster_last_element, second_element);
  double speed_diff = calc_speed_diff(data, first, second, window_size);
  *res_angle = calc_angle_diff(data, first, second, window_size);
  double time_diff =
      fabs(second_element[EPOCH] - first_cluster_last_element[EPOCH]);

  return *res_haversine < distance_threshold &&
         *res_angle < angle_diff_threshold &&
         speed_diff < speed_diff_threshold && time_diff < time_threshold;
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
    double speed_diff_threshold, unsigned int window_size) {
  int min_index = -1;
  double min_value = INFINITY;

  for (unsigned int i = 0; i < cluster_len; i++) {
    double haversine_distance = 0;
    double angle_ditstance = 0;

    if (check_compatibility(data, clusters_array[i], index, distance_threshold,
                            time_threshold, angle_diff_threshold,
                            speed_diff_threshold, window_size,
                            &haversine_distance, &angle_ditstance) &&
        sqrt(pow(haversine_distance, 2) + pow(angle_ditstance, 2)) <
            min_value) {
      min_index = i;
      min_value = min_value;
    }
  }

  return min_index;
}