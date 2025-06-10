#ifndef __MAIL_H__
#define __MAIL_H__

void Mail_receive(const char *id_from, const char *char_name_from,
                 const char *id_to, const char *char_name_to,
                 const char *message, const int option,
                 const unsigned use_msgid, const unsigned msg_id);
void Mail_receive_ack(const char *id, const char *char_name, const int a,
                      const unsigned mesgid);
void Mail_flush(const int fd, const char *id, const char *char_name);
void Mail_expire();
int Mail_read(const char *dir);

#endif
