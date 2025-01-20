#include "version.h"
#include "linux_platform.h"
#include "proto.h"
#include "config_file.h"
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include "util.h"
#include <dirent.h>

void chompex(char *buf) {
  while (*buf) {
    if (*buf == '\r' || *buf == '\n') {
      *buf = '\0';
    }
    buf++;
  }
}

void dchop(char *src, char *del) {
  int dellen, srclen;
  int i;
  BOOL delete = FALSE;
  srclen = strlen(src);
  dellen = strlen(del);
  if (srclen == 0 || dellen == 0)
    return;
  for (i = 0; i < dellen; i++) {
    if (src[srclen - 1] == del[i]) {
      delete = TRUE;
      break;
    }
  }
  if (delete)
    src[srclen - 1] = '\0';
}

void pohcd(char *src, char *del) {
  int dellen, srclen;
  int i;
  BOOL delete = FALSE;
  srclen = strlen(src);
  dellen = strlen(del);
  if (srclen == 0 || dellen == 0)
    return;
  for (i = 0; i < dellen; i++) {
    if (src[0] == del[i]) {
      delete = TRUE;
      break;
    }
  }

  if (delete)
    for (i = 0; i < srclen; i++)
      src[i] = src[i + 1];
}

/* string match operation: TODO: optimize. */
int charInclude(const char *src, const char *include) {
  int i, j;
  int srclen = strlen(src);
  int includelen = strlen(include);
  for (i = 0; i < srclen; i++) {
    for (j = 0; j < includelen; j++)
      if (src[i] == include[j])
        return i;
  }
  return -1;
}

int strcmptail(const char *s1, const char *s2) {
  int i;
  int len1 = strlen(s1);
  int len2 = strlen(s2);

  for (i = 0;; i++) {
    int ind1 = len1 - 1 - i;
    int ind2 = len2 - 1 - i;
    if (ind1 < 0 || ind2 < 0)
      return 0;
    if (s1[ind1] != s2[ind2])
      return 1;
  }
}

#define DECIMAL_CHAR_NUM 10
#define FIGURE_CHAR_NUM 3
#define SPECIAL_CHAR_NUM 2
char *UTIL_decimalchar[DECIMAL_CHAR_NUM] = {"0", "1", "2", "3", "4",
                                            "5", "6", "7", "8", "9"};
char *UTIL_figurechar[FIGURE_CHAR_NUM] = {"", "", ""};
char *UTIL_specialchar[SPECIAL_CHAR_NUM] = {"", ""};

static int findNumberString(char *in, char *out, int outsiz) {
  int len, i, j, k, findflag, numstrflag;
  char tmp[3];

  len = strlen(in);
  if (len == 0)
    return 0;

  numstrflag = 0;
  findflag = 0;
  for (i = 0, j = 0; in[i] != '\0' && j < outsiz - 2; i++) {
    findflag = 0;
    if (in[i] & 0x80) {
      tmp[0] = in[i];
      tmp[1] = in[i + 1];
      tmp[2] = '\0';
    } else {
      tmp[0] = in[i];
      tmp[1] = '\0';
    }
    for (k = 0; k < DECIMAL_CHAR_NUM; k++) {
      if (strstr(UTIL_decimalchar[k], tmp)) {
        numstrflag = 1;
        findflag = 1;
      }
    }
    for (k = 0; k < FIGURE_CHAR_NUM; k++) {
      if (strstr(UTIL_figurechar[k], tmp)) {
        numstrflag = 1;
        findflag = 1;
      }
    }
    for (k = 0; k < SPECIAL_CHAR_NUM; k++) {
      if (strstr(UTIL_specialchar[k], tmp)) {
        numstrflag = 1;
        findflag = 1;
      }
    }
    if (findflag) {
      if (in[i] & 0x80) {
        out[j] = tmp[0];
        out[j + 1] = tmp[1];
        j += 2;
        i++;
      } else {
        out[j] = tmp[0];
        j += 1;
      }
      continue;
    }

    if (numstrflag) {
      out[j] = '\0';
      return j;
    }
  }

  if (findflag) {
    out[j] = '\0';
    return j;
  } else {
    return 0;
  }
}

// kanji: Chinese char.
int numStrIsKanji(const char *str);
int decimalStrToInt(const char *str);
int kanjiStrToInt(const char *str);

int textToInt(const char *str) {
  char numstr[256];
  if (!findNumberString(str, numstr, sizeof(numstr)))
    return 0;
  if (numStrIsKanji(numstr))
    return kanjiStrToInt(numstr);
  else
    return decimalStrToInt(numstr);
}

int numStrIsKanji(const char *str) {
  int i, j;
  char tmp[3];

  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] & 0x80) {
      tmp[0] = str[i];
      tmp[1] = str[i + 1];
      tmp[2] = '\0';
      i++;
    } else {
      tmp[0] = str[i];
      tmp[1] = '\0';
    }
    for (j = 0; j < FIGURE_CHAR_NUM; j++) {
      if (strstr(UTIL_figurechar[j], tmp)) {
        return 1;
      }
    }
    for (j = 0; j < SPECIAL_CHAR_NUM; j++) {
      if (strstr(UTIL_specialchar[j], tmp)) {
        return 1;
      }
    }
  }
  return 0;
}

int decimalStrToInt(const char *str) {

  double val;
  char tmp[3];
  int i, j;

  val = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] & 0x80) {
      tmp[0] = str[i];
      tmp[1] = str[i + 1];
      tmp[2] = '\0';
      i++;
    } else {
      tmp[0] = str[i];
      tmp[1] = '\0';
    }
    for (j = 0; j < DECIMAL_CHAR_NUM; j++) {
      if (strstr(UTIL_decimalchar[j], tmp)) {
        val = val * 10 + j;
      }
    }
  }

  if (val > 0x7fffffff)
    return 0x7fffffff;
  else
    return (int)val;
}

int kanjiStrToInt(const char *str) {
  double val, tmpval;
  char tmp[3];
  int num, i, j, numflag;

  numflag = 0;
  num = 1;
  tmpval = 0;
  val = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] & 0x80) {
      tmp[0] = str[i];
      tmp[1] = str[i + 1];
      tmp[2] = '\0';
      i++;
    } else {
      tmp[0] = str[i];
      tmp[1] = '\0';
    }
    for (j = 0; j < SPECIAL_CHAR_NUM; j++) {
      if (strstr(UTIL_specialchar[j], tmp)) {
        if (numflag == 1)
          tmpval += num;
        if (j == 0)
          val += tmpval * 10000;
        else if (j == 1)
          val += tmpval * 100000000;
        num = 1;
        numflag = 0;
        tmpval = 0;
        goto nextchar;
      }
    }
    for (j = 0; j < FIGURE_CHAR_NUM; j++) {
      if (strstr(UTIL_figurechar[j], tmp)) {
        if (j == 0)
          tmpval += num * 10;
        else if (j == 1)
          tmpval += num * 100;
        else if (j == 2)
          tmpval += num * 1000;
        num = 1;
        numflag = 0;
        goto nextchar;
      }
    }
    for (j = 0; j < DECIMAL_CHAR_NUM; j++) {
      if (strstr(UTIL_decimalchar[j], tmp)) {
        num = j;
        numflag = 1;
        goto nextchar;
      }
    }
  nextchar:
    continue;
  }
  if (numflag == 1)
    tmpval += num;
  val += tmpval;

  if (val > 0x7fffffff)
    return 0x7fffffff;
  else
    return (int)val;
}

BOOL strtolchecknum(char *arg, void *number, int base, CTYPE type) {
  char *buf;
  int num;
  num = strtol(arg, &buf, base);
  switch (type) {
  case CHAR:
    *(char *)number = (char)num;
    break;
  case SHORT:
    *(short *)number = (short)num;
    break;
  case INT:
    *(int *)number = num;
    break;
  case DOUBLE:
    *(double *)number = (double)num;
    break;
  default:
    break;
  }

  if (strlen(buf) >= 1)
    return FALSE;
  return TRUE;
}

void deleteCharFromString(char *src, const char *dels) {
  int index = 0;
  int i = 0, j;
  int delength = strlen(dels);
  while (src[i] != '\0') {
    if (src[i] == BACKSLASH)
      src[index++] = src[++i]; /* substitute next char */
    else {
      for (j = 0; j < delength; j++) {
        if (src[i] == dels[j])
          goto incrementi;
      }
      src[index++] = src[i];
    }
  incrementi:
    i++;
  }
  src[index] = '\0';
}

void deleteCharFromStringNoEscape(char *src, const char *dels) {
  int index = 0;
  int i = 0, j;
  int delength = strlen(dels);
  while (src[i] != '\0') {
    for (j = 0; j < delength; j++) {
      if (src[i] == dels[j])
        goto incrementi;
    }
    src[index++] = src[i];
  incrementi:
    i++;
  }
  src[index] = '\0';
}

char *replaceString(char *src, char oldc, char newc) {
  char *cp = src;
  do {
    if (*cp == oldc)
      *cp = newc;
  } while (*cp++);
  return src;
}

typedef struct tagEscapeChar {
  char escapechar;
  char escapedchar;
} EscapeChar;
static EscapeChar escapeChar[] = {
    {'\n', 'n'},
    {',', 'c'},
    {'|', 'z'},
    {'\\', 'y'},
};

char makeCharFromEscaped(char c) // add this function,because the second had it
{
  int i;

  for (i = 0; i < sizeof(escapeChar) / sizeof(escapeChar[0]); i++) {
    if (escapeChar[i].escapedchar == c) {
      c = escapeChar[i].escapechar;
      break;
    }
  }
  return c;
}

/*----------------------------------------
 * delim: 查找
 * index: 查找第index-1到第index个delim间的字符数据.
 * Param:
 *  src (const char*):   源字符串.
 *  delim (const char*): 分隔符.
 *  index (const int): 第几个分隔符.
 *  buf (char *):
 *  buflen (const int):
 * Return Value:
 *  BUF内存在有效数据 TRUE(1);
 *  BUF内不存在有效数据 FALSE(0);
 *
 *  getStringFromIndexWithDelim("aho=hoge","=",1,buf,sizeof(buf));
 *  buf ... aho
 *  getStringFromIndexWithDelim("aho=hoge","=",2,buf,sizeof(buf));
 *  buf ... hoge
 *  getStringFromIndexWithDelim("aho=hoge","=",3,buf,sizeof(buf));
 *  return FALSE;
 ----------------------------------------*/
BOOL getStringFromIndexWithDelim_body(
    const char *src, const char *delim, const int index,
    char *buf, const int buflen,
    const char *file, const int line) {
  int i, length = 0, addlen = 0;
  const int delim_len = strlen(delim);
  for (i = 0; i < index; i++) {
    char *last;
    src += addlen;
    if (delim_len == 0) {
      last = strstr_onebyte(src, delim[0]);
    } else {
      last = strstr(src, delim);
    }
    if (last == NULL) {
      util_strncpysafe2(buf, buflen, src);
      return i == index - 1 ? TRUE : FALSE;
    }
    length = last - src;
    addlen = length + delim_len;
  }
  util_strncpysafe1(buf, buflen, src, length);
  return TRUE;
}

/*------------------------------------------------------------
 * "a, b, c, d"
 *  src  char* input_string.
 *  int1 int* int()
 *  int2 int* int()
 *  int3 int* int()
 *  int4 int* int()
 ------------------------------------------------------------*/
void getFourIntsFromString(char *src, int *int1, int *int2, int *int3,
                           int *int4) {
  int ret;
  char string[128];

  ret = getStringFromIndexWithDelim(src, ",", 1, string, sizeof(string));
  if (ret == FALSE)
    *int1 = 0;
  else
    *int1 = atoi(string);

  ret = getStringFromIndexWithDelim(src, ",", 2, string, sizeof(string));
  if (ret == FALSE)
    *int2 = 0;
  else
    *int2 = atoi(string);

  ret = getStringFromIndexWithDelim(src, ",", 3, string, sizeof(string));
  if (ret == FALSE)
    *int3 = 0;
  else
    *int3 = atoi(string);

  ret = getStringFromIndexWithDelim(src, ",", 4, string, sizeof(string));
  if (ret == FALSE)
    *int4 = 0;
  else
    *int4 = atoi(string);
}

void deleteSequentChar(char *src, char *dels) {
  int length;
  int delength;
  int i, j;
  int index = 0;
  char backchar = '\0';
  length = strlen(src);
  delength = strlen(dels);

  for (i = 0; i < length; i++) {
    if (src[i] == BACKSLASH) {
      src[index++] = src[i++];
      if (i >= length) {
        print("*\n");
        break;
      }
      src[index++] = src[i];
      backchar = '\0';
      continue;
    }
    if (src[i] == backchar)
      continue;

    backchar = '\0';
    for (j = 0; j < delength; j++) {
      if (src[i] == dels[j]) {
        backchar = src[i];
        break;
      }
    }
    src[index++] = src[i];
  }
  src[index++] = '\0';
}

/*----------------------------------------
 * (TCP)
 * Param:
 *     port (unsigned short): port to listen external connection.
 * Return Value:
 * 1. socket:
 * 2. bind:
 * 3. listen:
 ----------------------------------------*/
int bindLocalhost(unsigned short port) {
  struct sockaddr_in _socket;
  int _s_fd;

  _s_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (_s_fd == -1) {
    print("%s\n", strerror(errno));
    return -1;
  }
  if (getReuseaddr()) {
    int send_buff;
    setsockopt(_s_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&send_buff,
               sizeof(send_buff));
  }

  // normal style to initialize socket.
  memset(&_socket, 0, sizeof(_socket));
  _socket.sin_family = AF_INET;
  _socket.sin_port = htons(port);
  _socket.sin_addr.s_addr = INADDR_ANY;

  if (-1 ==
      bind(_s_fd, (struct sockaddr *)&_socket, sizeof(struct sockaddr_in))) {
    print("%s\n", strerror(errno));
    return -1;
  }

  if (-1 == listen(_s_fd, 5)) {
    print("%s\n", strerror(errno));
    return -1;
  }
  return _s_fd;
}

int connectHost(char *hostname, unsigned short port) {
  struct sockaddr_in _socket;
  struct hostent *hoste;
  int fd;

  memset(&_socket, 0, sizeof(_socket));
  _socket.sin_family = AF_INET;
  _socket.sin_port = htons(port);
  _socket.sin_addr.s_addr = inet_addr(hostname);
  if (_socket.sin_addr.s_addr == -1) {

    hoste = gethostbyname(hostname);
    if (hoste == NULL) {
      print("Cannot get host info from host name: %s\n.", hostname);
      return -1;
    }
    memcpy((void *)&_socket.sin_addr.s_addr, hoste->h_addr,
           sizeof(struct in_addr));
  }

  fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd == -1) {
    print("Cannot Create Socket(%s errno:%d)\n", strerror(errno), errno);
    return -1;
  }

  if (0 !=
      connect(fd, (struct sockaddr *)&_socket, sizeof(struct sockaddr_in))) {
    print("Cannot connect. (%s errno:%d)\n", strerror(errno), errno);
    return -1;
  }
  return fd;
}

char *nindex(char *string, int c, int number) {
  int i;
  int num = 0;
  int length = strlen(string);
  if (number <= 0)
    return string;
  for (i = 0; i < length; i++) {
    if (string[i] == c)
      num++;
    if (number == num)
      return &string[i];
  }
  return NULL;
}

BOOL rrd(const char *dirname, const STRING64 *buf, const int bufsize,
         int *index) {
  char dirn[1024];
  struct dirent64 *p_dirent;
  struct stat st;
  BOOL ret_val = TRUE;
  DIR *p_dir = opendir(dirname);
  if (p_dir == NULL)
    return FALSE;
  while (TRUE) {
    p_dirent = readdir64(p_dir);
    if (p_dirent == NULL) {
      if (errno == EBADF) {
        print("%d\n", errno);
        ret_val = FALSE;
      }
      break;
    }
    print("%s\n", p_dirent->d_name);
    if (p_dirent->d_name[0] == '.')
      continue;
    snprintf(dirn, sizeof(dirn), "%s/%s", dirname, p_dirent->d_name);
    if (stat(dirn, &st) == -1)
      continue;
    if (S_ISDIR(st.st_mode)) {
      if (rrd(dirn, buf, bufsize, index) == FALSE) {
        ret_val = FALSE;
        break;
      }
    } else {
      if (*index >= bufsize)
        break;
      util_strncpysafe2(buf[*index].string, sizeof(buf[*index].string), dirn);
      (*index)++;
    }
  }
  closedir(p_dir);
  return TRUE;
}

int rgetFileName(const char *dirname, const STRING64 *string, const int size) {
  int index = 0;
  if (rrd(dirname, string, size, &index) == FALSE)
    return -1;
  else
    return index;
}

BOOL checkStringsUnique(char **strings, int num, int verbose) {
  int i, j;
  for (i = 0; i < num - 1; i++) {
    for (j = i + 1; j < num; j++) {
      if (!strcmp(strings[i], strings[j])) {
        if (verbose)
          print("Overlapp string is %s\n", strings[i]);
        return FALSE;
      }
    }
  }
  return TRUE;
}

BOOL PointInRect(RECT *rect, POINT *p) {
  if (rect->x <= p->x && p->x <= rect->x + rect->width && rect->y <= p->y &&
      p->y <= rect->y + rect->height)
    return TRUE;
  return FALSE;
}

BOOL CoordinateInRect(RECT *rect, int x, int y) {
  POINT p = {x, y};
  return PointInRect(rect, &p);
}

int clipRect(RECT *rect1, RECT *rect2, RECT *ret) {
  if (rect1->x > rect2->x + rect2->width - 1 ||
      rect2->x > rect1->x + rect1->width - 1 ||
      rect1->y > rect2->y + rect2->height - 1 ||
      rect2->y > rect1->y + rect1->height - 1)
    return 0;
  ret->x = max(rect1->x, rect2->x);
  ret->y = max(rect1->y, rect2->y);
  ret->width = min(rect1->x + rect1->width, rect2->x + rect2->width) - ret->x;
  ret->height =
      min(rect1->y + rect1->height, rect2->y + rect2->height) - ret->y;
  return 1;
}

BOOL isstring1or0(char *string) {
  if (strcasecmp(string, "TRUE") == 0)
    return TRUE;
  if (strcasecmp(string, "FALSE") == 0)
    return FALSE;
  if (strcasecmp(string, "1") == 0)
    return TRUE;
  if (strcasecmp(string, "0") == 0)
    return FALSE;
  if (strcasecmp(string, "ON") == 0)
    return TRUE;
  if (strcasecmp(string, "OFF") == 0)
    return FALSE;
  return FALSE;
}

float linearDiv(float val1, float val2, float d) {
  return val1 + (val2 - val1) * (d);
}

void makeRandomString(char *cand, char *out, int len) {
  int i;
  int l = strlen(cand);
  for (i = 0; i < len; i++) {
    out[i] = cand[rand() % l];
  }
  out[i] = '\0';
}

BOOL isExistFile(char *filename) {
  FILE *fp;
  fp = fopen(filename, "w");
  if (fp) {
    fclose(fp);
    return TRUE;
  } else
    return FALSE;
}

// 查看数组中是否有重复数字.
BOOL checkRedundancy(const int *array, const int array_size) {
  int i, j;
  int ret = FALSE;
  for (i = 0; i < array_size && !ret; i++) {
    if (*(array + i) != -1) {
      for (j = i + 1; j < array_size; j++) {
        if (*(array + i) == *(array + j)) {
          ret = TRUE;
          break;
        }
      }
    }
  }
  return ret;
}

static unsigned char BitTable[] = {
    0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0,
    0x30, 0xB0, 0x70, 0xF0, 0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8,
    0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8, 0x04, 0x84, 0x44, 0xC4,
    0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
    0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC,
    0x3C, 0xBC, 0x7C, 0xFC, 0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2,
    0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2, 0x0A, 0x8A, 0x4A, 0xCA,
    0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
    0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6,
    0x36, 0xB6, 0x76, 0xF6, 0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE,
    0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE, 0x01, 0x81, 0x41, 0xC1,
    0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
    0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9,
    0x39, 0xB9, 0x79, 0xF9, 0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5,
    0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5, 0x0D, 0x8D, 0x4D, 0xCD,
    0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
    0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3,
    0x33, 0xB3, 0x73, 0xF3, 0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB,
    0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB, 0x07, 0x87, 0x47, 0xC7,
    0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
    0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF,
    0x3F, 0xBF, 0x7F, 0xFF};
static unsigned short crctab16[] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7, 0x8108,
    0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef, 0x1231, 0x0210,
    0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6, 0x9339, 0x8318, 0xb37b,
    0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de, 0x2462, 0x3443, 0x0420, 0x1401,
    0x64e6, 0x74c7, 0x44a4, 0x5485, 0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee,
    0xf5cf, 0xc5ac, 0xd58d, 0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,
    0x5695, 0x46b4, 0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d,
    0xc7bc, 0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b, 0x5af5,
    0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12, 0xdbfd, 0xcbdc,
    0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a, 0x6ca6, 0x7c87, 0x4ce4,
    0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41, 0xedae, 0xfd8f, 0xcdec, 0xddcd,
    0xad2a, 0xbd0b, 0x8d68, 0x9d49, 0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13,
    0x2e32, 0x1e51, 0x0e70, 0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,
    0x9f59, 0x8f78, 0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e,
    0xe16f, 0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e, 0x02b1,
    0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256, 0xb5ea, 0xa5cb,
    0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d, 0x34e2, 0x24c3, 0x14a0,
    0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xa7db, 0xb7fa, 0x8799, 0x97b8,
    0xe75f, 0xf77e, 0xc71d, 0xd73c, 0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657,
    0x7676, 0x4615, 0x5634, 0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,
    0xb98a, 0xa9ab, 0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882,
    0x28a3, 0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92, 0xfd2e,
    0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9, 0x7c26, 0x6c07,
    0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1, 0xef1f, 0xff3e, 0xcf5d,
    0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8, 0x6e17, 0x7e36, 0x4e55, 0x5e74,
    0x2e93, 0x3eb2, 0x0ed1, 0x1ef0,
};
unsigned short CheckCRC(unsigned char *p, int size) {
  unsigned short crc = 0;
  int i;

  for (i = 0; i < size; i++) {
    crc = (crctab16[(crc >> 8) & 0xFF] ^ (crc << 8) ^ BitTable[p[i]]);
  }
  return crc;
}

// Add Code By Shan  2001.06.16
INLINE double time_diff(struct timeval subtrahend, struct timeval subtractor) {
  return ((subtrahend.tv_sec - subtractor.tv_sec) +
          (subtrahend.tv_usec - subtractor.tv_usec) / (double)1E6);
}
char *strcpy_s(char *des, size_t n, const char *src) {
  assert(des != NULL && src != NULL);

  if (des == NULL || src == NULL || n == 0) {
    return NULL;
  }

  char *TM_RetChar = des;
  int i = 0;

  while ((*des++ = *src++)) {
    i++;
    if (i >= n) {
      i--;
      break;
    }
  }
  TM_RetChar[i] = '\0';
  return TM_RetChar;
}
char *itoa(long n, int base) {
  register char *p;
  register int minus;
  static char buf[36];
  p = &buf[36];
  *--p = '\0';
  if (n < 0) {
    minus = 1;
    n = -n;
  } else
    minus = 0;
  if (n == 0)
    *--p = '0';
  else
    while (n > 0) {
      *--p = "0123456789abcdef"[n % base];
      n /= base;
    }
  if (minus)
    *--p = '-';
  return p;
}
