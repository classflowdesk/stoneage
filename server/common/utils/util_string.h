#ifndef __COMMON_UTILS_UTIL_STRING_H__
#define __COMMON_UTILS_UTIL_STRING_H__

#include "common.h"

void easyGetTokenFromString(char *src, int count, char *output, int len);
char *makeStringFromEscaped(char *src);
char *makeEscapeString(const char *src, char *dst, const int dst_len);
int getHash(const char *s);

char *chop(char *src);
// src的尾字符是否在del_str中，若是，则删除
char *dchop(char *src, const char *del_str);
// 删除尾部回车
#define chomp(src) dchop(src, "\n")
// 删除串中所有的\r和\n
char *chmopex(char *src);
// src的头字符是否在del_str中，若是，则删除
char *pohcd(char *src, const char *del_str);

int hashpjw(const char *s);

int easyGetTokenFromBuf(const char *src, const char *delim, const int count,
                        char *output, const int len);

double time_diff(struct timeval t1, struct timeval t2);
unsigned time_diff_us(struct timeval t1, struct timeval t2);

#endif /* __COMMON_UTILS_UTIL_STRING_H__ */
