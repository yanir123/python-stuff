#include "agglomerative.h"

void print_array(double *arr, size_t len) {
  printf("{");

  for (size_t i = 0; i < len; i++) {
    printf("%f", arr[i]);

    if (i + 1 != len) {
      printf(", ");
    }
  }

  printf("}\n");
}

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
        speed_top_threshold, speed_min_threshold, window_size, &clusters_array,
        &cluster_len);
  }

  int *res = get_cluster_array_with_origininal_indices(clusters_array,
                                                       cluster_len, height);
  free_all_clusters(clusters_array, cluster_len);
  return res;
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
  return atan2(second_y - first_y, second_x - first_x) * 180 / M_PI;
}

double calc_angle_variance(double **data, cluster_t first, cluster_t second,
                           size_t window_size) {
  size_t first_angle_window = window_size > first.len ? first.len : window_size;
  size_t second_angle_window =
      window_size > second.len ? second.len : window_size;
  size_t angles_number = first_angle_window + second_angle_window - 1;

  double *angles = (double *)malloc(sizeof(double) * (angles_number));

#pragma omp parallel for
  for (size_t i = first.len - first_angle_window; i < first_angle_window - 1;
       i++) {
    angles[i - (first.len - first_angle_window)] = angle_degree(
        data[first.indices[i]][LON], data[first.indices[i]][LAT],
        data[first.indices[i + 1]][LON], data[first.indices[i + 1]][LAT]);
  }

  angles[first_angle_window - 1] =
      angle_degree(data[first.indices[first.len - 1]][LON],
                   data[first.indices[first.len - 1]][LAT],
                   data[second.indices[0]][LON], data[second.indices[0]][LAT]);

#pragma omp parallel for
  for (size_t i = 0; i < second_angle_window - 1; i++) {
    angles[i + first_angle_window] = angle_degree(
        data[second.indices[i]][LON], data[second.indices[i]][LAT],
        data[second.indices[i + 1]][LON], data[second.indices[i + 1]][LAT]);
  }

  double sum = 0;
#pragma omp parallel for shared(angles)
  for (size_t i = 0; i < angles_number; i++) {
    sum += angles[i];
  }

  double mean = sum / angles_number;

  double sqDiff = 0;
#pragma omp parallel for shared(angles, mean)
  for (size_t i = 0; i < angles_number; i++) {
    sqDiff += pow(angles[i] - mean, 2);
  }

  free(angles);

  return sqDiff / angles_number;
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

void free_cluster(cluster_t cluster) { free(cluster.indices); }

void add_cluster_and_remove_old_ones(cluster_t **clusters, size_t *len,
                                     cluster_t new_cluster, size_t first_index,
                                     size_t second_index) {
  cluster_t *new_clusters =
      (cluster_t *)malloc(sizeof(cluster_t) * ((*len) - 1));

#pragma omp parallel for
  for (size_t i = 0; i < second_index; i++) {
    if (i == first_index) {
      new_clusters[i] = new_cluster;
    } else {
      new_clusters[i] = (*clusters)[i];
    }
  }

#pragma omp parallel for
  for (size_t i = second_index; i < (*len) - 1; i++) {
    new_clusters[i] = (*clusters)[i + 1];
  }

  free_cluster((*clusters)[first_index]);
  free_cluster((*clusters)[second_index]);
  free(*clusters);

  *clusters = new_clusters;
  (*len)--;
}

uint8_t find_compatible_clusters(double **data, size_t height,
                                 double distance_threshold,
                                 double angle_variance_threshold,
                                 double speed_top_threshold,
                                 double speed_min_threshold, size_t window_size,
                                 cluster_t **clusters, size_t *cluster_len) {
  for (size_t i = 0; i < (*cluster_len) - 1; i++) {
    for (size_t j = i + 1; j < *cluster_len; j++) {
      if (check_compatibility(data, (*clusters)[i], (*clusters)[j],
                              distance_threshold, angle_variance_threshold,
                              speed_top_threshold, speed_min_threshold,
                              window_size)) {
        cluster_t new_cluster = merge_clusters((*clusters)[i], (*clusters)[j]);
        add_cluster_and_remove_old_ones(clusters, cluster_len, new_cluster, i,
                                        j);
        return TRUE;
      }
    }
  }

  return FALSE;
}

uint8_t check_compatibility(double **data, cluster_t first, cluster_t second,
                            double distance_threshold,
                            double angle_variance_threshold,
                            double speed_top_threshold,
                            double speed_min_threshold, size_t window_size) {
  double *first_cluster_last_element = data[first.indices[first.len - 1]];
  double *second_cluster_first_element = data[second.indices[0]];
  double distance = haversine_distance(first_cluster_last_element,
                                       second_cluster_first_element);
  double speed = calc_speed(first_cluster_last_element,
                            second_cluster_first_element, distance);
  double angle_variance = calc_angle_variance(data, first, second, window_size);

  return distance < distance_threshold &&
         angle_variance < angle_variance_threshold &&
         speed < speed_top_threshold && speed > speed_min_threshold;
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