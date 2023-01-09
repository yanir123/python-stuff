#include "DBSCAN.h"

int* dbscan_with_degree(double** data, size_t first_dim, size_t second_dim, double eps, uint32_t min_samples, size_t rollback) {
    int* clusters = (int*)calloc(first_dim, sizeof(int));
    int curr_cluster = 0;

    for (size_t i = 0; i < first_dim; i++) {
        if (clusters[i]) {
            continue;
        }

        size_t neighbor_number = 0;
        int* neighbors = radiusnn(data[i], data, first_dim, second_dim, eps, &neighbor_number);

        if (neighbor_number < min_samples) {
            clusters[i] = -1;
            continue;
        }

        curr_cluster++;
        clusters[i] = curr_cluster;

        for (size_t j = 0; j < neighbor_number; j++) {
            if (clusters[neighbors[j]] < 0) {
                clusters[neighbors[j]] = curr_cluster;
                continue;
            }

            if (clusters[neighbors[j]]) {
                continue;
            }

            clusters[neighbors[j]] = curr_cluster;

            size_t new_neighbor_number = 0;
            int* new_neighbors = radiusnn(data[neighbors[j]], data, first_dim, second_dim, eps, &new_neighbor_number);

            if (new_neighbor_number >= min_samples) {
                new_neighbor_number = concat_arrays(&neighbors, neighbor_number, new_neighbors, new_neighbor_number);
                if (!new_neighbor_number) {
                    free(clusters);
                    return NULL;
                }

                neighbor_number = new_neighbor_number;
            }
        }

        free(neighbors);
    }

    return clusters;
}

void print_neighbor(int* neighbor, size_t len) {
    printf("[");
    for (size_t i = 0; i < len; i++) {
        printf("%d", neighbor[i]);
        printf(i == len - 1 ? "" : ",");
    }
    printf("]\n");
}

size_t concat_arrays(int** first, size_t first_size, int* second, size_t second_size) {
    int* temp = (int*)malloc(sizeof(int) * (first_size + second_size));

    size_t len = first_size;

    if (!temp) {
        puts("malloc failed");
        free(*first);
        free(second);
        return 0;
    }

    for (size_t i = 0; i < first_size; i++) {
        temp[i] = (*first)[i];
    }

    for (size_t i = 0; i < second_size; i++) {
        if (!check_if_exists_in_array(*first, first_size, second[i])) {
            len++;
            temp[len - 1] = second[i];
        }
    }

    int* new_array = temp;

    if (len > 0) {
        new_array = (int*)malloc(sizeof(int) * len);

        for (size_t i = 0; i < len; i++) {
            new_array[i] = temp[i];
        }
    }

    free(*first);
    free(second);
    free(temp);

    *first = new_array;
    return len;
}

uint8_t check_if_exists_in_array(int* array, size_t len, int element) {
    for (size_t i = 0; i < len; i++) {
        if (array[i] == element) {
            return 1;
        }
    }

    return 0;
}

double degree_distance(double** cluster, size_t cluster_elements, double* new_element, size_t element_size, size_t rollback_number) {
    if (element_size < 2 || cluster_elements == 0) {
        return 0.0;
    }

    if (rollback_number > cluster_elements) {
        rollback_number = cluster_elements;
    }

    double prev_degree = 0.0;
    double next_degree = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;

    #pragma omp parallel for reduction(+:prev_degree)
    for (size_t i = 0; i < rollback_number; i++) {
        delta_x = cluster[(cluster_elements - 1 - i)][0] - cluster[(cluster_elements - 2 - i)][0];
        delta_y = cluster[(cluster_elements - 1 - i)][1] - cluster[(cluster_elements - 2 - i)][1];

        prev_degree += atan(delta_y / delta_x);
    }

    prev_degree /= rollback_number;

    delta_x = new_element[0] - cluster[(cluster_elements - 1)][0];
    delta_y = new_element[1] - cluster[(cluster_elements - 1)][1];

    next_degree = atan(delta_y / delta_x);

    return sqrt(pow(next_degree - prev_degree, 2));
}

double distance(double* first, double* second, size_t size) {
    double sum = 0.0;

    #pragma omp parallel for reduction(+:sum)
    for (size_t i = 0; i < size; i++) {
        sum += pow(second[i] - first[i], 2);
    }

    return sqrt(sum); 
}

int* radiusnn(double* element, double** data, size_t first_dim, size_t second_dim, double r, size_t* len) {
    int* res = (int*)malloc(sizeof(int) * first_dim);
    size_t count = 0;

    if (!res) {
        puts("malloc failed");
        return NULL;
    }

    #pragma omp parallel for
    for (size_t i = 0; i < first_dim; i++) {
        double dist = distance(element, data[i], second_dim);
        if (dist <= r && dist) {
            count++;
            res[count - 1] = i;
        }
    }

    int* new_array = (int*)malloc(sizeof(int) * count);

    for (size_t i = 0; i < count; i++) {
        new_array[i] = res[i];
    }

    free(res);

    *len = count;
    return new_array;
}

void mark_cluster(int* clusters, size_t clusters_len, int* indexes_to_mark, size_t indexes_len, int clusterid) {
    #pragma omp parallel for
    for (size_t i = 0; i < indexes_len; i++) {
        if (indexes_to_mark[i] < clusters_len && clusters[indexes_to_mark[i]] <= 0) {
            clusters[indexes_to_mark[i]] = clusterid;
        }
    }
}