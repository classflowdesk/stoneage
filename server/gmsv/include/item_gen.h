#ifndef __ITEM_GEN_H__
#define __ITEM_GEN_H__

int ITEM_initItemIngCache(void);
int ITEM_initItemAtom(const char *filename);
int ITEM_initRandTable(void);
int ITEM_mergeItem(int char_index, ITEM_Item *items, int num, int money,
                   int petid, int searchtable, int petindex, int alchemist);
int ITEM_canDigest(ITEM_Item *t);
int ITEM_mergeItem_merge(int char_index, int petid, char *data, int petindex,
                         int alchemist);

#ifdef _ITEM_INSLAY
#ifdef _EXPANSION_ITEM_INSLAY
int PETSKILL_ITEM_inslay(int charindex, int inslayindex, int item_index,
                         int num);
#else
int PETSKILL_ITEM_inslay(int charindex, int inslayindex, int item_index);
#endif
#endif

#ifdef _PETSKILL_FIXITEM
int PETSKILL_ITEM_FixItem(int charindex, int fixindex, int *item_index);
#endif

#endif
