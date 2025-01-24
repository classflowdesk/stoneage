#ifndef _BATTLE_ITEM_H_
#define _BATTLE_ITEM_H_
//--------------------------------------------------------------
//  荚汊失奶  丞毛银匀凶桦宁及质  
//--------------------------------------------------------------
// 爵    及桦宁
void ITEM_useRecovery_Battle( int char_index, int toindex, int item_index );
void ITEM_useStatusChange_Battle( int char_index, int toindex, int item_index );
void ITEM_useMagicDef_Battle( int char_index, int toindex, int item_index );
void ITEM_useParamChange_Battle( int char_index, int toindex, int item_index );
void ITEM_useFieldChange_Battle( int char_index, int toindex, int item_index );
void ITEM_useAttReverse_Battle( int char_index, int toindex, int item_index );
void ITEM_useStatusRecovery_Battle( int char_index, int toindex, int item_index );
void ITEM_useCaptureUp_Battle( int char_index, int toindex, int item_index );
#ifdef _ITEM_CRACKER
void ITEM_useCracker_Effect( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_ADDEXP	//vincent 经验提升
void ITEM_useAddexp_Effect( int char_index, int toindex, int haveitem_index);
#endif
#ifdef _ITEM_REFRESH //vincent 解除异常状态道具
void ITEM_useRefresh_Effect( int char_index, int toindex, int haveitem_index);
#endif
//Terry add 2001/12/24
#ifdef _ITEM_FIRECRACKER
void ITEM_useFirecracker_Battle(int char_index,int toindex,int item_index);
#endif
//Terry end

#ifdef _ITEM_MAGICRECOVERY
void ITEM_useMRecovery_Battle( int char_index, int toNo, int haveitem_index );
#endif
#ifdef _ITEM_USEMAGIC
void ITEM_useMagic_Battle( int char_index, int toNo, int haveitem_index );
#endif
#endif
