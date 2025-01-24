#include "version.h"
//
#include "common.h"
//
#include "char.h"
#include "char_base.h"
#include "magic.h"
#include "magic_base.h"
#include "magic_field.h"

int MAGIC_Recovery_Field(int char_index, int magicindex) {
  char *magicarg;
  float power;
  int workhp;
  int prevhp;
  char msgbuf[64];

#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(char_index, "HP") == FALSE)
    return TRUE;
#endif
  magicarg = MAGIC_getChar(magicindex, MAGIC_OPTION);
  power = atoi(magicarg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(char_index);
#endif
  prevhp = CHAR_getInt(char_index, CHAR_HP);
  workhp = prevhp + (int)power;
  CHAR_setInt(char_index, CHAR_HP,
              min(workhp, CHAR_getWorkInt(char_index, CHAR_WORKMAXHP)));
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (CHAR_getWorkInt(char_index, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE) {
    int mypartyarray = -1;
    int oyaindex = CHAR_getWorkInt(char_index, CHAR_WORKPARTYINDEX1);
    if (CHAR_CHECKINDEX(oyaindex)) {
      int i;
      for (i = 0; i < getPartyNum(oyaindex); i++) {
        int workindex = CHAR_getWorkInt(oyaindex, CHAR_WORKPARTYINDEX1 + i);
        if (CHAR_CHECKINDEX(workindex)) {
          if (workindex == char_index) {
            mypartyarray = i;
            break;
          }
        }
      }
      if (mypartyarray != -1) {
        for (i = 0; i < getPartyNum(char_index); i++) {
          int index = CHAR_getPartyIndex(char_index, i);
          if (CHAR_CHECKINDEX(index)) {
            if (index != char_index) {
              CHAR_send_N_StatusString(index, mypartyarray,
                                       CHAR_N_STRING_HP | CHAR_N_STRING_MP);
            }
          }
        }
      }
    }
  }
  snprintf(msgbuf, sizeof(msgbuf), "�;����ظ�%d",
           min(workhp, CHAR_getWorkInt(char_index, CHAR_WORKMAXHP)) - prevhp);
  CHAR_talkToCli(char_index, -1, msgbuf, CHAR_COLORWHITE);
  return TRUE;
}

int MAGIC_OtherRecovery_Field(int char_index, int toindex, int magicindex) {
  char *magicarg;
  float power;
  int workhp;
  int prevhp;
  char msgbuf[64];

#ifdef _TYPE_TOXICATION
  if (CHAR_CanCureFlg(char_index, "HP") == FALSE)
    return FALSE;
#endif
  magicarg = MAGIC_getChar(magicindex, MAGIC_OPTION);
  power = atoi(magicarg);
  power = RAND((power * 0.9), (power * 1.1));
#ifndef _MAGIC_REHPAI
  power *= GetRecoveryRate(toindex);
#endif
  prevhp = CHAR_getInt(toindex, CHAR_HP);
  workhp = prevhp + (int)power;
  CHAR_setInt(toindex, CHAR_HP,
              min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)));
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
    if (char_index != toindex) {
      CHAR_send_P_StatusString(toindex, CHAR_P_STRING_HP);
    }
  }
  CHAR_send_P_StatusString(char_index, CHAR_P_STRING_HP | CHAR_P_STRING_MP);
  if (char_index != toindex) {
    CHAR_PartyUpdate(char_index, CHAR_N_STRING_MP);
    CHAR_PartyUpdate(toindex, CHAR_N_STRING_HP);
  } else {
    CHAR_PartyUpdate(char_index, CHAR_N_STRING_MP | CHAR_N_STRING_HP);
  }
  if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPET) {
    int i;
    for (i = 0; i < CHAR_MAXPETHAVE; i++) {
      int workindex = CHAR_getCharPet(char_index, i);
      if (workindex == toindex) {
        CHAR_send_K_StatusString(char_index, i, CHAR_K_STRING_HP);
      }
    }
  }

  if (char_index != toindex) {
    snprintf(msgbuf, sizeof(msgbuf), "%s���;����ظ�%d", CHAR_getUseName(toindex),
             min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) - prevhp);
    CHAR_talkToCli(char_index, -1, msgbuf, CHAR_COLORWHITE);

    if (CHAR_getInt(toindex, CHAR_WHICHTYPE) == CHAR_TYPEPLAYER) {
      snprintf(msgbuf, sizeof(msgbuf), "����%s�;����ظ�%d",
               CHAR_getUseName(char_index),
               min(workhp, CHAR_getWorkInt(toindex, CHAR_WORKMAXHP)) - prevhp);
      CHAR_talkToCli(toindex, -1, msgbuf, CHAR_COLORWHITE);
    }
  } else {
    snprintf(msgbuf, sizeof(msgbuf), "�;����ظ�%d",
             min(workhp, CHAR_getWorkInt(char_index, CHAR_WORKMAXHP)) - prevhp);
    CHAR_talkToCli(char_index, -1, msgbuf, CHAR_COLORWHITE);
  }

  return TRUE;
}
