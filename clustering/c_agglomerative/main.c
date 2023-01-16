#include <Windows.h>
#include <stdint.h>
#include <time.h>

#define WIN32_LEAN_AND_MEAN

int gettimeofday(struct timeval* tp, struct timezone* tzp) {
  // Note: some broken versions only have 8 trailing zero's, the correct epoch
  // has 9 trailing zero's This magic number is the number of 100 nanosecond
  // intervals since January 1, 1601 (UTC) until 00:00:00 January 1, 1970
  static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

  SYSTEMTIME system_time;
  FILETIME file_time;
  uint64_t time;

  GetSystemTime(&system_time);
  SystemTimeToFileTime(&system_time, &file_time);
  time = ((uint64_t)file_time.dwLowDateTime);
  time += ((uint64_t)file_time.dwHighDateTime) << 32;

  tp->tv_sec = (long)((time - EPOCH) / 10000000L);
  tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
  return 0;
}

#include "agglomerative.h"

int main(int argc, char** argv, char** wenv) {
  size_t len = 100;
  struct timeval start, end;

  double farr[2] = {1, 2};
  double sarr[2] = {3, 4};
  double seconda[2] = {7, 9};
  double secondb[2] = {12, 13};
  double* arr[4] = {farr, sarr, seconda, secondb};

  int first_index[2] = {0, 1};
  int second_index[2] = {2, 3};

  cluster_t first = {1, 2, first_index};
  cluster_t second = {2, 2, second_index};

  gettimeofday(&start, 0);
  double var = calc_angle_variance(arr, first, second, 2);
  gettimeofday(&end, 0);

  printf("var is: %f\n", var);

  printf("executed in :%.20f microseconds\n",
         ((double)(end.tv_usec - start.tv_usec)) / CLOCKS_PER_SEC);

  return 0;
}