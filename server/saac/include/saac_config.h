#ifndef __SAAC_CONFIG_H__
#define __SAAC_CONFIG_H__

//
#include "version.h"
//
#include "common.h"

typedef struct tagSaacConfig {
  int port;
  char logdir[128];
  char chardir[128];
  char svpass[128];
  char dbdir[128];
  int log_rotate_interval;
  char maildir[128];
  char familydir[128];
  char fmpointdir[128];
  char fmsmemodir[128];
  int Total_Charlist;
  int Expired_mail;
  int Del_Family_or_Member;
  int Write_Family;
  int sameipmun;
#ifdef _AUTO_BACKUP
  int autobackupday;
  int autobackuphour;
#endif
#ifdef _LOTTERY_SYSTEM
  int lotterysystem;
#endif
  int savezip;
  int servid;
#ifdef _SLEEP_CHAR
  char sleepchardir[128];
#endif
} SaacConfig;

#ifdef __SAAC_CONFIG_C__
SaacConfig g_saac_config;
const char *DEFAULT_CONFIG_FILE = "acserv.cf";
#else
extern SaacConfig g_saac_config;
extern const char *DEFAULT_CONFIG_FILE;
#endif

int saac_read_config(const char *path);

#endif // __SAAC_CONFIG_H__