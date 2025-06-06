#include "version.h"
//
#include "gmsv_server.h"
#include "saac_client.h"
//
#include "config_file.h"
#include "readmap.h"
#include "object.h"
#include "char.h"
#include "char_base.h"
#include "npcutil.h"
#include "family.h"
#include "log.h"
#include "handletime.h"
#include "buf.h"
#include "net.h"
#include "char_base.h"
#include "battle.h"
#include "npc_bus.h"
#include "char_talk.h"
#include "npc_scheduleman.h"
#ifdef _FM_MODIFY
#include "npc_fmdengon.h"
#endif
#ifdef _ALLBLUES_LUA   
#include "mylua/function.h"
#endif
#ifdef _SASQL
#include "longzoro/sasql.h"
#endif


#define CHAR_MAXNAME 32
#define CHAR_MAXID 20
#define MINFMLEVLEFORPOINT 3
#ifdef _FAMILY_MANORNUM_CHANGE
#else
#define	MANORNUM 4
#endif


int	familyNumTotal = 0;
char	familyListBuf[MAXFAMILYLIST];

int	channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];
int	familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

int	familyTax[FAMILY_MAXNUM];

extern	tagRidePetTable ridePetTable[296];

void LeaveMemberIndex( int char_index, int fmindexi);


// Arminius: ȡ�ü��� pk dp ����/��ʧֵ
// getFMdpAward
// arg: windp=winner's fmdp	losedp=loser's fmdp
// ret: dp award
//
#ifdef _MERGE_NEW_8
int fmdplevelexp[]={0,			// 0
					150000,		// 1
					500000,		// 2
					1000000,	// 3
					1500000,	// 4
					2000000,	// 5
					2500000,	// 6
					3500000,	// 7
					4500000,	// 8
					5000000,	// 9
					7000000		//10
					};
#else
#ifdef _REDUCE_MERGE_FAME
int fmdplevelexp[]={0,			// 0
					1000,		// 1
					3000,		// 2
					10000,		// 3
					50000,		// 4
					150000,	// 5
					500000,	// 6
					1000000,	// 7
					2000000,	// 8
					10000000,	// 9
					20000000	//10
					};
#else
int fmdplevelexp[]={0,			// 0
					10000,		// 1
					30000,		// 2
					100000,		// 3
					500000,		// 4
					1500000,	// 5
					5000000,	// 6
					15000000,	// 7
					50000000,	// 8
					200000000,	// 9
					500000000	//10
					};
#endif
#endif
// Arminius end

// shan begin
int getFmLv(int playerindex)
{
    int i, dp;
    dp = CHAR_getWorkInt(playerindex, CHAR_WORKFMDP);
    if( dp > fmdplevelexp[10] ){
//        print("\n player DP->%d",dp);
        return 10;
    }
    for(i=0; i<=10; i++)
        if( dp <= fmdplevelexp[i+1] ) break;
	// Nuke 20040217: Open the merge limit
	//if(i>=9) i = 8;
	if (i>=10) i=10;
	
    return i;
}

struct FMMEMBER_LIST memberlist[FAMILY_MAXNUM];
struct FMS_MEMO      fmsmemo;
struct FM_POINTLIST  fmpointlist;
#ifdef _NEW_MANOR_LAW
ManorSchedule_t	ManorSchedule[MANORNUM];
#endif
struct FMS_DPTOP     fmdptop; 
struct FM_PKFLOOR    fmpkflnum[FAMILY_FMPKFLOOR]=
{
	{142},
	{143},
	{144},
	{145},
	{146},
	{1042},
	{2032},
	{3032},
	{4032},
#ifdef _FAMILY_MANORNUM_CHANGE	// CoolFish �����޸�װ԰����
	{5032},
	{6032},
	{7032},
	{8032},
	{9032},
	{10032},
#endif
};
int leaderdengonindex = 0;
// shan end ����ͼ���赽 family.h ���� FAMILY_FMPKFLOOR ����

void SetFMPetVarInit(int meindex)
{
	int i = 0, petindex = 0;
#ifdef _FMVER21	
	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
#else
	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)
#endif	
	{
		for (i = 0; i < CHAR_MAXPETHAVE; i++)
		{
			petindex = CHAR_getCharPet(meindex, i);
			if (!CHAR_CHECKINDEX(petindex))	continue;
			CHAR_setInt(petindex, CHAR_PETFAMILY, -1);
		}
   	}
}

void SetFMVarInit(int meindex)
{
    SetFMPetVarInit(meindex); //
   	CHAR_setInt(meindex, CHAR_FMINDEX, -1);
   	CHAR_setChar(meindex, CHAR_FMNAME, "");
   	CHAR_setInt(meindex, CHAR_FMSPRITE, -1);
#ifdef _FMVER21   	
   	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_NONE);
#else
   	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, -1);
#endif   	
   	CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, -1);
   	CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, -1);
   	CHAR_setWorkInt(meindex, CHAR_WORKFMCHARINDEX, -1);
}

void FAMILY_Init( void )
{
	int i, j, k;
	for( i=0; i<FAMILY_MAXNUM; i++)	{
	    for( j=0; j<FAMILY_MAXCHANNEL; j++)
	        for( k=0; k<FAMILY_MAXMEMBER; k++)
	        	channelMember[i][j][k] = -1;
	}

	for(i=0; i<FAMILY_MAXNUM; i++)
	    for(j=0; j<FAMILY_MAXMEMBER; j++ )
	    	familyMemberIndex[i][j] = -1;	    
#ifdef _NEW_MANOR_LAW
	for(i=0;i<FAMILY_MAXHOME;i++){
		fmpointlist.fm_momentum[i] = -1;
		fmpointlist.fm_inwar[i] = FALSE;
	}
	memset(&ManorSchedule,0,sizeof(ManorSchedule));
	for(j=0;j<MANORNUM;j++){
		for(i=0;i<10;i++){
			ManorSchedule[j].iSort[i] = ManorSchedule[j].iFmIndex[i] = -1;
		}
	}
#endif		
	familyListBuf[0] = '\0';
	SaacClient__ACShowFMList_send( acfd );
	// print("FamilyData_Init:%s ", familyListBuf );
}


void CHAR_Family(int fd, int index, char *message)
{
   char		firstToken[64];
   char		messageeraseescape[512];
   char*	messagebody;
   {
      if (*message == 0) return;
      CHAR_getMessageBody(message, firstToken, sizeof(firstToken), &messagebody);
      if (!messagebody)	return;
      strcpysafe(messageeraseescape, sizeof(messageeraseescape), messagebody);
      makeStringFromEscaped(messageeraseescape);
      switch(tolower(firstToken[0]))
      {
			case 'a':
				// ��������
				FAMILY_Add(fd, index, message);
				break;
			case 'j':
				// �������
				FAMILY_Join(fd, index, message);
				break;
			case 'e':
				// �뿪���˳�����
				FAMILY_Leave(fd, index, message);
				break;
			case 'm':
				// �峤���
				FAMILY_CheckMember(fd, index, message);
				break;         
			case 's':
				// ȡ�ü����������
				FAMILY_Detail(fd, index, message);
				break;
			case 'c':
				// ����Ƶ��
				FAMILY_Channel(fd, index, message);
				break;
			case 'b':
				// ��������
				FAMILY_Bank(fd, index, message);
				break;
			case 'p':
				// �������ݵ�
				FAMILY_SetPoint(fd, index, message);
				break;
			case 't':
				// �Ƿ������ļ��Ա
				FAMILY_SetAcceptFlag(fd, index, message);
				break;
			case 'x':
				// �޸ļ�����ּ
				FAMILY_FixRule( fd, index, message );
				break;
#ifdef _RIDEFLG_
			case 'r':
				// ��˳���
				if(!FAMILY_RidePet( fd, index, message )){
					CHAR_setInt( index , CHAR_RIDEPET, -1 );
					CHAR_setInt( index , CHAR_BASEIMAGENUMBER , CHAR_getInt( index , CHAR_BASEBASEIMAGENUMBER) );
					CHAR_complianceParameter( index );
					CHAR_sendCToArroundCharacter( CHAR_getWorkInt( index, CHAR_WORKOBJINDEX ));
					CHAR_send_P_StatusString( index , CHAR_P_STRING_RIDEPET);
				}
				break;
#endif
			case 'l':
				// �峤����
				FAMILY_LeaderFunc( fd, index, message );
				break;
#ifdef _FM_MODIFY
				// ���岼��������
			case 'd':
#ifdef _UN_FMMEMO
#else
				NPC_FmDengonLooked(0,index);
#endif
				break;
#endif
			default:
				break;
      }
   }
}

int CheckFMLeader(int meindex)
{
   if (CHAR_getInt(meindex, CHAR_FMINDEX) > 0)	return -1;
   if (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") != 0)	return -1;
#ifdef _FMVER21
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)	return -1;
#else
   if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1)	return -1;
#endif   
   return 1;
}

/**
 * 检查角色是否为家族族长。
 * 
 * @param char_index 角色索引
 * @return int 如果是族长返回-1，否则返回1
 */
int CheckFMMember(int char_index)
{
   if (CHAR_getInt(char_index, CHAR_FMINDEX) > 0) return -1;
   if (strcmp(CHAR_getChar(char_index, CHAR_FMNAME), "") != 0) return -1;
#ifdef _FMVER21
   if (CHAR_getInt(char_index, CHAR_FMLEADERFLAG) > 0 &&
       CHAR_getInt(char_index, CHAR_FMLEADERFLAG) != FMMEMBER_APPLY) return -1;
#else
   if (CHAR_getInt(char_index, CHAR_FMLEADERFLAG) > 0) return -1;
#endif       
   return 1;
}

/**
 * 检查角色是否为家族成员。
 *
 * @param char_index 角色索引
 * @return int 如果是家族成员返回-1，否则返回1
 */

int CheckLeaderQ(int meindex)
{
   if (CHAR_getInt(meindex, CHAR_LV) < FMLEADERLV
   	&& CHAR_getInt(meindex, CHAR_TRANSMIGRATION) <= 0)
   	return -1;
   if (!NPC_EventCheckFlg(meindex, 4))
   	return -2;
   return 0;
}

/**
 * 添加家族成员。
 *
 * @param fd 连接描述符
 * @param meindex 角色索引
 * @param message 消息
 */
void FAMILY_Add(int fd, int meindex, char* message)
{
	char token[128], fmname[128], charname[128], charid[128];
	char petname[128], fmrule[256], petattr[256], buf[1024];
	int charlv, havepetindex, petindex, fmsprite = 0, chargrano;
	int gold, tmpflag;
#ifdef _FAMILYBADGE_
	int fmbadge = -1;
#endif
	if (!CHAR_CHECKINDEX(meindex))	return;
	
	if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
		return;
	if (CHAR_getInt(meindex, CHAR_FMINDEX) >= 0 
		&& strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") != 0)
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n���Ѿ���������ޡ��޷��ٳ������壡", buf, sizeof(buf)));
   	return;
	}
	tmpflag = CheckLeaderQ(meindex);
	if(tmpflag == -1)
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�ܱ�Ǹร���ĵȼ����㣡", buf, sizeof(buf)));
   	return;
	}
	
	if(tmpflag == -2)
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�ܱ�Ǹร����������ɳ�������У�", buf, sizeof(buf)));
   	return;   
	}

	gold = CHAR_getInt(meindex, CHAR_GOLD);
	if( gold < 1000 )
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�ܱ�Ǹร�����������Ҫ1000ʯ�ҵ������ѣ�", buf, sizeof(buf)));
   	return;
	}
	else {
   	CHAR_setInt( meindex, CHAR_GOLD, gold-1000 );
   	CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
	sprintf(fmname, "%s", token);
	if ((strstr(fmname, " ")) || (strcmp(fmname, "") == 0) || (strstr(fmname, "��")))
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�����������������ո�", buf, sizeof(buf)));
   	return;
	}
	if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
	havepetindex = atoi(token);
	petindex = CHAR_getCharPet(meindex, havepetindex);
	if (!CHAR_CHECKINDEX(petindex))
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n��ѡ��һֻ������Ϊ�����ػ��ޣ�", buf, sizeof(buf)));
   	return;
	}
	if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
	fmsprite = atoi(token);
	if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
	if (strcmp(token, "") == 0)
   	sprintf(fmrule, "��");
	else
   	sprintf(fmrule, "%s", token);

#ifdef _FAMILYBADGE_
	if (getStringFromIndexWithDelim(message, "|", 6, token,
   	sizeof(token)) == FALSE) {
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n����������ô���", buf, sizeof(buf)));
		return;
	}
	fmbadge = atoi(token);
	extern int FamilyBadgeData[];
	extern int FamilyBadgeDataNum;
	if(fmbadge <0 || fmbadge>FamilyBadgeDataNum){
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n����������ô���", buf, sizeof(buf)));
		return;
	}
	fmbadge = FamilyBadgeData[fmbadge];
#endif

	sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
	sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
	charlv = CHAR_getInt(meindex, CHAR_LV);
	chargrano = CHAR_getInt(meindex, CHAR_FACEIMAGENUMBER);
	if (strlen(CHAR_getChar(petindex, CHAR_USERPETNAME)) == 0)
   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_NAME));
	else
   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_USERPETNAME));
	sprintf(petattr, "%d %d %d %d", 
   	CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER),
   	CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
   	CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
   	CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
	CHAR_setInt(petindex, CHAR_PETFAMILY, 1);
	CHAR_setChar(meindex, CHAR_FMNAME, fmname);
#ifdef _FMVER21
	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_LEADER);
#else
	CHAR_setInt(meindex, CHAR_FMLEADERFLAG, 1);
#endif   
	CHAR_setInt(meindex, CHAR_FMSPRITE, fmsprite);

	//   print("%s %s %s %d %s %s %s %d %d\n", fmname, charname, charid, charlv, petname,
	//   	petattr, fmrule, fmsprite, chargrano);
#ifdef _PERSONAL_FAME
	SaacClient__ACAddFM_send(acfd, fmname, charname, charid, charlv,
   	petname, petattr, fmrule, fmsprite, chargrano,
   	CHAR_getInt(meindex, CHAR_FAME),
#ifdef _FAMILYBADGE_
   	fmbadge,
#endif
   	CONNECT_getFdid(fd));
	//   print("ACAddFM acfd:%d meindex:%d fmname:%s charname:%s fame:%d Connectfd:%d fd:%d\n",
	//   	acfd, meindex, fmname, charname, CHAR_getInt(meindex, CHAR_FAME), CONNECT_getFdid(fd), fd);
#else
	SaacClient__ACAddFM_send(acfd, fmname, charname, charid, charlv,
   	petname, petattr, fmrule, fmsprite, chargrano, CONNECT_getFdid(fd));
#endif
	//SaacClient__ACShowFMList_send( acfd );
	
}



/**
 * 添加家族（族长创建家族）的处理函数。
 *
 * 该函数会根据传入的参数，组装家族相关信息，并调用 SaacClient__ACAddFM_send
 * 向 SAAC 服务器发送创建家族的请求。会设置角色和宠物的家族相关属性。
 *
 * @param fd        连接描述符
 * @param meindex   角色索引
 * @param message   客户端传来的消息字符串
 */

void ACAddFM(int fd, int result, int fmindex, int index)
{
   int meindex = CONNECT_getCharaindex(fd);
   char buf[1024];
   
//   print("ACAddFM result:%d fmindex:%d meindex:%d\n", result, fmindex, meindex); // test

   if (!CHAR_CHECKINDEX(meindex))	return;

//   print("ACAddFM_2!\n");
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;

//   print("ACAddFM_3!\n");

  if(result == 1)
	{
		CHAR_setInt(meindex, CHAR_FMINDEX, fmindex);
		CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);
		CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, 0);
#ifdef _NEW_MANOR_LAW
		CHAR_setInt(meindex,CHAR_MOMENTUM,0);
		CHAR_talkToCli(meindex,-1,"��������������ƹ���",CHAR_COLORYELLOW);
#endif
		 GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			 WINDOW_BUTTONTYPE_OK,
			 -1, -1,
			 makeEscapeString( "\n��ϲ��������µļ��壡�����ڣ���֮���ټ������������˼��룬��Ȼ��ȡ�������ʸ�ม�", buf, sizeof(buf)));
		 JoinMemberIndex( meindex, index);
		 CHAR_charSaveFromConnect(meindex, FALSE);
		 
		 // Ҫ�����¼�������
		 SaacClient__ACShowFMList_send( acfd );
		 SaacClient__ACShowMemberList_send( acfd, index );
		 SaacClient__ACShowTopFMList_send(acfd, FM_TOP_INTEGRATE);
#ifdef _NEW_MANOR_LAW
		 SaacClient__ACShowTopFMList_send(acfd, FM_TOP_MOMENTUM);
#endif
		 LogFamily(
			 CHAR_getChar( meindex, CHAR_FMNAME),
			 CHAR_getInt( meindex, CHAR_FMINDEX),
			 CHAR_getChar( meindex, CHAR_NAME),
			 CHAR_getChar( meindex, CHAR_CDKEY),
			 "ADDFAMILY(��������)",
			 ""
			 );
   }
   else
   {
   	int i = 0, petindex = 0;
   	char tmpbuf[256];
   	int gold = CHAR_getInt(meindex, CHAR_GOLD);
   	CHAR_setInt(meindex, CHAR_GOLD, gold + 10000000);
   	CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
   	SetFMVarInit(meindex);
   	
   	for (i = 0; i < CHAR_MAXPETHAVE; i++)
   	{
   	   petindex = CHAR_getCharPet(meindex, i);
   	   if (!CHAR_CHECKINDEX(petindex))	continue;
   	   CHAR_setInt(petindex, CHAR_PETFAMILY, -1);
   	}
   	if (fmindex == -2)
   		sprintf(tmpbuf, "\n�Ѿ�����ͬ���ֵļ�������ˣ�");
   	else
   		sprintf(tmpbuf, "\n�����������ʧ�ܣ�");
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString(tmpbuf, buf, sizeof(buf)));
   }
   CHAR_sendStatusString( meindex, "F");
}

void FAMILY_Join(int fd, int meindex, char *message)
{
   int fmindex, charlv, index, fmsprite;
   char token[128], fmname[128], charname[128], charid[128], buf[1024];

   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;

   if (CheckFMMember(meindex) < 0){
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n���Ѿ���������������ร�", buf, sizeof(buf)));
		return;
   }

#ifdef _FM_JOINLIMIT
	if( CHAR_getInt( meindex, CHAR_FMTIMELIMIT ) > (int)time(NULL) ){
		char buff[255];
		sprintf(buff, "\n��֮ǰ�˳����壬\n����%dСʱ�����ټ������ร�",(CHAR_getInt( meindex, CHAR_FMTIMELIMIT )-(int)time(NULL))/3600+1);
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			makeEscapeString( buff, buf, sizeof(buf)));
		return;
	}
#endif

   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   index = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   fmindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   sprintf(fmname, "%s", token);
   if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
   fmsprite = atoi(token);

	 if(memberlist[index].fmnum > FAMILY_MAXMEMBER){
			GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString( "\n�˼���Ŀǰ�޷����ճ�Ա�������Ա�����ѵ������ޣ�", buf, sizeof(buf)));
			return;
	 }
   
   sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
   sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
   charlv = CHAR_getInt(meindex, CHAR_LV);
   CHAR_setInt(meindex, CHAR_FMINDEX, fmindex);
   CHAR_setChar(meindex, CHAR_FMNAME, fmname);
#ifdef _FMVER21
   CHAR_setInt(meindex, CHAR_FMLEADERFLAG, FMMEMBER_APPLY);
#else
   CHAR_setInt(meindex, CHAR_FMLEADERFLAG, 0);
#endif   
   CHAR_setInt(meindex, CHAR_FMSPRITE, fmsprite);
   CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);

//   print("JoinFM index:%d fmindex:%d fmname:%s charname:%s charid:%s charlv:%d sprite:%d\n",
//   	index, fmindex, fmname, charname, charid, charlv, fmsprite);
#ifdef _PERSONAL_FAME	// Arminius: �����������
//   print("fame:%d charfdid:%d\n", CHAR_getInt(meindex, CHAR_FAME),
//   	CONNECT_getFdid(fd));
   SaacClient__ACJoinFM_send(acfd, fmname, fmindex, charname, charid, charlv,
   	index, CHAR_getInt(meindex, CHAR_FAME), CONNECT_getFdid(fd));
#else   
//   print("charfdid:%d\n", CONNECT_getFdid(fd));
   SaacClient__ACJoinFM_send(acfd, fmname, fmindex, charname, charid, charlv,
   	index, CONNECT_getFdid(fd));
#endif
}

void ACJoinFM(int fd, int result, int recv)
{
  int meindex = CONNECT_getCharaindex(fd);
  char buf[1024];
   
  if(!CHAR_CHECKINDEX(meindex))	return;
  
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;

   if (result == 1) {
		 GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\nлл��ļ������룡���ȵ��峤��������ͨ��֮�ᣬ������ʽ���롣", buf, sizeof(buf)));
		 
		  JoinMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
#ifdef _NEW_MANOR_LAW
			CHAR_setInt(meindex,CHAR_MOMENTUM,0);
			CHAR_talkToCli(meindex,-1,"�������������ƹ���",CHAR_COLORYELLOW);
#endif
		 
		sprintf(buf,"fame:%d",CHAR_getInt(meindex,CHAR_FAME));
		 
		 LogFamily(
			 CHAR_getChar( meindex, CHAR_FMNAME),
			 CHAR_getInt( meindex, CHAR_FMINDEX),
			 CHAR_getChar( meindex, CHAR_NAME),
			 CHAR_getChar( meindex, CHAR_CDKEY),
			 "JOINFAMILY(����������)",
			 buf
			 );
		 
   }
   else
   {
	SetFMVarInit(meindex);
   	if (recv == -2)
   	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�˼���Ŀǰ��Ը�����ճ�Ա��", buf, sizeof(buf)));
	}
	else if (recv == -3)
	{
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�˼���Ŀǰ�޷����ճ�Ա�������Ա�����ѵ������ޣ�", buf, sizeof(buf)));
	}
	else
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n����������ʧ�ܣ�", buf, sizeof(buf)));
   }
   
   CHAR_sendStatusString( meindex, "F");   
}

void FAMILY_Leave(int fd, int meindex, char *message)
{
   int result, fmindex, index;
   char token[128], fmname[128], charname[128], charid[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_NONE))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == -1))
#endif      
   {
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�㲢û�м������ร�", buf, sizeof(buf)));
      	return;
   }

   {
	   int i, fmpks_pos;
		 for( i=1; i<=MANORNUM; i++){ // CoolFish 4->MANORNUM 2002/2/25
			   fmpks_pos = i * MAX_SCHEDULE;
			  if(fmpks[fmpks_pos+1].host_index > -1 && fmpks[fmpks_pos+1].guest_index > -1){
#ifdef _FMVER21
         	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER){
#else
         	if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1){
#endif         
			     if( (fmpks[fmpks_pos+1].host_index+1)  == CHAR_getInt(meindex, CHAR_FMINDEX) ||
	    			   (fmpks[fmpks_pos+1].guest_index+1) == CHAR_getInt(meindex, CHAR_FMINDEX) ){
		    		   GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			                             WINDOW_BUTTONTYPE_OK,
			                             -1, -1,
			                             makeEscapeString( "\n��Ŀǰ�ļ�����Լս�У�����޷���ɢ���壡", buf, sizeof(buf)));
	      	     return;
	      	 }
			   }
		   }
	   }
   }

   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
   if (result == 1)
   {
      fmindex = CHAR_getInt(meindex, CHAR_FMINDEX);
      index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
      sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
      sprintf(charname, "%s", CHAR_getChar(meindex, CHAR_NAME));
      sprintf(charid, "%s", CHAR_getChar(meindex, CHAR_CDKEY));
#ifdef _FMVER21
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1) 
#endif      
			{
         SaacClient__ACDelFM_send(acfd, fmname, fmindex, index, charname, charid,CONNECT_getFdid(fd));
      }else {
				 SaacClient__ACLeaveFM_send(acfd, fmname, fmindex, charname, charid, index, CONNECT_getFdid(fd));
      }
   }
}

void ACLeaveFM( int fd, int result, int resultflag)
{
	int meindex = CONNECT_getCharaindex(fd);
	char buf[1024];
	
	if (!CHAR_CHECKINDEX(meindex))	return;
	if (result == 1){
		// won 2002/01/05
		LogFamily(		
			CHAR_getChar( meindex, CHAR_FMNAME),
			CHAR_getInt( meindex, CHAR_FMINDEX),
			CHAR_getChar( meindex, CHAR_NAME),
			CHAR_getChar( meindex, CHAR_CDKEY),
	    	"LEAVEFAMILY(�뿪����)",
				""
				);
		if(  CHAR_getWorkInt( meindex, CHAR_WORKFMCHANNEL) != -1 )
			CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNEL, -1 );
		LeaveMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
		SetFMVarInit(meindex);
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK, -1, -1,
			makeEscapeString( "\n�����˳�����ϣˣ�", buf, sizeof(buf)));
		CHAR_setWorkInt( meindex, CHAR_WORKFMFLOOR, -1);
#ifdef _FM_JOINLIMIT
		CHAR_setInt( meindex, CHAR_FMTIMELIMIT, (int)time(NULL)+getJoinFamilyTime()*(60*60) );
#endif
#ifdef _NEW_MANOR_LAW
		CHAR_setInt(meindex,CHAR_MOMENTUM,0);
		CHAR_talkToCli(meindex,-1,"�˳�����������ƹ���",CHAR_COLORYELLOW);
#endif
		CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
		CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
		CHAR_complianceParameter( meindex );
		CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
		CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
#ifdef _FM_METAMO
			CHAR_ReMetamo(meindex);
#endif
	}else
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK, -1, -1,
		makeEscapeString( "\n�����˳�����ʧ�ܣ�", buf, sizeof(buf)));
	
	CHAR_sendStatusString( meindex, "F" );
}

void ACDelFM(int fd, int result)
{
   char buf[1024];
   int meindex = CONNECT_getCharaindex(fd);
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   //if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
   //   || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
   //      return;
         
   if (result == 1)
   {
// WON ADD
	LogFamily(
		CHAR_getChar( meindex, CHAR_FMNAME),
		CHAR_getInt( meindex, CHAR_FMINDEX),
		CHAR_getChar( meindex, CHAR_NAME),
		CHAR_getChar( meindex, CHAR_CDKEY),
		"DELFAMILY(�����ɢ)",
		""
	);

        LeaveMemberIndex( meindex, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) );
        /*
        for( i=0; i<FAMILY_MAXMEMBER; i++)
        {
      			familyMemberIndex[ CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI) ][i] = -1;
        }
        */
        
	SetFMVarInit(meindex);
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�����Ѿ���ɢ�ˣ�", buf, sizeof(buf)));

	// Ҫ�����¼����б�
	SaacClient__ACShowFMList_send( acfd );

// won �Ƶ�ǰ��ȥ
/*
	LogFamily(
		CHAR_getChar( meindex, CHAR_FMNAME),
		CHAR_getInt( meindex, CHAR_FMINDEX),
		CHAR_getChar( meindex, CHAR_NAME),
		CHAR_getChar( meindex, CHAR_CDKEY),
		"DELFAMILY(�����ɢ)",
		""
	);
*/
   }
   else
	 GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�����ɢ����ʧ�ܣ�", buf, sizeof(buf)));

   CHAR_sendStatusString( meindex, "F");
}

void ACShowFMList(int result, int fmnum, char *data)
{
   if( result != 1 )
   	return;

   //print("\ngetFamilyListFromAC:%d", fmnum );
   familyNumTotal = fmnum;
   strcpy( familyListBuf, data );   
   
//   print( "FamilyData:%s ", familyListBuf );
}

void ACShowMemberList(int result, int index, int fmnum, int fmacceptflag, int fmjoinnum,char *data
#ifdef _FAMILYBADGE_
	,int badge
#endif
	)
{
   int  j;
   char numberid[3];
   char tmpbuf[128];
   if(result==0){
       return;
   }else{
       if(fmnum == -1){
           print("\n FamilyWorkIndex Error!!");
       }
       for(j=1;j<=FAMILY_MAXMEMBER;j++){
           strcpy(memberlist[index].numberlistarray[j-1],"");
       }
       for(j=1;j<=fmnum;j++){
       		 if(j > FAMILY_MAXMEMBER)break;
           if(getStringFromIndexWithDelim(data," ",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
               return;
           strcpy(memberlist[index].numberlistarray[j-1],tmpbuf);    
           getStringFromIndexWithDelim(tmpbuf,"|",1,numberid,sizeof(numberid));
           memberlist[index].memberindex[j-1] = atoi(numberid);
       }
       memberlist[index].fmnum  = fmnum;
       memberlist[index].fmjoinnum = fmjoinnum;
       memberlist[index].accept = fmacceptflag;
#ifdef _FAMILYBADGE_
	   memberlist[index].badge = badge;
#endif
       return;
   }
}

void ACShowDpTop(int result,int num, char *data, int kindflag)
{
    int i;
    char tmpbuf[256],tmpbuf1[64];
    if(result==0){
        return;
    }else{
        switch( kindflag )
        {
            case FM_TOP_INTEGRATE:
            {
                for(i=0; i<FAMILY_MAXNUM; i++){
                    strcpy(fmdptop.topmemo[i], "");
                    fmdptop.fmtopid[i] = -1;
#ifdef _FMVER21
                    fmdptop.fmtopdp[i] = -1;
#endif
#ifdef _NEW_MANOR_LAW
										fmdptop.fmMomentum[i] = -1;
										fmdptop.momentum_topid[i] = -1;
										strcpy(fmdptop.momentum_topmemo[i], "");
#endif
                }
                fmdptop.num = num;
                for(i=0; i<fmdptop.num; i++){
                   if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                       return;
                   strcpy(fmdptop.topmemo[i], tmpbuf);
                   // family index
                   getStringFromIndexWithDelim( tmpbuf, "|", 1, tmpbuf1, sizeof(tmpbuf1));
                   fmdptop.fmtopid[i] = atoi(tmpbuf1);
#ifdef _FMVER21
                   // family popularity
                   getStringFromIndexWithDelim( tmpbuf, "|", 6, tmpbuf1, sizeof(tmpbuf1));
                   fmdptop.fmtopdp[i] = atoi(tmpbuf1);                   
#endif                   
#ifdef _NEW_MANOR_LAW
									 getStringFromIndexWithDelim( tmpbuf, "|", 7, tmpbuf1, sizeof(tmpbuf1));
                   fmdptop.fmMomentum[i] = atoi(tmpbuf1);
#endif
                }
            }
            break;
            case FM_TOP_ADV:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.adv_topmemo[i], "");
                fmdptop.adv_num = num;
                for(i=0; i<fmdptop.adv_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.adv_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_FEED:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.feed_topmemo[i], "");
                fmdptop.feed_num = num;
                for(i=0; i<fmdptop.feed_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.feed_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_SYNTHESIZE:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.syn_topmemo[i], "");
                fmdptop.syn_num = num;
                for(i=0; i<fmdptop.syn_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.syn_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_DEALFOOD:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.food_topmemo[i], ""); 
                fmdptop.food_num = num;
                for(i=0; i<fmdptop.food_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.food_topmemo[i], tmpbuf);
                }                
            }
            break;
            case FM_TOP_PK:
            {
                for(i=0; i<30; i++)
                    strcpy(fmdptop.pk_topmemo[i], "");
                fmdptop.pk_num = num;
                for(i=0; i<fmdptop.pk_num; i++){
                    if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
                        return;
                    strcpy(fmdptop.pk_topmemo[i], tmpbuf);
                }                
            }
            break;
#ifdef _NEW_MANOR_LAW
						case FM_TOP_MOMENTUM:
						{
							for(i=0; i<FAMILY_MAXNUM; i++){
								strcpy(fmdptop.momentum_topmemo[i], "");
								if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE) break;
								strcpy(fmdptop.momentum_topmemo[i],tmpbuf);
							}
							for(i=0; i<num; i++){
								if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE) return;
								getStringFromIndexWithDelim(tmpbuf,"|",1,tmpbuf1,sizeof(tmpbuf1));
								// get top id
								fmdptop.momentum_topid[i] = atoi(tmpbuf1);
							}
						}
						break;
#endif
            default:
            break;
        }
    }
}

void ACShowPointList(int result, char *data)
{
    int i;
    char tmpbuf[100];
    char tmp[100];
    if(result==0){
       return;
    }else{
        for(i=0;i<FAMILY_MAXHOME;i++){
            if(getStringFromIndexWithDelim(data," ",i+1,tmpbuf,sizeof(tmpbuf)) == FALSE)
               return;
            getStringFromIndexWithDelim(tmpbuf,"|",2,tmp,sizeof(tmp));

            if(atoi(tmp)==0){
            	strcpy(fmpointlist.pointlistarray[i],"");
            }else{
            	strcpy(fmpointlist.pointlistarray[i],tmpbuf);
            }
        }
    }
}

void ACShowFMMemo(int result, int index, int num, int dataindex, char *data)
{
   int  j;
   char tmpbuf[220];
   
   if(index==10000)
   {
       if(result==0){
           return;
       }else{
           for(j=1;j<=140;j++){
               strcpy(fmsmemo.memo[j-1],"");
           }
           for(j=1;j<=num;j++){
               if(getStringFromIndexWithDelim(data,"|",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
                    return;
               makeStringFromEscaped(tmpbuf);    
               strcpy(fmsmemo.memo[j-1],tmpbuf);    
           }
           fmsmemo.memonum   = num;
           fmsmemo.memoindex = dataindex-1;
           return;
       }
   }else{
       if(result==0){
           return;
       }else{
           if(num == -1){
                 print("\n FamilyWorkIndex Error!!");
           }

           for(j=1;j<=35;j++){
               strcpy(memberlist[index].memo[j-1],"");
           }
           for(j=1;j<=num;j++){
               if(getStringFromIndexWithDelim(data,"|",j,tmpbuf,sizeof(tmpbuf)) == FALSE) 
                   return;
               makeStringFromEscaped(tmpbuf);
               strcpy(memberlist[index].memo[j-1],tmpbuf);    
           }
           memberlist[index].memonum = num;
           memberlist[index].memoindex = dataindex-1;
           return;
       }
   }
}

#ifdef _PERSONAL_FAME   // Arminius:
void ACFMCharLogin(int fd, int result, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex, int charfame
	#ifdef _NEW_MANOR_LAW
	,int momentum
	#endif
	)
#else
void ACFMCharLogin(int fd, int result, int index, int floor, int fmdp,
	int joinflag, int fmsetupflag, int flag, int charindex)
#endif
{
   char buf[1024];
   int i, petindex;
   int meindex = CONNECT_getCharaindex(fd);
   if (!CHAR_CHECKINDEX(meindex))	return;
   if (result == 1){
#ifdef _NEW_MANOR_LAW
		 // ���ƻش�Ϊ0ʱҪ������������趨Ϊ0,��Ϊ�ش���0�п����Ǵ���ׯ԰ս,��������Ҫ����
		 if(momentum == 0) CHAR_setInt(meindex,CHAR_MOMENTUM,0);
		 else CHAR_setInt(meindex,CHAR_MOMENTUM,momentum);
#endif
		 if(charfame != CHAR_getInt(meindex,CHAR_FAME)){
			 sprintf(buf,"server fame:%d,ac fame:%d",CHAR_getInt(meindex,CHAR_FAME),charfame);
			 LogFamily(
				 CHAR_getChar(meindex, CHAR_FMNAME),
				 CHAR_getInt(meindex, CHAR_FMINDEX),
				 CHAR_getChar(meindex, CHAR_NAME),
				 CHAR_getChar(meindex, CHAR_CDKEY),
				 "ACFMCharLogin",
				 buf
				 );
		 }
		 //CHAR_setInt(meindex,CHAR_FAME,charfame);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMINDEXI, index);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMFLOOR, floor);

		 CHAR_setWorkInt(meindex, CHAR_WORKFMDP, fmdp);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMSETUPFLAG, fmsetupflag);
		 CHAR_setWorkInt(meindex, CHAR_WORKFMCHARINDEX, charindex);
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != joinflag)
			 SetFMPetVarInit(meindex);
		 CHAR_setInt(meindex, CHAR_FMLEADERFLAG, joinflag);
		
		 JoinMemberIndex(meindex, index);
		 
		 CHAR_sendStatusString(meindex, "f");
		 
		 CHAR_complianceParameter( meindex );
		 CHAR_sendStatusString( meindex , "P");
		 
		 CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex , CHAR_WORKOBJINDEX ));

#ifdef _FMVER21
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == FMMEMBER_LEADER)
#else
		 if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) == 1)
#endif        
		 {
			 for (i = 0; i < CHAR_MAXPETHAVE; i++)
			 {
				 petindex = CHAR_getCharPet(meindex, i);
				 if (!CHAR_CHECKINDEX(petindex))     continue;
				 if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1)
					 return;
			 }
			 for (i = 0; i < CHAR_MAXPOOLPETHAVE; i++)
			 {
				 petindex = CHAR_getCharPoolPet(meindex, i);
				 if (!CHAR_CHECKINDEX(petindex))     continue;
				 if (CHAR_getInt(petindex, CHAR_PETFAMILY) == 1)
					 return;
			 }
/*
			 GmsvServer_WN_send(fd, WINDOW_MESSAGETYPE_MESSAGE,
				 WINDOW_BUTTONTYPE_OK,
				 -1, -1,
				 makeEscapeString("\n�����ػ�����ʧ�ˣ�\n��������ѡ��һֻ�ػ��ޣ�\n�������������֮�����ʧࡣ�\n", buf, sizeof(buf)));
*/
				
			CHAR_talkToCli(meindex, -1, "\n�����ػ�����ʧ�ˣ�\n��������ѡ��һֻ�ػ��ޣ�\n�������������֮�����ʧࡣ�\n", CHAR_COLORRED);
			
			 SaacClient__ACFixFMData_send(acfd,
				 CHAR_getChar(meindex, CHAR_FMNAME),
				 CHAR_getInt(meindex, CHAR_FMINDEX),
				 CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_DELFMTIME,
				 "", "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
				 CONNECT_getFdid(fd));
		 }
   }
   else
   {
   	   if(flag == 0){
   	   	  SetFMVarInit(meindex);
#ifdef _FM_JOINLIMIT
			 		CHAR_setInt( meindex, CHAR_FMTIMELIMIT, (int)time(NULL)+getJoinFamilyTime()*(60*60) );
#endif
   		    CHAR_talkToCli(meindex, -1, "���Ѿ��˳����������Ѿ��������ˣ�", CHAR_COLORYELLOW);
					CHAR_setWorkInt( meindex, CHAR_WORKFMFLOOR, -1);
					CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
					CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
					CHAR_complianceParameter( meindex );
					CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
					CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
#ifdef _FM_METAMO
					CHAR_ReMetamo(meindex);
#endif
		  }
   }
}

void FAMILY_Detail(int fd, int meindex, char *message)
{
	char token[128], token2[128], fmname[128];
	char buf[1024], subbuf[256], sendbuf[2048];
	int pindex1, i, j;
	int fmindex, tempindex;
	
	if (!CHAR_CHECKINDEX(meindex))	return;
	
	if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
		return;
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
	
	if ( strcmp(token,"F") ==0 )	{
		if (getStringFromIndexWithDelim(message, "|", 3, token2,
			sizeof(token)) == FALSE)	return;
		
		strcpy( buf, "");
		j = 0;	
		
		pindex1 = (atoi(token2) - 1)*10 +1;
		
		for( i=pindex1 ; i< pindex1+10 ; i++  ) {
			if( i > familyNumTotal )	break;
			if( getStringFromIndexWithDelim( familyListBuf, "|", i, subbuf,
				sizeof(subbuf) ) == FALSE)	break;
			strcat( buf, "|" );
			strcat( buf, subbuf );
			j++;
			
			// print(" |%s| ", subbuf);
		}
		
		sprintf( sendbuf, "S|F|%d|%d|%d%s", familyNumTotal, atoi(token2), j, buf );
		GmsvServer_FM_send( fd, sendbuf );
		
		return;
		
	}
	
	// shan add
	if (strcmp(token, "P") == 0 ){
#ifdef _DATA_INFO_SHOW
	FreeDataInfoShow(meindex);
#else
		int  personfame;
		char sendbuf[512]; 
#ifdef _PERSONAL_FAME
		personfame = (CHAR_getInt( meindex, CHAR_FAME)/100);
#else
		personfame = CHAR_getWorkInt( meindex, CHAR_WORKFMDP);
#endif       
		sprintf( sendbuf, "��Ŀǰ�ĸ�����������Ϊ��%d", personfame);
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#ifdef _NEW_MANOR_LAW
		sprintf(sendbuf,"��Ŀǰ�ĸ������Ƶ���Ϊ��%d",CHAR_getInt(meindex,CHAR_MOMENTUM)/100);
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORCYAN);
#endif
#ifndef _VERSION_NEW
#ifdef _VIP_SERVER
		sprintf( sendbuf, "��Ŀǰ�ĸ��˻��ֵ���Ϊ��%d",CHAR_getInt(meindex,CHAR_AMPOINT));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#endif
#endif
#ifdef _ONLINE_SHOP
		sprintf(sendbuf,"��Ŀǰ�ĸ��˱��ҵ���Ϊ��%d",CHAR_getInt(meindex, CHAR_BJ));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORGRAY);
#endif
#ifdef _BOUND_TIME
	time_t boundtime = CHAR_getInt(meindex, CHAR_BOUNDTIME);
	if(CHAR_getInt(meindex, CHAR_BOUNDTIME)==0){
		CHAR_talkToCli(meindex, -1, "��Ŀǰ������Ȩ������δ���ã�", CHAR_COLORYELLOW);
	}else if(CHAR_getInt(meindex, CHAR_BOUNDTIME) > time(NULL)){
		struct tm *tm1 = localtime(&boundtime);
		sprintf( sendbuf, "��Ŀǰ������Ȩ������Ϊ��%4d��%2d��%2d�� %2d:%2d:%2d\n",
																										tm1->tm_year + 1900,
																										tm1->tm_mon + 1,
																										tm1->tm_mday,
																										tm1->tm_hour,
																										tm1->tm_min,
																										tm1->tm_sec);	
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
	}else{
		CHAR_talkToCli(meindex, -1, "��Ŀǰ������Ȩ�������ѹ��ڣ�", CHAR_COLORYELLOW);
	}
#endif
#ifdef _VIP_RIDE
	
	if(CHAR_getInt(meindex, CHAR_VIPTIME)==-1){
		CHAR_talkToCli(meindex, -1, "��Ŀǰ�Ļ�ԱȨ���������ã�", CHAR_COLORYELLOW);
	}else if(CHAR_getInt(meindex, CHAR_VIPTIME)==0){
		CHAR_talkToCli(meindex, -1, "��Ŀǰ�Ļ�ԱȨ������δ���ã�", CHAR_COLORYELLOW);
	}else if(CHAR_getInt(meindex, CHAR_VIPTIME) > time(NULL)){
		time_t viptime = (time_t)CHAR_getInt(meindex, CHAR_VIPTIME);
		struct tm *tm1 = localtime(&viptime);

		sprintf( sendbuf, "��Ŀǰ�Ļ�ԱȨ����Ч�ڣ�%4d��%2d��%2d�� %2d:%2d:%2d\n",
																										tm1->tm_year + 1900,
																										tm1->tm_mon + 1,
																										tm1->tm_mday,
																										tm1->tm_hour,
																										tm1->tm_min,
																										tm1->tm_sec);	
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORPURPLE);
	}else{
		CHAR_talkToCli(meindex, -1, "��Ŀǰ�Ļ�ԱȨ�������ѹ��ڣ�", CHAR_COLORYELLOW);
	}
#endif

#ifdef _CAMEO_MONEY
		sprintf(sendbuf,"��Ŀǰ�ĸ��˱�����ĿΪ��%d", CHAR_getInt(meindex,CHAR_CAMEO));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#endif

#ifdef _MISSION_TRAIN
{
	int num = CHAR_getInt(meindex, CHAR_MISSIONTRAIN_NUM) - 1;
	if(num < 0){
		CHAR_talkToCli(meindex, -1, "��Ŀǰ�����������������δ������", CHAR_COLORYELLOW);
	}else{
		sprintf(sendbuf,"��Ŀǰ�����������������%d", num);
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
	}
}
#endif

#ifdef _PLAYER_TITLE      
	char titletoken[256];
	if ( CHAR_getInt( meindex, CHAR_TITLE_LV) > 0 ){
		if(CHAR_getInt(meindex, CHAR_TITLE_TIME) > time(NULL)){
			time_t titletime = (time_t)CHAR_getInt(meindex, CHAR_TITLE_TIME);
			struct tm *tm1 = localtime(&titletime);
			
			sprintf( sendbuf, "��Ŀǰ��Ӣ�۳ƺ���Ч�ڣ�%4d��%2d��%2d�� %2d:%2d:%2d\n",
				tm1->tm_year + 1900,
			    tm1->tm_mon + 1,
			    tm1->tm_mday,
			    tm1->tm_hour,
			    tm1->tm_min,
			    tm1->tm_sec);	
			CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORBLUE);
		}
	}
#endif
#ifdef _MISSION_TIME
	if(CHAR_getInt(meindex, CHAR_MISSION_TIME) > time(NULL)){
		sprintf( sendbuf, "��Ŀǰ��������ʱ��ʣ�ࣺ%d����", (CHAR_getInt(meindex, CHAR_MISSION_TIME) - time(NULL)) / 60);
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORGREEN);
	}
#endif
#ifdef _ACTIVE_GAME
		sprintf(sendbuf,"��Ŀǰ�ĸ��˻�����ĿΪ��%d", CHAR_getInt(meindex,CHAR_ACTIVE));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORCYAN);
#endif
{
	int addexptime = CHAR_getWorkInt( meindex, CHAR_WORKITEM_ADDEXPTIME );
	int lefttime = addexptime;
	if(lefttime>0){
		sprintf(sendbuf, "��Ŀǰ�ľ�������Ч��Ϊ���������� %d\% ʣ��ʱ�� %d �� %d ��", CHAR_getWorkInt( meindex, CHAR_WORKITEM_ADDEXP ), lefttime/60, lefttime%60 );
		CHAR_talkToCli( meindex, -1, sendbuf, CHAR_COLORYELLOW);
	}
}
#ifdef _SASQL
		sprintf(sendbuf,"��Ŀǰ�ĸ��˽����Ϊ��%d",sasql_getVipPoint(meindex));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#else
#ifdef _NEW_VIP_SHOP
#ifdef _OTHER_SAAC_LINK
		if(osfd == -1){
			OtherSaacConnect();
			CHAR_talkToCli( meindex, -1, "���������δ��������!", CHAR_COLORRED );
		}else{
			CHAR_talkToCli(meindex, -1, "��ȡ����У����Ժ�...", CHAR_COLORRED);
			SaacClient__QueryPoint_send( osfd, fd, CHAR_getChar(meindex, CHAR_CDKEY));
		}
#else
		CHAR_talkToCli(meindex, -1, "��ȡ����У����Ժ�...", CHAR_COLORRED);
		SaacClient__QueryPoint_send( acfd, fd, CHAR_getChar(meindex, CHAR_CDKEY));
#endif
#endif
#endif
#ifdef _OFFLINE_TIME 
		sprintf(sendbuf,"��Ŀǰ�Ŀ�����ʱ����ӣ�%d", CHAR_getInt(meindex,CHAR_OFFLINE_TIME));
		CHAR_talkToCli(meindex, -1, sendbuf, CHAR_COLORYELLOW);
#endif
#endif
	}
	
	if(strcmp(token,"D") == 0){
		if(getStringFromIndexWithDelim(message, "|", 3, fmname, sizeof(fmname)) == FALSE)	return;
		if(getStringFromIndexWithDelim(message, "|", 4, token2, sizeof(token2)) == FALSE)	return;
		fmindex = atoi( token2 );
		if(getStringFromIndexWithDelim(message, "|", 5, token2, sizeof(token2)) == FALSE)	return;
		tempindex = atoi( token2 );
		
		//print(" send_fmname_ac:%s ", fmname);
		SaacClient__ACFMDetail_send( acfd, fmname, fmindex, tempindex, CONNECT_getFdid(fd) );
	}
	
	// shan begin
	else if (strcmp(token, "D2") ==0 ) {
		char sendbuf[2048], tmpbuf[1024], leadernamebuf[64];
		int h, i = 0;
		int meindex = CONNECT_getCharaindex(fd);
		int  fmindex_wk = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
		if( fmindex_wk < 0 || fmindex_wk >= FAMILY_MAXNUM) return;
		
		for( h=0; h<FAMILY_MAXNUM; h++)
			if( fmdptop.fmtopid[h] == fmindex_wk )
				break;
			strcpy(tmpbuf, "");
			for (i = 0 + 1; i < FAMILY_MAXHOME + 1; i++)
			{
				int fmpks_pos = i * MAX_SCHEDULE, index;
				sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
				index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
				if (fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_BATTLEBEGIN
					|| fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_PREPARE
					|| fmpks[fmpks_pos + 1].flag == FMPKS_FLAG_MANOR_OTHERPLANET)
				{
					if ((fmpks[fmpks_pos + 1].host_index == index
          		&& strcmp(fmname, fmpks[fmpks_pos + 1].host_name) == 0)
          	   || (fmpks[fmpks_pos + 1].guest_index == index
          	   	&& strcmp(fmname, fmpks[fmpks_pos + 1].guest_name) == 0))
					{
							time_t dueltime = (time_t)fmpks[fmpks_pos + 1].dueltime;
							struct tm *tm1 = localtime(&dueltime);
							
							sprintf(tmpbuf, "%d/%d %d:%d��%s�� %s ���� %s",
								tm1->tm_mon + 1, tm1->tm_mday, tm1->tm_hour, tm1->tm_min,
								fmpks[fmpks_pos + 2].host_name,
								fmpks[fmpks_pos + 1].guest_name,
								fmpks[fmpks_pos + 1].host_name);
					}
				}
			}
			if (strcmp(tmpbuf, "") == 0)
				sprintf(tmpbuf, "����ս�ų�");
      
      getStringFromIndexWithDelim(memberlist[fmindex_wk].numberlistarray[0],
				"|",2,leadernamebuf,sizeof(leadernamebuf));
      // sendbuf -> ��������|����|�峤����|��������|��������|��������|����ְλ|���徫��|PK
#ifdef _VERSION_NEW
			sprintf( sendbuf, "%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%d", 
#else
#ifdef _NEW_MANOR_LAW
			sprintf( sendbuf, "%s|%d|%s|%d|%d|%d|%d|%d|%s|%d|%d", 
#else
				sprintf( sendbuf, "%s|%d|%s|%d|%d|%d|%d|%d|%s", 
#endif
#endif
				CHAR_getChar(meindex, CHAR_FMNAME),
				memberlist[fmindex_wk].fmjoinnum,
				leadernamebuf,
				h+1,
#ifdef _FMVER21
				fmdptop.fmtopdp[h],
#else
				(CHAR_getWorkInt( meindex, CHAR_WORKFMDP)/100),
#endif                
#ifdef _PERSONAL_FAME
				(CHAR_getInt( meindex, CHAR_FAME)/100),
#else
				(CHAR_getWorkInt( meindex, CHAR_WORKFMDP)/100),
#endif
				CHAR_getInt( meindex, CHAR_FMLEADERFLAG),
				CHAR_getInt( meindex, CHAR_FMSPRITE ),
				tmpbuf

#ifdef _NEW_MANOR_LAW
				,fmdptop.fmMomentum[h]/100	// ��������
				,CHAR_getInt(meindex,CHAR_MOMENTUM)/100 // ��������
#endif
				);
#ifdef _FAMILYBADGE_
				sprintf(sendbuf+strlen(sendbuf),"|%d",memberlist[fmindex_wk].badge);
#endif
				GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_FAMILYDETAIL,
				WINDOW_BUTTONTYPE_OK,
				-1,
				-1,
				makeEscapeString( sendbuf, buf, sizeof(buf)));
	}
	// shan end
}

void ACFMDetail(int ret, char *data, int clifd)
{
	char sendbuf[1024];
	
	//print(" Detail:%s ", data);
	
	if( ret != 1 )
	{
		print(" ACFMDetailError!:%d ", clifd );
		return;
	}
	//print(" ACFMDetail:%d:%s ", clifd, data );
	
	/*	
	len = strlen(data);
	strcpy( buf, data );
	
	for( i=0 ; i<len ; i++ )
	{
		if( data[i] == '|' )	buf[i] = ' ';
		else 
		if( data[i] == ' ' )    buf[i] = '|';
	}
	*/
	//if (getStringFromIndexWithDelim(message, "|", 10, fmname,
   	//	sizeof(fmname)) == FALSE)	return;	

	//makeStringFromEscaped( buf );
	//buf2 = GmsvServer_demkstr_string( buf );

	
	sprintf(sendbuf, "S|D|%s", data); 
	GmsvServer_FM_send( clifd, sendbuf );
	//print(" Detail:%s ", sendbuf);
	/*
	GmsvServer_WN_send( clifd, WINDOW_MESSAGETYPE_FAMILYDETAIL,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		sendbuf );
	*/
	
	// from saac data
	/*	
	sprintf(data, "%d|%d|%s|%s|%d|%s|%s|%d|%d|%s|%d", index, family[index].fmindex,
           family[index].fmname, family[index].fmleadername,
           family[index].fmleadergrano, family[index].petname, family[index].petattr,
           family[index].fmjoinnum, family[index].fmacceptflag,
           family[index].fmrule, family[index].fmsprite );
        */                           
	
}

void FAMILY_CheckMember(int fd, int meindex, char *message)
{
   int result, charindex, i, index = 0;
   char token[128], charname[128], fmname[128], buf[1024];
   if (!CHAR_CHECKINDEX(meindex)){
   	return;
   }
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
   {
         return;
   }
   index = CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI);
   sprintf(fmname, "%s", CHAR_getChar(meindex, CHAR_FMNAME));
   if ((index == -1) || (strcmp(fmname, "") == 0)
#ifdef _FMVER21
//      || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
//          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)
//          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)))
      || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
          && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
//   	print("leaderflag:%d\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG));
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�㲻���峤������û���޸ĵ�Ȩ��ࡣ�", buf, sizeof(buf)));
      	return;
   }   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   sprintf(charname, "%s", token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   charindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
//   print("MemberCheck charname:%s charindex:%d mename:%s meworki:%d\n",
//   	charname, charindex, CHAR_getChar(meindex, CHAR_NAME),
//   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI));
   if ((strcmp(charname, CHAR_getChar(meindex, CHAR_NAME)) == 0)
   	&& (charindex == CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI)))
   		return;
//   print("MemberCheck message:%s\n", message);
   if (result == FMMEMBER_MEMBER)
#ifdef _FMVER21
   {
	// shan begin
    char sbuf[1024];
	sprintf( sbuf, "�峤����:%d -> ��������:%s �������� (�������Ϊ��Ա):%d\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	LogFamily(
		CHAR_getChar(meindex, CHAR_FMNAME),
		CHAR_getInt(meindex, CHAR_FMINDEX),
		CHAR_getChar(meindex, CHAR_NAME),
		CHAR_getChar(meindex, CHAR_CDKEY),
		"CheckMember",
		sbuf
		);
	// shan end

   	SaacClient__ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));	
   }
#else
   {
   	SaacClient__ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CONNECT_getFdid(fd));
   }
#endif
#ifdef _FMVER21
   else if (result == FMMEMBER_NONE)
#else
   else if (result == 4)
#endif
   {
      for (i = 0 + 1; i < FAMILY_MAXHOME + 1; i++)
      {
      	 int fmpks_pos = i * MAX_SCHEDULE;
      	 if ((fmpks[fmpks_pos].host_index == index
      	 	&& strcmp(fmname, fmpks[fmpks_pos].host_name) == 0)
      	    || (fmpks[fmpks_pos].guest_index == index
      	    	&& strcmp(fmname, fmpks[fmpks_pos].guest_name) == 0))
      	 {
      	 	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
      	 		WINDOW_BUTTONTYPE_OK,
      	 		-1, -1,
      	 		makeEscapeString( "\n����Ŀǰ����ս���У������޷���˳�Ա��", buf, sizeof(buf)));
//      	 	print("fmpks_pos:%d index:%d host:%d guest:%d\n", fmpks_pos,
//      	 		index, fmpks[fmpks_pos].host_index, fmpks[fmpks_pos].guest_index);
      	 	return;
      	 }
      }
#ifdef _FMVER21
	  {	  
	  // shan begin
      char sbuf[1024];	  
	  sprintf( sbuf, "�峤����:%d -> ��������:%s ��������:%d (���������˳�����)\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	  LogFamily(
		  CHAR_getChar(meindex, CHAR_FMNAME),
		  CHAR_getInt(meindex, CHAR_FMINDEX),
		  CHAR_getChar(meindex, CHAR_NAME),
		  CHAR_getChar(meindex, CHAR_CDKEY),
		  "CheckMember",
		  sbuf
		  );
	  // shan end      
	  SaacClient__ACMemberLeaveFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	  }
#else
	  {
      SaacClient__ACMemberLeaveFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   		CONNECT_getFdid(fd));
	  }
#endif
   }
#ifdef _FMVER21
// else if (result == FMMEMBER_ELDER || result == FMMEMBER_INVITE
//   	|| result == FMMEMBER_BAILEE || result == FMMEMBER_VICELEADER )
   else if (result == FMMEMBER_ELDER )
   {
      if (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
      		return;
#ifdef _FMVER21
	  {
	  // shan begin
      char sbuf[1024];
	  sprintf( sbuf, "�峤����:%d -> ��������:%s ��������:%d (�������Ϊ����)\n", CHAR_getInt(meindex, CHAR_FMLEADERFLAG), charname, charindex);
	  LogFamily(
		  CHAR_getChar(meindex, CHAR_FMNAME),
		  CHAR_getInt(meindex, CHAR_FMINDEX),
		  CHAR_getChar(meindex, CHAR_NAME),
		  CHAR_getChar(meindex, CHAR_CDKEY),
		  "CheckMember",
		  sbuf
		  );
	  // shan end      
   	SaacClient__ACMemberJoinFM_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	  }
#else
   {
   	SaacClient__ACMemberJoinFM_send(acfd,
      		CHAR_getChar(meindex, CHAR_FMNAME),
      	 	CHAR_getInt(meindex, CHAR_FMINDEX), charname, charindex,
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), result,
   		CONNECT_getFdid(fd));
	  }
#endif
   }
#endif
   // Ҫ�����¼����б�
   SaacClient__ACShowFMList_send( acfd );
}

void FAMILY_Channel(int fd, int meindex, char *message)
{
	char token[128], token2[128];
	char buf[4096], subbuf[4096], sendbuf[4096];
	int i, tempindex, fmindexi, channel, nowchannel, num;
	
	fmindexi = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
	nowchannel = CHAR_getWorkInt( meindex, CHAR_WORKFMCHANNEL );
	
	//   print(" channelFM:%d ", fmindexi);
	if( fmindexi < 0 ) {
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n�㻹û�м����κμ��壡", buf, sizeof(buf)));
		return;
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token,
		sizeof(token)) == FALSE)	return;
	 
	if (getStringFromIndexWithDelim(message, "|", 3, token2,
		sizeof(token2)) == FALSE)        return;
	 
	channel = atoi( token2 );
	 
	if( strcmp( token, "J") == 0) {
		if( channel < -1 || channel > FAMILY_MAXCHANNEL )return;
		if( nowchannel >= 0 && nowchannel < FAMILY_MAXCHANNEL ) {
			i = 0;
			while( i < FAMILY_MAXMEMBER ) {
				if( channelMember[fmindexi][nowchannel][i] == meindex ) {
					channelMember[fmindexi][nowchannel][i] = -1;
					break;
				}
				i++;
			}
		}
		
		if( channel > 0 && channel < FAMILY_MAXCHANNEL ) {
			i = 0;
			while( i < FAMILY_MAXCHANNELMEMBER ) {
				if( channelMember[fmindexi][channel][i] < 0 ) {
					channelMember[fmindexi][channel][i] = meindex;
					break;
				}
				i++;
			}
			if( i >= FAMILY_MAXCHANNELMEMBER ) {
				CHAR_talkToCli( meindex, -1, "��Ƶ������������", CHAR_COLORWHITE);
				return;
			}
			sprintf( buf, "�������Ƶ�� [%d]��", channel );
			CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
			
			if( nowchannel >=0 && nowchannel < FAMILY_MAXCHANNEL ) {
				sprintf( buf, "%s �˳�Ƶ����", CHAR_getChar( meindex, CHAR_NAME) );
				for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
					if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
						&& channelMember[fmindexi][nowchannel][i] != meindex ) {
						CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
					}
				}
			}
			sprintf( buf, "%s ����Ƶ����", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][channel][i])
					&& channelMember[fmindexi][channel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][channel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
			
		}
		else if( channel == 0 ) {
			i = 0;
			while( i < FAMILY_MAXMEMBER ) {
				if( channelMember[fmindexi][0][i] < 0 ) {
					channelMember[fmindexi][0][i] = meindex;
					break;
				}
				i++;
			}
			if( i >= FAMILY_MAXMEMBER ) {
#ifndef _CHANNEL_MODIFY
				CHAR_talkToCli( meindex, -1, "��Ƶ������������", CHAR_COLORWHITE);
#endif
				return;
			}
#ifndef _CHANNEL_MODIFY
			sprintf( buf, "�������Ƶ�� [ȫ]��");
			CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
			if( nowchannel >=0 && nowchannel < FAMILY_MAXCHANNEL ) {
				sprintf( buf, "%s �˳�Ƶ����", CHAR_getChar( meindex, CHAR_NAME) );
				for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
					if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
						&& channelMember[fmindexi][nowchannel][i] != meindex ) {
						CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
					}
				}
			}
			sprintf( buf, "%s ����Ƶ����", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][channel][i])
					&& channelMember[fmindexi][channel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][channel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
#endif
		}
#ifdef _FMVER21
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == FMMEMBER_LEADER )
#else
		else if( channel == FAMILY_MAXCHANNEL && CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) == 1 )
#endif
		{
			CHAR_talkToCli( meindex, -1, "�����峤�㲥��", CHAR_COLORWHITE);
		}
		else {
			channel = -1;
#ifndef _CHANNEL_MODIFY
			CHAR_talkToCli( meindex, -1, "�˳�����Ƶ����", CHAR_COLORWHITE);
#else
			CHAR_talkToCli( meindex, -1, "�ر��峤�㲥��", CHAR_COLORWHITE);
			channel = 0;
			i = 0;
			while(i < FAMILY_MAXMEMBER){
				if( channelMember[fmindexi][0][i] < 0 ) {
					channelMember[fmindexi][0][i] = meindex;
					break;
				}
				i++;
			}
#endif
			
			sprintf( buf, "%s �˳�Ƶ����", CHAR_getChar( meindex, CHAR_NAME) );
			for( i=0; i < FAMILY_MAXCHANNELMEMBER; i++ ) {
				if( CHAR_CHECKINDEX(channelMember[fmindexi][nowchannel][i])
					&& channelMember[fmindexi][nowchannel][i] != meindex ) {
					CHAR_talkToCli( channelMember[fmindexi][nowchannel][i], -1, buf, CHAR_COLORWHITE);
				}
			}
		}
		
		CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNEL, channel);
		if( channel != -1 ) CHAR_setWorkInt( meindex, CHAR_WORKFMCHANNELQUICK, channel);
		
		sprintf( sendbuf, "C|J|%d", channel);
		GmsvServer_FM_send( fd, sendbuf);
		
	}
	else if( strcmp( token, "L") == 0) {
		
		int j, membernum, bFind = 0;
		if( channel < 0 || channel >= FAMILY_MAXCHANNEL ) return;
		
		subbuf[0] = '\0';
		num = 0;
		if( channel != 0 )
			membernum = FAMILY_MAXCHANNELMEMBER;
		else
			membernum = FAMILY_MAXMEMBER;
		
		for( j = 0 ; j < FAMILY_MAXMEMBER ; j++ ) {
			bFind = 0;
			tempindex = familyMemberIndex[fmindexi][j];
			//if( tempindex >= 0 ) {
			if( CHAR_CHECKINDEX(tempindex) ) {
				for( i=0; i< membernum ; i++) {
					if( tempindex == channelMember[fmindexi][channel][i] ) {
						//if( CHAR_getChar( tempindex, CHAR_NAME ) == NULL ) {
						//	familyMemberIndex[fmindexi][j] = -1;
						//	channelMember[fmindexi][channel][i] = -1;
						//	continue;
						//}
						bFind = 1;
						break;
					}
				}
				if( bFind )
					strcat( subbuf, "|1|" );
				else
					strcat( subbuf, "|0|" );
				makeEscapeString( CHAR_getChar( tempindex, CHAR_NAME ), buf, sizeof(buf));
				strcat( subbuf, buf );
				num++;
			}
		}
		sprintf( sendbuf, "C|L|%d|%d%s", channel, num, subbuf);
		GmsvServer_FM_send( fd, sendbuf);
		//print(" CList:%s ", sendbuf);
	}
}

void FAMILY_Bank(int fd, int meindex, char *message)
{
	char token[128], token2[128], buf[1024];
	int fmindex, cash, bank, toBank;
	int MaxGold;
	MaxGold = CHAR_getMaxHaveGold( meindex);
	fmindex = CHAR_getInt( meindex, CHAR_FMINDEX);
	// add shan
	if( fmindex <= 0 && CHAR_getInt( meindex, CHAR_BANKGOLD) < 1) {
		CHAR_talkToCli( meindex, -1, "������ȼ�����塣", CHAR_COLORWHITE);
		return;
	}
	
	if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)
	   return;
	
	if( strcmp(token, "G" )==0 )	{
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token)) == FALSE)
			return;
		
		toBank = atoi( token2 );
		cash = CHAR_getInt( meindex, CHAR_GOLD);
		bank = CHAR_getInt( meindex, CHAR_BANKGOLD);
		if( ((cash - toBank) >= 0) && ((cash - toBank) <= MaxGold )
			&& ((((bank + toBank) >= 0) && ((bank + toBank) <= CHAR_MAXBANKGOLDHAVE))
			   	|| toBank < 0) ) {
			// shan add       
			if( toBank > 0 && CHAR_getInt( meindex, CHAR_FMINDEX ) < 1 ) {
				sprintf(buf, "��Ǹ����û�м����κμ��壬���Խ�����ȡ���");
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				return;
			}
			if( bank + toBank <0) {
				sprintf(buf, "��Ǹ����ļ������и����ʻ�û����ô����");
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				return;
			}
			CHAR_setInt( meindex, CHAR_GOLD, cash - toBank);
			CHAR_setInt( meindex, CHAR_BANKGOLD, bank + toBank);
			CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
			
			if( toBank >= 0 ) {
				sprintf(buf, "����%d���������и����ʻ���", toBank);
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				// Syu ADD ��������������д�ȡLog (������������)
				LogFamilyBankStone(
					CHAR_getChar( meindex, CHAR_NAME ), 
					CHAR_getChar( meindex, CHAR_CDKEY ),
					toBank,                            
					CHAR_getInt( meindex, CHAR_GOLD ),
					"myBank(���)(�����������)",
					CHAR_getInt( meindex,CHAR_FLOOR),
					CHAR_getInt( meindex,CHAR_X ),
					CHAR_getInt( meindex,CHAR_Y ),
					CHAR_getInt( meindex,CHAR_BANKGOLD)
					);
				
			}
			else {
				sprintf(buf, "�Ӽ������и����ʻ�ȡ��%d��", -toBank);
				CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
				// Syu ADD ��������������д�ȡLog (������������)
				LogFamilyBankStone(
					CHAR_getChar( meindex, CHAR_NAME ), 
					CHAR_getChar( meindex, CHAR_CDKEY ),
					toBank,                            
					CHAR_getInt( meindex, CHAR_GOLD ),
					"myBank(���)(�����������)",
					CHAR_getInt( meindex,CHAR_FLOOR),
					CHAR_getInt( meindex,CHAR_X ),
					CHAR_getInt( meindex,CHAR_Y ),
					CHAR_getInt( meindex,CHAR_BANKGOLD)
					);
				
			}
			// Syu ADD ��������������д�ȡLog (������������)
			LogStone(
				-1,
				CHAR_getChar( meindex, CHAR_NAME ), /* ƽ�ҷ�   */
				CHAR_getChar( meindex, CHAR_CDKEY ), /* �����ǡ�ID */
				-toBank,                                 /* �ź� */
				CHAR_getInt( meindex, CHAR_GOLD ),
				"myBank(�����������)",
				CHAR_getInt( meindex,CHAR_FLOOR),
				CHAR_getInt( meindex,CHAR_X ),
				CHAR_getInt( meindex,CHAR_Y )
				);
			
		}
/*
		else
			print(" bank_error ");
*/		
	}
	if( strcmp(token, "I" )==0 ) {
		
	}
	if( strcmp(token, "T" )==0 ) {
		int toTax;
		int mygold;
		int FMindex;
		
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token)) == FALSE)
			return;
		toTax = atoi( token2 );
		
#ifdef _FMVER21
		if( CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_LEADER &&
			CHAR_getInt( meindex, CHAR_FMLEADERFLAG ) != FMMEMBER_ELDER && toTax < 0 )
			return;
#endif
		
		
		FMindex = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI );
		mygold = CHAR_getInt( meindex, CHAR_GOLD);
		if( mygold < 0 || mygold > MaxGold || toTax == 0 )	return;
		if( toTax > 0 )	{//+���
			if( ((mygold-toTax) < 0) || (familyTax[ FMindex] + toTax) > CHAR_MAXFMBANKGOLDHAVE )	{
				return;
			}
		}else if( toTax < 0 ){ //-ȡ��
			if( ((mygold-toTax)>MaxGold) || (familyTax[ FMindex] + toTax) < 0 )	{
				return;
			}
		}
		
		
		if( toTax>0 ) {	//���Ԥ�ȿۿ�
			CHAR_setInt( meindex, CHAR_GOLD, CHAR_getInt( meindex, CHAR_GOLD)-toTax );
		}
		sprintf( buf, "��������%s������....", (toTax>0)?"���":"ȡ��");
		CHAR_talkToCli( meindex , -1, buf, CHAR_COLORYELLOW);
		
		CHAR_send_P_StatusString( meindex , CHAR_P_STRING_GOLD);
		sprintf( buf, "%d", toTax );
		
		SaacClient__ACFixFMData_send(acfd,
			CHAR_getChar(meindex, CHAR_FMNAME),
			CHAR_getInt(meindex, CHAR_FMINDEX),
			CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMGOLD, buf,
			"", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
		//print(" getTax:%s=%d ", CHAR_getChar(meindex, CHAR_FMNAME), toTax );
	}
}

void ACFMPointList(int ret, char *data)
{
}

void FAMILY_SetPoint(int fd, int meindex, char *message)
{
   int i, fmpointindex, fl, x, y, fmdp, fmlevel = 0;
   char token[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE)
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif
   {
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�㲻���峤������û���޸ĵ�Ȩ��ࡣ�", buf, sizeof(buf)));
      	return;
   }
   fmdp = CHAR_getWorkInt(meindex, CHAR_WORKFMDP);
#ifdef _FM_POINT_APPLY_FAME
  if(fmdp < 100000){
   	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,WINDOW_BUTTONTYPE_OK,-1, -1,
		makeEscapeString( "\n��������������1000�������޷�����ׯ԰��", buf, sizeof(buf)));
    return;
  }
#endif
   for (i = 1; i <= MANORNUM; i++) {
   	if (((strcmp(fmpks[i * MAX_SCHEDULE + 1].guest_name,
   		CHAR_getChar(meindex, CHAR_FMNAME)) == 0))
   	   && ((fmpks[i * MAX_SCHEDULE + 1].flag = FMPKS_FLAG_MANOR_BATTLEBEGIN)
   	   	|| (fmpks[i * MAX_SCHEDULE + 1].flag == FMPKS_FLAG_MANOR_PREPARE))){
   		char	tmpbuf[256];
   		sprintf(tmpbuf, "\n���%s�Ѿ���ԤԼׯ԰�������ˡ�\n�޷�������ׯ԰��ร�",
   			fmpks[i * MAX_SCHEDULE + 1].host_name);
   		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
   			WINDOW_BUTTONTYPE_OK,
   			-1, -1,
   			makeEscapeString(tmpbuf, buf, sizeof(buf)));
   	   	return;
   	}
   }
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   fmpointindex = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 3, token,
   	sizeof(token)) == FALSE)	return;
   fl = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 4, token,
   	sizeof(token)) == FALSE)	return;
   x = atoi(token);
   if (getStringFromIndexWithDelim(message, "|", 5, token,
   	sizeof(token)) == FALSE)	return;
   y = atoi(token);
/*
   print("SetFMPoint charname:%s fmindex:%d index:%d pointindex:%d fl:%d x:%d y:%d\n",
   	CHAR_getChar(meindex, CHAR_FMNAME),
   	CHAR_getInt(meindex, CHAR_FMINDEX),
   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   	fmpointindex, fl, x, y);
*/
   SaacClient__ACSetFMPoint_send(acfd,
   	CHAR_getChar(meindex, CHAR_FMNAME),
   	CHAR_getInt(meindex, CHAR_FMINDEX),
   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI),
   	fmpointindex, fl, x, y, CONNECT_getFdid(fd));
}

void ACSetFMPoint(int ret, int r, int clifd)
{
   int meindex = CONNECT_getCharaindex(clifd);
   char message[256], buf[512];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;
   if (ret == 0){
   		if (r == -1)
   			sprintf(message, "�������ʧ�ܣ�");
   		else if (r == -2)
   			sprintf(message, "���Ѿ��м���ݵ��ˡ������ظ����룡");
   		else if (r == -3)
   			sprintf(message, "��δ�����������ݵ���ʸ�");
   		else if (r == -4)
   			sprintf(message, "����ݵ��Ѿ��м�����ʹ�����ޣ�");
   		else if (r == -5)
   			sprintf(message, "���ļ�������δ�������׼ࡣ�");
   		else if (r == -6)
   			sprintf(message, "������ʧ��ׯ԰��������ʮ���Ӻ�����Ӵ��");
   }
   else if (ret == 1)
			sprintf(message, "�������ݵ�ϣˣ�");
   
   GmsvServer_WN_send( clifd, WINDOW_MESSAGETYPE_MESSAGE,
   	WINDOW_BUTTONTYPE_OK,
   	-1, -1,
   	makeEscapeString(message, buf, sizeof(buf)));
}

void ACFMAnnounce(int ret, char *fmname, int fmindex, int index,
	int kindflag, char *data, int color)
{
   // kindflag 1:�峤�㲥 2:ϵͳ������屻ɾ�� 3:ϵͳ֪ͨѶϢ
   int i, chindex;
   if( ret != 1 )	return;
//   print("fmname:%s fmindex:%d index:%d kindflag:%d data:%s color:%d\n",
//   	fmname, fmindex, index, kindflag, data, color);
   for( i=0; i < FAMILY_MAXMEMBER; i++)
   {
      chindex = familyMemberIndex[index][i];
      if( chindex >= 0 ) {
         if( CHAR_getCharUse(chindex) )
         {
            if (kindflag == 1)
            {
#ifdef _FMVER21            
                // shan 2001/12/13
				//if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_MEMBER )
                if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_MEMBER ||
					CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == FMMEMBER_ELDER )
#else
               if( CHAR_getInt( chindex, CHAR_FMLEADERFLAG ) == 2 )
#endif               
                  CHAR_talkToCli( chindex, -1, data, color );
            }
            else if (kindflag == 2)
            {
               int fd = getfdFromCharaIndex( chindex );
               if (fd == -1)	return;
               SetFMVarInit( chindex );
               CHAR_talkToCli( chindex , -1, "������ļ���������֮��û�����յ������������Ա�����Ա��Ƚ�ɢ�ˣ�",
               		CHAR_COLORRED);
            }
         }else
            familyMemberIndex[index][i] = -1;
      }
   }
   if (kindflag == 3)
   {
      int meindex = 0;
      int clifd = getfdFromFdid(color);
      if (CONNECT_checkfd(clifd) == FALSE)	return;
      meindex = CONNECT_getCharaindex(clifd);
      if (!CHAR_CHECKINDEX(meindex))	return;
      CHAR_talkToCli(meindex, -1, data, CHAR_COLORRED);
      if(strstr(data,"�Ѿ����������ļ������룡")!=NULL){

			}else if(strstr(data,"�Ѿ������߳������ˣ�")!=NULL){
				CHAR_setInt( meindex , CHAR_RIDEPET, -1 );
				CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , CHAR_getInt( meindex , CHAR_BASEBASEIMAGENUMBER) );
				CHAR_complianceParameter( meindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( meindex , CHAR_P_STRING_RIDEPET);
#ifdef _FM_METAMO
				CHAR_ReMetamo(meindex);
#endif
			}
   }
   if (kindflag == 4)
   {
      int meindex = 0;
      char buf[1024];
      int clifd = getfdFromFdid(color);
//      print("Here1\n");
      if (CONNECT_checkfd(clifd) == FALSE)	return;
      meindex = CONNECT_getCharaindex(clifd);
      if (!CHAR_CHECKINDEX(meindex))	return;
//      print("Here2\n");
      GmsvServer_WN_send(clifd, WINDOW_MESSAGETYPE_MESSAGE,
      		WINDOW_BUTTONTYPE_OK,
      		-1, -1,
      		makeEscapeString(data, buf, sizeof(buf)));
   }
}

void FAMILY_SetAcceptFlag(int fd, int meindex, char *message)
{	
   int result;
   char token[128], buf[1024];
   
   if (!CHAR_CHECKINDEX(meindex))	return;
   
   if ((CHAR_getWorkInt(meindex, CHAR_WORKPARTYMODE) != CHAR_PARTY_NONE)
      || (CHAR_getWorkInt(meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE))
         return;
   
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
//    || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
//       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)
//       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)))
    || ((CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
       && (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER)))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�㲻���峤������û���޸ĵ�Ȩ��ࡣ�", buf, sizeof(buf)));
      	return;
   }
   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   result = atoi(token);
   if ((result == 0) || (result == 1))
   {
   	SaacClient__ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
   		CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_ACCEPTFLAG,
   		token, "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
   }
}

void FAMILY_FixRule( int fd, int meindex, char* message )
{
   char token[1024], buf[1024];
      
   if (!CHAR_CHECKINDEX(meindex))return;

   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
#ifdef _FMVER21      
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER))
#else
      || (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1))
#endif      
   {
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n�㲻���峤������û���޸ĵ�Ȩ��ࡣ�", buf, sizeof(buf)));
      	return;
   }

   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   
   if( strcmp( token, "R") == 0 )
   {
	if (getStringFromIndexWithDelim(message, "|", 3, buf,
		sizeof( buf ) ) == FALSE)return;
 
	   if (strcmp( buf, "") == 0)
	   {
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			makeEscapeString( "\n������ּ����Ϊ�հ�ࡣ�", buf, sizeof(buf)));
	      	return;
	   }

//	   print(" new_rule:%s ", buf);
	   SaacClient__ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
	   	CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMRULE,
   		buf, "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	   return;
   }


#ifdef _FAMILYBADGE_
   if( strcmp( token, "B") == 0 )
   {
	   if (getStringFromIndexWithDelim(message, "|", 3, buf,
		   sizeof(buf)) == FALSE) {
			   GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				   WINDOW_BUTTONTYPE_OK,
				   -1, -1,
				   makeEscapeString( "\n����������ô���", buf, sizeof(buf)));
			   return;
	   }
	   int badgeindex = atoi( buf );
	   extern int FamilyBadgeData[];
	   extern int FamilyBadgeDataNum;
	   extern int FamilyBadgePrice;
	   if(badgeindex <0 || badgeindex>FamilyBadgeDataNum){
		   GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			   WINDOW_BUTTONTYPE_OK,
			   -1, -1,
			   makeEscapeString( "\n����������ô���", buf, sizeof(buf)));
		   return;
	   }
	   int vippoint = sasql_getVipPoint(meindex);
	   if(vippoint < FamilyBadgePrice){
		   GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
			   WINDOW_BUTTONTYPE_OK,
			   -1, -1,
			   makeEscapeString( "\n���Ľ�Ҳ��㣡", buf, sizeof(buf)));
		   return;
	   }
	   sasql_setVipPoint(meindex,vippoint - FamilyBadgePrice);
	   char token[256];
	   sprintf(token, "ϵͳ�۳�%d���",FamilyBadgePrice);
	   CHAR_talkToCli( meindex, -1, token, CHAR_COLORYELLOW);
	   badgeindex = FamilyBadgeData[badgeindex];
	   sprintf(buf,"%d",badgeindex);
	   SaacClient__ACFixFMData_send(acfd,
		   CHAR_getChar(meindex, CHAR_FMNAME),
		   CHAR_getInt(meindex, CHAR_FMINDEX),
		   CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_BADGE,
		   buf, "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
		   CONNECT_getFdid(fd));
	   SaacClient__ACShowMemberList_send( acfd, CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI));
	   return;
   }
#endif
   if( strcmp( token, "P") == 0 )
   {
	   int havepetindex, petindex, i;
	   char petname[20], petattr[512];

	   // ����Ƿ������ػ���
	   for( i =0; i< CHAR_MAXPETHAVE; i++ )
	   {
	   	int petindex = CHAR_getCharPet(meindex, i);
	   	if (!CHAR_CHECKINDEX(petindex))     continue;
	   	if( CHAR_getInt( petindex , CHAR_PETFAMILY ) ==1 )
	   	{
//			GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
//				WINDOW_BUTTONTYPE_OK,
//				-1, -1,
//				makeEscapeString( "\nԭ�����ػ��޻���ࡡ�", buf, sizeof(buf)));
//		      	return;
				CHAR_setInt( petindex , CHAR_PETFAMILY,0 );
	   	}
	   }
	   // ����Ƿ������ػ���(�ĳ�)
	   for( i =0; i< CHAR_MAXPOOLPETHAVE; i++ )
	   {
	   	int petindex = CHAR_getCharPoolPet(meindex, i);
	   	if (!CHAR_CHECKINDEX(petindex))     continue;
	   	if( CHAR_getInt( petindex , CHAR_PETFAMILY ) ==1 )
	   	{
//			GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
//				WINDOW_BUTTONTYPE_OK,
//				-1, -1,
//				makeEscapeString( "\nԭ�����ػ��޻���ࡡ�", buf, sizeof(buf)));
//		      	return;
				CHAR_setInt( petindex , CHAR_PETFAMILY,0 );
	   	}
	   }

	   if (getStringFromIndexWithDelim(message, "|", 3, buf,
		sizeof( buf ) ) == FALSE) return;
	   havepetindex = atoi( buf );
	   
	   petindex = CHAR_getCharPet(meindex, havepetindex);
	   
	   if (!CHAR_CHECKINDEX(petindex))	return;

 	   if (strlen(CHAR_getChar(petindex, CHAR_USERPETNAME)) == 0)
	   	sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_NAME));
	   else
   		sprintf(petname, "%s", CHAR_getChar(petindex, CHAR_USERPETNAME));
	   sprintf(petattr, "%d %d %d %d",
   		CHAR_getInt(petindex, CHAR_BASEIMAGENUMBER),
	   	CHAR_getWorkInt(petindex, CHAR_WORKATTACKPOWER),
   		CHAR_getWorkInt(petindex, CHAR_WORKDEFENCEPOWER),
	   	CHAR_getWorkInt(petindex, CHAR_WORKQUICK));
	   	
	   CHAR_setInt(petindex, CHAR_PETFAMILY, 1);
	   SaacClient__ACFixFMData_send(acfd,
   		CHAR_getChar(meindex, CHAR_FMNAME),
	   	CHAR_getInt(meindex, CHAR_FMINDEX),
   		CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMPET,
   		petname, petattr, CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX),
   		CONNECT_getFdid(fd));
	   return;
	   
   }
   
}

void JoinMemberIndex( int meindex, int fmindexi )
{
	int i;
	
	for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
		if( familyMemberIndex[fmindexi][i] == meindex ) familyMemberIndex[fmindexi][i] = -1;
	}
	
	for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
		if( familyMemberIndex[fmindexi][i] < 0 ){
			familyMemberIndex[fmindexi][i] = meindex;
			break;
		}
	}
#ifdef _CHANNEL_MODIFY
	i = 0;
	// ������ɵ�Ƶ����¼
	while(i < FAMILY_MAXMEMBER){
	 if(channelMember[fmindexi][0][i] == meindex){
		 channelMember[fmindexi][0][i] = -1;
	 }
	 i++;
	}
	i = 0;
	// ����Ƶ��
	while(i < FAMILY_MAXMEMBER){
	 if(channelMember[fmindexi][0][i] == -1){
		 channelMember[fmindexi][0][i] = meindex;
		 CHAR_setWorkInt(meindex,CHAR_WORKFMCHANNEL,0);
		 break;
	 }
	 i++;
	}
#endif
}

void LeaveMemberIndex( int meindex, int fmindexi )
{
	int i;
   
  for( i = 0 ; i < FAMILY_MAXMEMBER; i++){
    if( familyMemberIndex[fmindexi][i] == meindex ) familyMemberIndex[fmindexi][i] = -1;
  }
#ifdef _CHANNEL_MODIFY
	i = 0;
	// ����ɵ�Ƶ����¼
	while(i < FAMILY_MAXMEMBER){
	 if(channelMember[fmindexi][0][i] == meindex){
		 channelMember[fmindexi][0][i] = -1;
	 }
	 i++;
	}
#endif
}

int FAMILY_RidePet( int fd, int meindex, char* message )
{
	char token[64], token2[64];
	int petindex, rideGraNo = 0, leaderimageNo;
	// Arminius 8.25 recover
	int i;
#ifndef _NEW_RIDEPETS
	int big4fm = 0;
#endif
	if (!CHAR_CHECKINDEX(meindex))return 0;

	// Robin fix ս���в�����
	if( CHAR_getWorkInt( meindex, CHAR_WORKBATTLEMODE) != BATTLE_CHARMODE_NONE )
	{
		CHAR_talkToCli( meindex, -1, "ս���в�����裡", CHAR_COLORYELLOW );
		return 0;
	}
	// Robin fix �����в�����
	if( CHAR_getWorkInt(meindex, CHAR_WORKTRADEMODE) != CHAR_TRADE_FREE){
		CHAR_talkToCli( meindex, -1, "�����в�����裡", CHAR_COLORYELLOW );
		return 0;
	}

#ifdef _PETSKILL_BECOMEPIG
    if( CHAR_getInt( meindex, CHAR_BECOMEPIG) > -1 ){ //���������״̬
	    CHAR_setInt( meindex, CHAR_RIDEPET, -1 );
		//����ѡ���״̬��ȻΪ"���",������������
		CHAR_complianceParameter( meindex );
		CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET);
		CHAR_talkToCli( meindex, -1, "Ŀǰ�㴦��������״̬��������˳��", CHAR_COLORYELLOW );
		return 0;
	}
#endif
	if (getStringFromIndexWithDelim(message, "|", 2, token, sizeof(token)) == FALSE)	return 0;
	if( strcmp( token, "P") == 0) {
		if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token2)) == FALSE)
			return 0;
		
		if( atoi(token2) != -1 ) {
			petindex = CHAR_getCharPet( meindex, atoi( token2 ) );
			if(!CHAR_CHECKINDEX(petindex))return 0;
			//ȡ����ֹ����ս
			//if( CHAR_getInt( meindex, CHAR_DEFAULTPET ) == atoi( token2 ) )	return 0;
			if( CHAR_getInt( meindex, CHAR_RIDEPET) != -1 ) return 0;
			if( CHAR_getInt( meindex, CHAR_LEARNRIDE) < CHAR_getInt( petindex, CHAR_LV )  )
			{
				char buff[255];
				sprintf(buff,"��Ŀǰֻ����˵ȼ�С��%d���ĳ衣",CHAR_getInt( meindex, CHAR_LEARNRIDE));
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return 0;
			}
			if( CHAR_getWorkInt( petindex, CHAR_WORKFIXAI ) < 100 )
			{
				CHAR_talkToCli( meindex, -1, "��������С��100", CHAR_COLORYELLOW );
				return 0;
			}
	
#ifdef _PET_BEATITUDE
			if( CHAR_getInt(petindex, CHAR_BEATITUDE) > 0){
				CHAR_talkToCli( meindex, -1, "�������ĳ����޷���裡", CHAR_COLORYELLOW );
				return 0;
			}
#endif

#ifdef _PET_VALIDITY
			if(CHAR_getInt ( petindex, CHAR_PETVALIDITY) > 0 && CHAR_getInt ( petindex, CHAR_PETVALIDITY) < time(NULL) ){
				CHAR_talkToCli(meindex, -1, "�ó����Ѿ�ʧЧ�ˣ�", CHAR_COLORYELLOW);
				int s_pet = CHAR_getWorkInt( meindex, CHAR_WORKSTANDBYPET);
				s_pet ^= ( 1 << atoi( token2 ) );
				CHAR_setWorkInt( meindex, CHAR_WORKSTANDBYPET, s_pet);

				GmsvServer_SPET_send( fd, s_pet, TRUE);
				return FALSE;
			}
#endif
			
#ifdef _RIDELEVEL
			if( CHAR_getInt( meindex, CHAR_LV)+getRideLevel() < CHAR_getInt( petindex, CHAR_LV )  )
			{ 
				char buff[255];
				sprintf(buff,"�����ֻ�����ȼ������%d���ĳ衣",getRideLevel());
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return 0;
			}	
#else
			if( CHAR_getInt( meindex, CHAR_LV)+5 < CHAR_getInt( petindex, CHAR_LV )  )
			{ 
				char buff[255];
				sprintf(buff,"�����ֻ�����ȼ������5���ĳ衣");
				CHAR_talkToCli( meindex, -1, buff, CHAR_COLORYELLOW );
				return 0;
			}	
#endif
#ifdef _PET_2TRANS
			if( CHAR_getInt( petindex, CHAR_TRANSMIGRATION) > getRideTrans() ) return 0;
#endif

			rideGraNo = FamilyRideFunction(meindex, petindex, atoi( token2 ));

			if( rideGraNo != 0 ){
#ifdef _ITEM_METAMO
			//	CHAR_setWorkInt( meindex, CHAR_WORKITEMMETAMO, 0);
#endif
				CHAR_setInt( meindex , CHAR_RIDEPET, atoi( token2 ) );
				CHAR_setInt( meindex , CHAR_BASEIMAGENUMBER , rideGraNo );
				CHAR_complianceParameter( meindex );
				CHAR_sendCToArroundCharacter( CHAR_getWorkInt( meindex, CHAR_WORKOBJINDEX ));
				CHAR_send_P_StatusString( meindex, CHAR_P_STRING_RIDEPET );
				return 1;
			}
		}else {
		}
	}
	return 0;
}

void ACFixFMPK(int winindex, int loseindex, int data)
{
   int i = 0, charindex = 0;
   char msg1[256], msg2[256];
   
   sprintf(msg1, "��ϲ�����������������%8d�㣡", (data / 100));
   sprintf(msg2, "��������������%8d�㣡", (data / 100));
   for (i = 0; i < FAMILY_MAXMEMBER; i++)
   {
      charindex = familyMemberIndex[winindex][i];
      if( charindex >= 0 ) {
         if( CHAR_getCharUse(charindex) )
               CHAR_talkToCli( charindex , -1, msg1, CHAR_COLORYELLOW);
         else
            familyMemberIndex[winindex][i] = -1;
      }
      charindex = familyMemberIndex[loseindex][i];
      if( charindex >= 0 ) {
         if( CHAR_getCharUse(charindex) )
               CHAR_talkToCli( charindex , -1, msg2, CHAR_COLORRED);
         else
            familyMemberIndex[loseindex][i] = -1;
      }
   }
}

void getNewFMList()
{
	SaacClient__ACShowFMList_send( acfd );
}

//int     channelMember[FAMILY_MAXNUM][FAMILY_MAXCHANNEL][FAMILY_MAXMEMBER];
//int     familyMemberIndex[FAMILY_MAXNUM][FAMILY_MAXMEMBER];

void checkFamilyIndex( void )
{
	int i, j, k, char_index, err1=0, err2=0;
	for( i=0; i<FAMILY_MAXNUM; i++){
		for( j=0; j<FAMILY_MAXMEMBER; j++){
			char_index = familyMemberIndex[i][j];
			if( char_index == -1 )	continue;
			if( !CHAR_CHECKINDEX(char_index) ){
				familyMemberIndex[i][j] = -1;
				err1++;
				continue;
			}
			if( CHAR_getWorkInt( char_index, CHAR_WORKFMINDEXI ) != i )
			{
				familyMemberIndex[i][j] = -1;
				err1++;
				continue;
			}
		}
		
		for( j=0; j<FAMILY_MAXCHANNEL; j++ )
			for( k=0; k<FAMILY_MAXMEMBER; k++)
			{
				char_index = channelMember[i][j][k];
				if( char_index == -1 )  continue;
				if( !CHAR_CHECKINDEX(char_index) )
				{
					channelMember[i][j][k] = -1;
					err2++;
					continue;
				}
				if( CHAR_getWorkInt(char_index, CHAR_WORKFMINDEXI) != i )
				{
					channelMember[i][j][k] = -1;
					err2++;
					continue;
				}
			}
	}
}

void FAMILY_LeaderFunc( int fd, int meindex, char *message )
{
   char token[1024], token2[1024], buf[1024];

   if (!CHAR_CHECKINDEX(meindex))return;
   if ((CHAR_getInt(meindex, CHAR_FMINDEX) == -1)
      || (strcmp(CHAR_getChar(meindex, CHAR_FMNAME), "") == 0)
      //|| (CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)
   )
   {
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "�㻹δ������壬���Բ���ʹ��ࡣ�", buf, sizeof(buf) ));
      	return;
   }
   
   if (getStringFromIndexWithDelim(message, "|", 2, token,
   	sizeof(token)) == FALSE)	return;
   if( strcmp( token, "F") == 0 ){
       int  fmindex_wk;
       char sendbuf[1024],buf[1024];
       fmindex_wk = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI);
#ifdef _FMVER21       
//     if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
//         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER &&
//         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER )  return;
     if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
         CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER )  return;
#else
       if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif       
       if( CHAR_getInt(meindex, CHAR_FMINDEX) > 0 ){
           if( fmindex_wk < 0 || fmindex_wk >= FAMILY_MAXNUM){
               print("FamilyNumber Data Error!!");
               return;
           }
       }
       SaacClient__ACShowMemberList_send( acfd, fmindex_wk);           
	    
       sprintf( sendbuf, "               ���� �� �� ֪��\n��С�Ĵ�����Ա�����ϣ�һ���޸�����޷��ظ�ԭ̬������С�ġ�");
	    
       GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
	                 WINDOW_BUTTONTYPE_OK,
	                 CHAR_WINDOWTYPE_FM_MESSAGE2,
	                 -1,
	                 makeEscapeString( sendbuf, buf, sizeof(buf)));
   }
   if( strcmp( token, "L") == 0 ){
   	int i, kind, letterNo = 0;
   	char subtoken[256];

#ifdef _FMVER21
//   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
//   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_INVITE &&
//   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_VICELEADER)  return;
   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER &&
   	    CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_ELDER )  return;
#else
   	if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif   	
   	
   	if (getStringFromIndexWithDelim(message, "|", 3, token2, sizeof(token2)) == FALSE)	return;
		kind = atoi( token2 );
		
		for( i=0 ; i<FMPOINTNUM ; i++ ) {
			getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 5, subtoken, sizeof(subtoken));
			if( CHAR_getInt( meindex, CHAR_FMINDEX ) == atoi(subtoken) ) {
				getStringFromIndexWithDelim(fmpointlist.pointlistarray[i], "|", 9, subtoken, sizeof(subtoken));
#ifdef _FAME_REG_TIME
				FreeFameFeatures(meindex, kind, atoi(subtoken));
				return;
#else
				if( kind == 1 ) {
					switch( atoi(subtoken) ) {
					case 1:
					letterNo = 19001;	break;
					case 2:
					letterNo = 19002;	break;
					case 3:
					letterNo = 19003;	break;
					case 4:
					letterNo = 19004;	break;
#ifdef _NEW_RIDEPETS
					case 5:
						letterNo = 20229; break;
					case 6:
						letterNo = 20230; break;
					case 7:
						letterNo = 20231; break;
					case 8:
						letterNo = 20232; break;
					case 9:
						letterNo = 20233; break;
					case 10:
						if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 0)
							letterNo = 20768; 
						else if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 1)
							letterNo = 20769; 
						break;
#endif
					}
				} else
				if( kind == 2 ) {
					if(getRideMode()==2 || getRideMode()==4){
						CHAR_talkToCli( meindex, -1, "���˰ɣ����Ǳ��ɣ�����Ҳû��!", CHAR_COLORYELLOW);
						return;
					}
					switch( atoi(subtoken) ) {
					case 1:
					letterNo = 19005;	break;
					case 2:
					letterNo = 19006;	break;
					case 3:
					letterNo = 19007;	break;
					case 4:
					letterNo = 19008;	break;
#ifdef _NEW_RIDEPETS
					case 5:
						letterNo = 20224; break;
					case 6:
						letterNo = 20225; break;
					case 7:
						letterNo = 20226; break;
					case 8:
						letterNo = 20227; break;
					case 9:
						letterNo = 20228; break;
					case 10:
						if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 0)
							letterNo = 20766; 
						else if( CHAR_getInt( meindex, CHAR_FMSPRITE ) == 1)
							letterNo = 20767; 
						break;
#endif
					}			
				}
#endif  
			}	
		}

		if( letterNo == 0 ) {
			GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString("\nֻ��ӵ��ׯ԰���峤����������ࡣ�", buf, sizeof(buf)));
	      		return;
		}
#ifndef _FAME_REG_TIME
		else {
			int emptyitem_indexinchara = CHAR_findEmptyItemBox( meindex );
			int item_index = ITEM_makeItemAndRegist( letterNo );
			
			if( item_index == -1 )	return;
			if( emptyitem_indexinchara < 0 ) {
				CHAR_talkToCli( meindex, -1, "������������", CHAR_COLORWHITE);
				return;
			}
			
			CHAR_setItemIndex( meindex, emptyitem_indexinchara, item_index );
			ITEM_setWorkInt( item_index, ITEM_WORKOBJINDEX,-1);
			ITEM_setWorkInt( item_index, ITEM_WORKCHARAINDEX, meindex);
			CHAR_sendItemDataOne( meindex, emptyitem_indexinchara);
			snprintf( buf, sizeof( buf), "����%s�ɹ���",ITEM_getChar( item_index, ITEM_NAME));
			CHAR_talkToCli( meindex, -1, buf, CHAR_COLORWHITE);
		}
#endif
  }
   if( strcmp( token, "CHANGE") == 0 ){
   	int fmindexi, j, num=0;
   	char subbuf[2048], sendbuf[2048];
   	
   	if (getStringFromIndexWithDelim(message, "|", 3, token2,
	   sizeof(token2)) == FALSE)	return;
	
	fmindexi = CHAR_getWorkInt( meindex, CHAR_WORKFMINDEXI );

	// Ҫ���峤��ѡ���б�
	if( strcmp( token2, "L") == 0 ){
		char subsub[128];
	
#ifdef _FMVER21
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)  return;
#else
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif
		
		strcpy( subbuf, "");
		for( j = 0 ; j < FAMILY_MAXMEMBER ; j++ ) {
			int tempindex = familyMemberIndex[fmindexi][j];
			
			// CoolFish: 2001/9/22 
			if (!CHAR_CHECKINDEX(tempindex))	continue;
			
			if (CheckLeaderQ(tempindex) >= 0 && tempindex != meindex )
			{
				char	tmpbuf[1024];
				sprintf(tmpbuf, "%s", CHAR_getChar(tempindex, CHAR_NAME));
				makeEscapeString( tmpbuf, buf, sizeof(buf));
				sprintf( subsub, "|%d|%s", j, buf );
				strcat( subbuf, subsub );
				num++;
			}
		}
		sprintf( sendbuf, "L|CHANGE|L|%d%s", num, subbuf );
		//GmsvServer_FM_send( fd, sendbuf);
		
		GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_LEADERSELECT,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			sendbuf );
		
	}
	// ѯ���峤��ѡ���Ƿ�Ը�����
	if( strcmp( token2, "Q") == 0 )	
	{
		char token3[64], token4[64];
		int toindex;

#ifdef _FMVER21		
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER)  return;
#else
		if( CHAR_getInt(meindex, CHAR_FMLEADERFLAG) != 1)  return;
#endif 		
		if (getStringFromIndexWithDelim(message, "|", 4, token3,
			sizeof(token3)) == FALSE)return;
		if (getStringFromIndexWithDelim(message, "|", 5, token4,
			sizeof(token4)) == FALSE)return;
		makeStringFromEscaped( token4 );
		
		if( atoi(token3) < 0 || atoi(token3) > FAMILY_MAXMEMBER ) return;

		toindex = familyMemberIndex[fmindexi][atoi(token3)];
		if( !CHAR_CHECKINDEX( toindex ) )	return;
		if( strcmp( token4, CHAR_getChar( toindex, CHAR_NAME)) != 0 )	return;
		if( CheckLeaderQ(toindex) < 0 )	return;
		
		// ˫����������λʱ��CHAR_WORKLEADERCHANGE��ŶԷ���char_index
		CHAR_setWorkInt( toindex, CHAR_WORKLEADERCHANGE, meindex);
		CHAR_setWorkInt( meindex, CHAR_WORKLEADERCHANGE, toindex);
		
		sprintf( sendbuf, "%s|%d", makeEscapeString( CHAR_getChar( meindex, CHAR_NAME ), buf, sizeof(buf)), meindex );
		
		GmsvServer_WN_send( CHAR_getWorkInt( toindex, CHAR_WORKFD ), WINDOW_MESSAGETYPE_LEADERSELECTA,
			WINDOW_BUTTONTYPE_OK,
			-1, -1,
			sendbuf );

	}
	// ��ѡ�˵Ĵ�
	if( strcmp( token2, "A") == 0 )
	{
		int leaderindex, answerflag;
		char leadername[64], token3[64], token4[64];

//		print( "%s", message );
		
		if (getStringFromIndexWithDelim(message, "|", 4, token3,
			sizeof(token3)) == FALSE) return;
		answerflag = atoi( token3 );
		
		if (getStringFromIndexWithDelim(message, "|", 5, leadername,
			sizeof( leadername )) == FALSE)return;
		makeStringFromEscaped( leadername );
		
		if (getStringFromIndexWithDelim(message, "|", 6, token4,
			sizeof(token4)) == FALSE)return;
		
		//if( atoi(token4) < 0 || atoi(token4) > FAMILY_MAXMEMBER )  return;
		
		leaderindex = atoi( token4 );
		
		// ���˫����CHAR_WORKLEADERCHANGE�Ƿ����
		if( CHAR_getWorkInt( meindex, CHAR_WORKLEADERCHANGE ) != leaderindex )	return;
		if( !CHAR_CHECKINDEX(leaderindex) )	return;
		if( strcmp( leadername, CHAR_getChar( leaderindex, CHAR_NAME) ) != 0 )	return;
		if( CHAR_getWorkInt( leaderindex, CHAR_WORKLEADERCHANGE ) != meindex )  return;
		CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, -1 );
#ifdef _FMVER21		
		if( CHAR_getInt(leaderindex, CHAR_FMLEADERFLAG ) != FMMEMBER_LEADER )  return;
#else
		if( CHAR_getInt(leaderindex, CHAR_FMLEADERFLAG ) != 1 )  return;
#endif 		
		if( CHAR_getInt(meindex, CHAR_FMINDEX) != CHAR_getInt(leaderindex, CHAR_FMINDEX) )  return;

		if( answerflag == 0 )
		{
			CHAR_setWorkInt( meindex, CHAR_WORKLEADERCHANGE, 0);
			CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, 0);
			GmsvServer_WN_send( CHAR_getWorkInt( leaderindex, CHAR_WORKFD) , WINDOW_MESSAGETYPE_MESSAGE,
				WINDOW_BUTTONTYPE_OK,
				-1, -1,
				makeEscapeString( "\n�Բ��𣡶Է���Ը����ܣ�", buf, sizeof(buf)) );
		   	return;
		}
		
		if( answerflag == 1 )
		{
			char	tmpbuf[1024];
			sprintf( buf, "%d", CHAR_getInt( meindex, CHAR_FACEIMAGENUMBER ) );
			// CoolFish: add charname 2001/9/27
			sprintf( tmpbuf, "%s", CHAR_getChar( meindex, CHAR_NAME ) );
			SaacClient__ACFixFMData_send(acfd,
				CHAR_getChar(meindex, CHAR_FMNAME),
	   			CHAR_getInt(meindex, CHAR_FMINDEX),
			   	CHAR_getWorkInt(meindex, CHAR_WORKFMINDEXI), FM_FIX_FMLEADERCHANGE , buf,
			   	tmpbuf, CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
			   	// "", CHAR_getWorkInt(meindex, CHAR_WORKFMCHARINDEX), CONNECT_getFdid(fd));
			return;
		}
	}	
   }
}

void ACFMJob(int fd, int ret, char* data1, char* data2)
{
	const int char_index = CONNECT_getCharaindex(fd);
	if ( !CHAR_CHECKINDEX(char_index) ) return;
	int leaderindex = CHAR_getWorkInt( char_index, CHAR_WORKLEADERCHANGE );
    char buf[256], buf2[256];

    CHAR_setWorkInt( char_index, CHAR_WORKLEADERCHANGE, 0 );
    print("leaderindex:%d:%s\n", leaderindex,CHAR_getChar(leaderindex,CHAR_NAME) );
    
    if( !CHAR_CHECKINDEX(leaderindex) ) return;
    //if( CHAR_getWorkInt( leaderindex, CHAR_WORKLEADERCHANGE ) != char_index ) return;
    CHAR_setWorkInt( leaderindex, CHAR_WORKLEADERCHANGE, 0 );
    
    if( ret == 0 ){
    	CHAR_talkToCli( char_index, -1, "�峤��λʧ�ܣ�", CHAR_COLORYELLOW );
    	CHAR_talkToCli( leaderindex, -1, "�峤��λʧ�ܣ�", CHAR_COLORYELLOW );
	   	return;
	   }
	   
	   // Robin 10/02 debug
    if( CHAR_getInt( leaderindex, CHAR_FMINDEX) != CHAR_getInt( char_index, CHAR_FMINDEX)
#ifdef _FMVER21        	
			// || CHAR_getInt( leaderindex, CHAR_FMLEADERFLAG) != FMMEMBER_LEADER )
#else
			// || CHAR_getInt( leaderindex, CHAR_FMLEADERFLAG) != 1
#endif
	)
	{
		sprintf( buf, "leaderindex:%d:%s\n", leaderindex, CHAR_getChar( leaderindex, CHAR_NAME) );
		LogFamily(
			CHAR_getChar(char_index, CHAR_FMNAME),
			CHAR_getInt(char_index, CHAR_FMINDEX),
			CHAR_getChar(char_index, CHAR_NAME),
			CHAR_getChar(char_index, CHAR_CDKEY),
			"LEADERCHANGE_ERROR(�峤��λʧ��)",
			buf
		);
		return;
	}
	
	//CHAR_setInt( leaderindex, CHAR_FMLEADERFLAG, FMMEMBER_MEMBER);
	//CHAR_setInt( char_index, CHAR_FMLEADERFLAG, FMMEMBER_LEADER);
	SetFMPetVarInit( leaderindex );
	SetFMPetVarInit( char_index );
	CHAR_sendStatusString( leaderindex, "F");
	CHAR_sendStatusString( char_index, "F");
	
	GmsvServer_WN_send( fd, WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( "\n��ϲ�㣡���Ѿ������ε��峤�ˡ�\n��úõ�Ŭ���ɣ�\n���ˡ��ǵ����ȵ��峤�ҵļ������Աѡ��\n�µļ����ػ��ޣ�������彫�ᱻ��ɢࡣ�", buf, sizeof(buf)));
		
	sprintf( buf2, "\n�������ˣ����Ѿ����峤��λ�ӽ���%s�ˡ�", CHAR_getChar( char_index, CHAR_NAME) );
	GmsvServer_WN_send( CHAR_getWorkInt( leaderindex, CHAR_WORKFD) , WINDOW_MESSAGETYPE_MESSAGE,
		WINDOW_BUTTONTYPE_OK,
		-1, -1,
		makeEscapeString( buf2, buf, sizeof(buf)));

	sprintf( buf, "%s\t%s\t%s",
		CHAR_getChar(leaderindex, CHAR_FMNAME),
		CHAR_getChar(leaderindex, CHAR_NAME),
		CHAR_getChar(leaderindex, CHAR_CDKEY)
	);
	
	LogFamily(
		CHAR_getChar(char_index, CHAR_FMNAME),
		CHAR_getInt(char_index, CHAR_FMINDEX),
		CHAR_getChar(char_index, CHAR_NAME),
		CHAR_getChar(char_index, CHAR_CDKEY),
		"LEADERCHANGE(�峤��λ)",
		buf);
}
#ifdef _MO_LNS_CHARSUOXU
int Char_GetFm(int id, int x)
{
	int	fd = getfdFromCharaIndex(id);
	if (x == 1)
		return fmdptop.fmMomentum[id];
	else if (x == 2)
		return fmdptop.fmtopdp[id];
	else if (x == 3)
		return familyTax[CHAR_getWorkInt( id, CHAR_WORKFMINDEXI )];
}

char * FM_getManorData(int manor_id, const int flag)
{
	char *pointbuf = "";
	if (getStringFromIndexWithDelim(fmpointlist.pointlistarray[manor_id], "|", flag, pointbuf, sizeof(pointbuf)) == FALSE) return -1;
	return pointbuf;
}
#endif

#ifdef _FAMILYBADGE_
/**
 * 获取家族徽章。
 *
 * @param char_index 角色索引
 * @return int 家族徽章的int类型
 */
int getFamilyBadge(const int char_index)
{
	if (CHAR_CHECKINDEX(char_index)) {
		int fmindex_wk = CHAR_getWorkInt(char_index, CHAR_WORKFMINDEXI);
		if (fmindex_wk < 0 || fmindex_wk >= FAMILY_MAXNUM) return 0;
		return memberlist[fmindex_wk].badge;
	}
	return 0;
}
#endif
