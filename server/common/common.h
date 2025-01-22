#ifndef __COMMON_COMMON_H__
#define __COMMON_COMMON_H__

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <dirent.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <strings.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#define RETURN_FALSE_IF_FALSE(x)                                               \
  {                                                                            \
    if (!x)                                                                    \
      return FALSE;                                                            \
  }
#define EXIT_WITH_CODE_IF_FALSE(x, code)                                       \
  {                                                                            \
    if (!x)                                                                    \
      exit(code);                                                              \
  }

#define debug(x, y) fprintf(stderr, #x " = %" #y "\n", x)
#define arraysizeof(x) (sizeof(x) / sizeof(x[0]))
#define errorprint                                                             \
  {                                                                            \
    extern int errno;                                                          \
    fprint("%s\n", strerror(errno));                                           \
  }

#define logErr(format, args...) fprintf(stderr, format, ##args)
#define logOut(format, args...) fprintf(stdout, format, ##args)
#define logFile(filename, format, args...)                                     \
  {                                                                            \
    FILE *f;                                                                   \
    f = fopen(filename, "a");                                                  \
    if (f != NULL) {                                                           \
      fprintf(f, format, ##args);                                              \
      fclose(f);                                                               \
    } else {                                                                   \
      fprintf(stderr, "cannot open file %s. reason:%s\n", filename,            \
              strerror(errno));                                                \
    }                                                                          \
  }
#ifdef __GNUC__
#define print(format, arg...) fprintf(stderr, format, ##arg)
#define fprint(format, arg...)                                                 \
  fprintf(stderr, "%s:%d:", format, __FILE__, __LINE__, ##arg)
#endif // __GUNC__

#define BACKSLASH '\\'
#define NEWLINE '\n'
#define TAB '\t'
#define SPACE ' '
//
#define SUCCESSFUL "successful"
#define FAILED "failed"
#define LOCK 1
#define UNLOCK 0
//
#define ON 1
#define OFF 0
#define BOOL int
#define FALSE 0
#define TRUE 1
#define INLINE inline
#define LINE_SIZE 1024

#endif // __COMMON_COMMON_H__
