#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <omp.h>

/**
 * @brief function to cluster points based on density and degree
 * 
 * @param data:double** array containing first_dim points with second_dim features 
 * @param first_dim:size_t number of points
 * @param second_dim:size_t number of features for each point
 * @param eps:double the maximum distance between two points needed for them to be considered neighbors
 * @param min_samples:uint32_t the minimum number of related points needed to create a cluster
 * @param rollback:size_t length of window to look back at when computing the degree 
 * @return int* array containing marks
 */
int* dbscan_with_degree(double** data, size_t first_dim, size_t second_dim, double eps, uint32_t min_samples, size_t rollback);
/**
 * @brief function to calculate the degree difference in correlation to the previous element in the cluster
 * 
 * @param cluster:double** current cluster containing grouped elements
 * @param cluster_elements:size_t number of elements in the cluster 
 * @param new_element:double* element to compare to existing cluster 
 * @param element_size:size_t number of columns in the row 
 * @param rollback_number:size_t number of elements to go back in the cluster (if smaller than cluster_elements the whole cluster will be use)
 * @return double containing the degree distance between the element and the cluster
 */
double degree_distance(double** cluster, size_t cluster_elements, double* new_element, size_t element_size, size_t rollback_number);
/**
 * @brief function to calculate the euclidean distance between two elements (parallelized)
 * 
 * @param first:double* array of data (one line)
 * @param second:double* array of data (one line) 
 * @param size:size_t number of columns in the line 
 * @return double containing distance of the two elements 
 */
double distance(double* first, double* second, size_t size);
/**
 * @brief function to get the k nearest neighbors
 * 
 * @param element:double* element to get neighbors for 
 * @param data:double** neighbors 
 * @param first_dim:size_t number of neighbors 
 * @param second_dim:size_t number of fearutes for every neighbor 
 * @param r:double the maximum distance between two points needed for them to be considered neighbors
 * @param len:size_t* return length of result
 * @return int* array containning indexes
 */
int* radiusnn(double* element, double** data, size_t first_dim, size_t second_dim, double r, size_t* len);
/**
 * @brief function to mark indexes as the same clusters
 * 
 * @param clusters:int* array of the cluster ids 
 * @param clusters_len:size_t number of elements in clusters array
 * @param indexes_to_mark:int* array of the indexes to mark as the same cluster 
 * @param indexes_len:size_t number of elements in indexes array 
 * @param clusterid:int id to mark all indexes 
 */
void mark_cluster(int* clusters, size_t clusters_len, int* indexes_to_mark, size_t indexes_len, int clusterid);
/**
 * @brief function to find and mark the neighbors recursively
 * 
 * @param clusters:int* array containing the cluster markings 
 * @param element_index:size_t index of the current element in inspection 
 * @param data:double** array containing all the data 
 * @param first_dim:size_t number of points 
 * @param second_dim:size_t number of features for each point 
 * @param eps:double the maximum distance between two points needed for them to be considered neighbors
 * @param min_samples:uint32_t the minimum number of related points needed to create a cluster  
 * @param rollback:size_t the number of point to look back at when calculating the degree 
 * @param curr_clusterid:int the current iteration mark 
 * @return uint8_t 
 */
uint8_t dbscan_recursion(int* clusters, size_t element_index, double** data, size_t first_dim, size_t second_dim, double eps, uint32_t min_samples, size_t rollback, int curr_clusterid);
/**
 * @brief fucntion to concatenate two arrays
 * 
 * @param first:int* first array 
 * @param first_size:size_t first array size 
 * @param second:int* second array 
 * @param second_size:size_t second array size 
 * @return int status (-1 if failed or 1 if ok) 
 */
size_t concat_arrays(int** first, size_t first_size, int* second, size_t second_size);
/**
 * @brief 
 * 
 * @param neighbor 
 * @param len 
 */
void print_neighbor(int* neighbor, size_t len);
/**
 * @brief 
 * 
 * @param array 
 * @param len 
 * @param element 
 * @return uint8_t 
 */
uint8_t check_if_exists_in_array(int* array, size_t len, int element);