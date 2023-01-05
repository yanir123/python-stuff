#pragma once

#include <stdlib.h>
#include <math.h>
#include <omp.h>

/**
 * @brief 
 * 
 * @param data 
 * @param first_dim 
 * @param second_dim 
 * @param eps 
 * @param min_samples 
 * @param time_index 
 * @return int* 
 */
int* DBSCAN(double* data, size_t first_dim, size_t second_dim, double eps, int min_samples, int time_index);
/**
 * @brief function to calculate the degree difference in correlation to the previous element in the cluster
 * 
 * @param cluster:double* current cluster containing grouped elements
 * @param cluster_elements:size_t number of elements in the cluster 
 * @param new_element:double* element to compare to existing cluster 
 * @param element_size:size_t number of columns in the row 
 * @return double containing the degree distance between the element and the cluster
 */
double degree_distance(double* cluster, size_t cluster_elements, double* new_element, size_t element_size);
/**
 * @brief function to calculate the euclidean distance between two elements (parallelized)
 * 
 * @param first:double* array of data (one line)
 * @param second:double* array of data (one line) 
 * @param size:size_t number of columns in the line 
 * @return double containing distance of the two elements 
 */
double distance(double* first, double* second, size_t size);
