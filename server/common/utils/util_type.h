#ifndef __COMMON_UTILS_UTIL_TYPE_H__
#define __COMMON_UTILS_UTIL_TYPE_H__

typedef struct tagSTRING32 {
  char string[32];
} STRING32;

typedef struct tagSTRING64 {
  char string[64];
} STRING64;

typedef struct tagSTRING128 {
  char string[128];
} STRING128;

typedef struct tagRECT {
  int x;
  int y;
  int width;
  int height;
} RECT;

typedef struct tagPOINT {
  int x;
  int y;
} POINT;

typedef enum { CHAR, SHORT, INT, DOUBLE } CTYPE;

#endif
