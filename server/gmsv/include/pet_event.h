#ifndef __PET_EVENT_H__
#define __PET_EVENT_H__

/*
 * 
 * 2025.05.28 revise
 */

/**/
void PET_Talkfunc(int meindex, int talkerindex, char *msg, int color);

/**/
void PET_Watchfunc(int objmeindex, int objmoveindex, CHAR_ACTION act, int x,
                   int y, int dir, int *opt, int optlen);
/*清除*/
int PET_CleanPetdeletetime(int objmeindex);
/*清除所有Free状态的的Pet*/
void PET_CleanFreePetAll();
/*确定一只Pet是否是FreePet*/
void PET_CHECKFreePetIsIt(int petindex);

#endif
