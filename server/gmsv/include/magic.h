#ifndef __MAGIC_H__
#define __MAGIC_H__

/* ÈÈÖî */
int MAGIC_Use( int char_index, int haveitem_index, int toindex);
#ifdef _MAGIC_FEATHERS
int MAGIC_Feathers( int char_index, int toindex, int marray, int mp );
#endif

int MAGIC_Recovery( int char_index, int toindex,int marray, int mp );
int MAGIC_OtherRecovery( int char_index, int toindex, int marray, int mp );
int	MAGIC_FieldAttChange( int char_index, int toindex, int marray, int mp );
int	MAGIC_StatusChange( int char_index, int toindex, int marray, int mp );
#ifdef _MAGIC_DEEPPOISON
int	MAGIC_StatusChange2( int char_index, int toindex, int marray, int mp );
#endif
int	MAGIC_StatusRecovery( int char_index, int toindex, int marray, int mp );
int	MAGIC_MagicDef( int char_index, int toindex, int marray, int mp );
int	MAGIC_Ressurect( int char_index, int toindex, int marray, int mp );
int	MAGIC_AttReverse( int char_index, int toindex, int marray, int mp );
int	MAGIC_ResAndDef( int char_index, int toindex, int marray, int mp );
#ifdef _OTHER_MAGICSTAUTS
int	MAGIC_MagicStatusChange( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _ATTACK_MAGIC
int     MAGIC_AttMagic( int char_index , int toindex , int marray , int mp );
#endif
#ifdef _ITEM_METAMO
int MAGIC_Metamo( int char_index, int toindex,int marray, int mp );
#endif

#ifdef _ITEM_ATTSKILLMAGIC
int MAGIC_AttSkill( int char_index, int toindex,int marray, int mp );
#endif
#ifdef _MAGIC_WEAKEN// vincent  ¾«Áé:ĞéÈõ
int	MAGIC_Weaken( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_BARRIER// vincent  ¾«Áé:Ä§ÕÏ
int	MAGIC_Barrier( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_NOCAST// vincent  ¾«Áé:³ÁÄ¬
int	MAGIC_Nocast( int char_index, int toindex, int marray, int mp );
#endif
#ifdef _MAGIC_TOCALL
int MAGIC_ToCallDragon( int char_index, int toindex,int marray, int mp );
#endif

//----------------------------------------------------------------------
// Ê§ÄÌ  Ø©¼°ŞË  ¾®ÈÕÈÈÖî  Ä¯Ã«ß¯ÔÊ
//
int MAGIC_GetArrayNo(
	int char_index, 	// Æ½ÅÒ·ÂÛÍÕıÄÌ¼ş·¸ÓÀÛÍµ©
	int haveitem_index   //    ÔÈ»¯ÔÂÊ§ÄÌ  Ø©¼°ŞË  
);
//
//----------------------------------------------------------------------
//-------------------------------------------------------------------
//
//  ÈÈÖîÃ«  ÌÎÒøµ¤
//
int MAGIC_DirectUse(
	int char_index, // Òøµ¤Æ½ÅÒ·Â¼°ÄÌ¼ş·¸ÓÀÛÍµ©
	int marray, 	// Òøµ¤ÈÈÖî¼°ÄÌ¼ş·¸ÓÀÛÍµ©
	int toindex, 	// ²¾±åÒøµ¤£¢
	int itemnum
);
//
//-------------------------------------------------------------------

#endif 

