#ifndef __MAIL_H__
#define __MAIL_H__

#include "saac_common.h"

#ifdef __MAIL_C__
#define EXT
#else
#define EXT extern
#endif

#define TEXT_MAX 1024

typedef enum enumMailState {
  MS_NOUSE = 0,
  MS_NEWMESSAGE,
  MS_WAIT_ACK,
} MailState;

typedef struct tagMail {
  int use;
  unsigned int id_char_name_hash;
  char id_to[USERID_MAX];
  char char_name_to[CHARNAME_MAX];
  char id_from[USERID_MAX];
  char char_name_from[CHARNAME_MAX];
  char text[TEXT_MAX];
  int option;
  unsigned int message_id;
  MailState state;
  time_t recv_time;
} Mail;

// mail_buf
EXT Mail *g_mail_buf;
// mail_buf的数量
EXT int g_mail_buf_size;
// 可用的mail_buf的索引
EXT int g_mail_buf_finder;

void Mail_receive(const char *id_from, const char *char_name_from,
                 const char *id_to, const char *char_name_to,
                 const char *message, const int option,
                 const unsigned use_msgid, const unsigned msg_id);
void Mail_receive_ack(const char *id, const char *char_name, const int a,
                      const unsigned mesgid);
void Mail_flush(const int fd, const char *id, const char *char_name);
void Mail_expire();
int Mail_read(const char *dir);

#endif // __MAIL_H__
