#include "DBSCAN.h"

int* DBSCAN(double* data, size_t first_dim, size_t second_dim, double eps, int min_samples, int time_index) {
    // TODO: Implement this function
}

double degree_distance(double* cluster, size_t cluster_elements, double* new_element, size_t element_size) {
    if (element_size < 2) {
        return 0.0;
    }

    double prev_degree = 0.0;
    double next_degree = 0.0;
    double delta_x = 0.0;
    double delta_y = 0.0;

    delta_x = cluster[(cluster_elements - 1) * cluster_elements + 0] - cluster[(cluster_elements - 2) * cluster_elements + 0];
    delta_y = cluster[(cluster_elements - 1) * cluster_elements + 1] - cluster[(cluster_elements - 2) * cluster_elements + 1];

    prev_degree = atan(delta_y / delta_x);

    delta_x = new_element[0] - cluster[(cluster_elements - 1) * cluster_elements + 0];
    delta_y = new_element[1] - cluster[(cluster_elements - 1) * cluster_elements + 1];

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