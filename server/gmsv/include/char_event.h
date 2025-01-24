#ifndef __CHAR_EVENT_H__
#define __CHAR_EVENT_H__

#include "char_base.h"

void CHAR_allpostwalk( int index );
BOOL CHAR_allprewalk( int index,int* dir,int* mode);

void CHAR_recoveryStatus( int char_index );
void CHAR_loopFunc( int index );
void CHAR_playerWatchfunc( int meindex, int moveindex, CHAR_ACTION act,
                           int x, int y, int dir, int* opt, int optlen );

void CHAR_sendWallDamage( int char_index,int x, int y, int damage );
void CHAR_playerresurrect( int char_index, int hp );

int CHAR_die( int char_index );
void CHAR_playerTalkedfunc( int char_index, int talkindex,char* message,
                            int color, int channel );

void CHAR_recoveryStatus( int char_index );
BOOL CHAR_makeCADefaultString( int objindex,char* buf,int buflen, int act );
BOOL CHAR_makeCAOPT1String( int objindex,char* buf,
                                        int buflen, int act,int opt1 );
BOOL CHAR_makeCAOPT3String( int objindex,char* buf,
                            int buflen, int act,int opt1,int opt2,int opt3 );
#ifdef _STREET_VENDOR
BOOL CHAR_makeCAOPTString(int objindex,char* buf,int buflen,int act,char *string);
#endif

#endif 
