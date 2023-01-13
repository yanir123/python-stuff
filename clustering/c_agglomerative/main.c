#include <stdio.h>
#include <sys/time.h>

#include "agglomerative.h"

void print_array(int* arr, size_t len) {
  printf("{");

  for (size_t i = 0; i < len; i++) {
    printf("%d", arr[i]);

    if (i + 1 != len) {
      printf(", ");
    }
  }

  printf("}\n");
}

int main(int argc, char** argv, char** wenv) {
  size_t len = 100;
  struct timeval start, end;

  // double farr[2] = {1, 2};
  // double sarr[2] = {3, 4};
  // double* arr[2] = {farr, sarr};
  // double second[2] = {7, 9};

  gettimeofday(&start, 0);
  cluster_t* arr = create_base_array(len);
  gettimeofday(&end, 0);

  print_array(arr[50].indices, arr[50].len);
  free_all_clusters(arr, len);

  printf("executed in :%.20f microseconds\n",
         ((double)(end.tv_usec - start.tv_usec)) / CLOCKS_PER_SEC);

  return 0;
}