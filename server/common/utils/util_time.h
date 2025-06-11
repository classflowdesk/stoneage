#ifndef __COMMON_UTILS_UTIL_TIME_H__
#define __COMMON_UTILS_UTIL_TIME_H__

#include <sys/time.h>

double time_diff(struct timeval t1, struct timeval t2);
unsigned time_diff_us(struct timeval t1, struct timeval t2);

#endif  
