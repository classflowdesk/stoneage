#include "util_time.h"

double time_diff(struct timeval t1, struct timeval t2) {
  return ((t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) / (double)1E6);
}

unsigned time_diff_us(struct timeval t1, struct timeval t2) {
  return (t1.tv_sec - t2.tv_sec) * 1000000 + (t1.tv_usec - t2.tv_usec);
}