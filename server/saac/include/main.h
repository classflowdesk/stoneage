#ifndef __MAIN_H__
#define __MAIN_H__

#include "saac_common.h"

#define SA_NODEFER 0x40000000
#define SA_NOMASK SA_NODEFER
#define CHARDATASIZE (1024 * 1024)

char *chartime();

typedef struct tagGmsv {
  int use;
  int fd;
  char name[128];
} gmsv;

int get_rotate_count(void);
void checkGSUCheck(char *id);
int logout_game_server(int ti);
int is_game_server_login(int ti);

void gmsvBroadcast(int fd, char *p1, char *p2, char *p3, int flag);

#if _ATTESTAION_ID == 1
int login_game_server(const int ti, const int id, const char *svname,
                      const char *svpas, char *result, const int resultlen,
                      char *retdata, int retdatalen);
#else
int login_game_server(const int ti, const char *svname, const char *svpas,
                      char *result, const int resultlen, char *retdata,
                      const int retdatalen);
#endif

#ifdef _ANGEL_SUMMON
void delMissionTableOnedata(int index);

typedef enum {
  MISSION_NONE = 0,
  MISSION_WAIT_ANSWER,
  MISSION_DOING,
  MISSION_HERO_COMPLETE,
  MISSION_TIMEOVER,
} ANGEL_MISSIONFLAG;
#endif

typedef struct tagMemBuffer {
  int use;
  char buf[512];
  int len;
  int next;
} MemBuffer;
typedef struct tagConnection {
  int use;
  int fd;
  int mbtop_ri;
  int mbtop_wi;
  struct sockaddr_in remoteaddr; // 远端地址，即客户端的socket地址
  int closed_by_remote;
} Connection;
#ifdef __MAIN_C__
#define EXT
#else
#define EXT extern
#endif
EXT MemBuffer *g_mem_buffer;
EXT int g_mem_buffer_size;
EXT int g_mem_buffer_used;
EXT int g_mem_buffer_finder;
EXT char g_temp_buffer[1 << 20];
EXT Connection *g_con; // SAAC-Client连接
EXT int g_main_sock_fd; // 主sock文件描述符
EXT struct sockaddr_in g_local_addr; // 本地SAAC服务器的TCP地址
EXT struct timeval select_timeout; // 选择超时时间
EXT time_t sys_time; // Robin add
EXT gmsv gs[MAXCONNECTION]; // SAAC-GMSV连接
#undef EXT

int findregBlankMemBuf(void);
int unregMemBuf(int index);
int findregBlankCon(void);
int getFreeMem(void);
int appendReadBuffer(int index, char *data, int len);
int appendWriteBuffer(int index, char *data, int len);
int appendMemBufList(int top, char *data, int len);
int consumeMemBufList(int top, char *out, int len, int flag, int copyflag);
int getLineReadBuffer(int index, char *buf, int len);
#endif
