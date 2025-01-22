#ifndef __COMMON_H__
#define __COMMON_H__

#define mj_sprintf(format,arg...) ((sizeof(format)==4)?sprintf(format,##arg):snprintf(format,sizeof(format),##arg))

#define min(x, y) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __x : __y; })
#define max(x, y) ({typeof(x) __x=(x),__y=(y);(__x < __y) ? __y : __x; })
#define swap(x, y) ({typeof(x) __x=(y);(y)=(x);(x)=__x;})

#define STRINGBUFSIZ 16384 // 4096*4

#ifdef __GNUC__
    __attribute__ ((format(printf, 3, 4)));
#else
    ;
#endif

#endif
