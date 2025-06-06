
BOOL strtolchecknum(char *arg, void *number, int base, CTYPE type);
int strcmptail(const char *s1, const char *s2);
int textToInt(const char *str);
void deleteCharFromString(char *src, const char *dels);
void deleteCharFromStringNoEscape(char *src, const char *dels);
#define deleteWhiteSpace(src) deleteCharFromStringNoEscape(src, " \t")
char *replaceString(char *src, char oldc, char newc);
char *escapeStrStr(char *src, char *needle);
void getFourIntsFromString(char *src, int *int1, int *int2, int *int3, int *int4);
void deleteSequentChar(char *src, char *dels);
int hashpjw(const char *s);

int bindLocalhost(unsigned short port);
int connectHost(char *hostname, unsigned short port);
int charInclude(const char *src, const char *include);
char *nindex(char *string, int c, int number);
void makeRandomString(char *cand, char *out, int len);
BOOL checkStringsUnique(char **strings, int num, int verbose);

BOOL PointInRect(RECT *rect, POINT *p);
BOOL CoordinateInRect(RECT *rect, int x, int y);
int clipRect(RECT *rect1, RECT *rect2, RECT *ret);
BOOL isstring1or0(char *string);

float linearDiv(float val1, float val2, float d);
BOOL checkRedundancy(const int *array, const int array_size);
unsigned short CheckCRC(unsigned char *p, int size);
// Shan Add
char *strcpy_s(char *des, size_t n, const char *src);

#define ABS(x) ((x) >= 0 ? (x) : -(x))

#define VALIDATEDIR(x)                                                         \
  ({                                                                           \
    (x) %= 8;                                                                  \
    (x) = ((x) + 8) % 8;                                                       \
  })

#endif
