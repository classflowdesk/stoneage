#include "version.h"
#include "longzoro/newshop.h"
#include "longzoro/sasql.h"
#include "char.h"
#include "autil.h"
#include "enemy.h"
#include "item.h"
#include "gmsv_server.h"

#ifdef _ONLINE_SHOP

Online_Shop_t onlineshop[ONLINESHOP_NUM][SHOPLISTMAX];
int max[ONLINESHOP_NUM]={0};

BOOL OnlineShop_init()
{
	FILE* fp;
	int shoplist=0;
#ifdef _CRYPTO_DATA		
	BOOL crypto = FALSE;
	fp = fopen( "data/onlineshop.txt.allblues", "r");
	if( fp != NULL ){
		crypto = TRUE;
	}else
#endif
{
	fp = fopen("data/onlineshop.txt", "r");
}
	if (fp == NULL)
	{
		print("无法打开文件\n");
		return FALSE;
	}
	memset( &onlineshop, 0, sizeof( onlineshop ) );
	memset( &max, 0, sizeof( max ) );
	char line[64],buf[64];
	while( fgets( line , sizeof( line ), fp ) ){
#ifdef _CRYPTO_DATA
		if(crypto==TRUE){
			DecryptKey(line);
		}
#endif
		
		if( line[0] == '#' )continue;
    if( line[0] == '\n' )continue;
		if( strlen(line)== 0 )continue;
			
		chop(line);

		if(strcmp(line, "ONLINESHOP_PET")==0){
			shoplist=ONLINESHOP_PET;
			continue;
		}else if(strcmp(line, "ONLINESHOP_ITEM")==0){
			shoplist=ONLINESHOP_ITEM;
			continue;
		}else if(strcmp(line, "ONLINESHOP_HEALER")==0){
			shoplist=ONLINESHOP_HEALER;
			continue;
		}else if(strcmp(line, "ONLINESHOP_COMPOSE")==0){
			shoplist=ONLINESHOP_COMPOSE;
			continue;
		}else if(strcmp(line, "ONLINESHOP_MISSION")==0){
			shoplist=ONLINESHOP_MISSION;
			continue;
		}else if(strcmp(line, "ONLINESHOP_OTHER")==0){
			shoplist=ONLINESHOP_OTHER;
			continue;
		}else if(strcmp(line, "ONLINESHOP_AMPOINT")==0){
			shoplist=ONLINESHOP_AMPOINT;
			continue;
		}
		int list=max[shoplist];
		
		if(getStringFromIndexWithDelim(line,",", 1, onlineshop[shoplist][list].name, sizeof(onlineshop[shoplist][list].name))==FALSE){
			onlineshop[shoplist][list].use=FALSE;
			continue;
		}
		if(getStringFromIndexWithDelim(line,",", 2, buf, sizeof(buf))==TRUE){
			onlineshop[shoplist][list].id=atoi(buf);
		}else{
			onlineshop[shoplist][list].use=FALSE;
			continue;
		}
		if(getStringFromIndexWithDelim(line,",", 3, buf, sizeof(buf))==TRUE){
			onlineshop[shoplist][list].picid=atoi(buf);
		}else{
			onlineshop[shoplist][list].use=FALSE;
			continue;
		}
		if(getStringFromIndexWithDelim(line,",", 4, buf, sizeof(buf))==TRUE){
			onlineshop[shoplist][list].price=atoi(buf);
		}else{
			onlineshop[shoplist][list].use=FALSE;
			continue;
		}
		if(getStringFromIndexWithDelim(line,",", 5, onlineshop[shoplist][list].readme, sizeof(onlineshop[shoplist][list].readme))==FALSE){
			strcmp(onlineshop[shoplist][list].readme,onlineshop[shoplist][list].name);
		}
		if(getStringFromIndexWithDelim(line,",", 6, buf, sizeof(buf))==TRUE){
			onlineshop[shoplist][list].percentage=atoi(buf);
		}else{
			onlineshop[shoplist][list].percentage=100;
		}
		if(getStringFromIndexWithDelim(line,",", 7, buf, sizeof(buf))==TRUE){
			onlineshop[shoplist][list].color=atoi(buf);
		}else{
			onlineshop[shoplist][list].color=CHAR_COLORWHITE;
		}
		onlineshop[shoplist][list].use=TRUE;
		max[shoplist]++;
	}
	fclose(fp);
	return TRUE;
}

void OnlineShop_ShowList(int fd, int char_index, int type, int page)
{
	if(type>ONLINESHOP_NUM)return;
	int pagemax=(max[type-1]-1)/SHOPPAGEMAX+1;
	if(page>pagemax)return;
	char token[4096]="";
	char buf[1024];
	int i;
	int availability=0;
	int startpage=(page-1)*SHOPPAGEMAX;
	if(startpage<=0)startpage=0;

	for(i=startpage;i<startpage+SHOPPAGEMAX;i++){
		if(i>SHOPLISTMAX)break;
		if(onlineshop[type-1][i].use==TRUE){
			sprintf(buf,"%d#%d#%s#%d#%d#%d#%s|",i,
																					onlineshop[type-1][i].picid,
																					onlineshop[type-1][i].name,
																					onlineshop[type-1][i].color,
																					onlineshop[type-1][i].percentage,
																					onlineshop[type-1][i].price,
																					onlineshop[type-1][i].readme);
			strcat(token, buf);
			availability++;
		}
	}


	if((type-1) == ONLINESHOP_AMPOINT){
		GmsvServer_VIP_SHOP_send( fd, availability,CHAR_getInt(char_index, CHAR_AMPOINT), type, page, pagemax ,token);
	}else{
#ifdef _ONLINE_SHOP_MYSQL
		GmsvServer_VIP_SHOP_send( fd, availability,sasql_getVipPoint(char_index), type, page, pagemax ,token);
#else	
	GmsvServer_VIP_SHOP_send( fd, availability,CHAR_getInt(char_index, CHAR_BJ), type, page, pagemax ,token);
#endif
	}
}

void OnlineShop_Buy(int fd, int char_index, int type, int page, int id, int num)
{
	switch(type-1){
		case ONLINESHOP_PET:
			{
#ifdef _ONLINE_SHOP_MYSQL
				int bj = sasql_getVipPoint(char_index);
#else	
				int bj = CHAR_getInt(char_index, CHAR_BJ);
#endif
				int price = onlineshop[type-1][id].price * onlineshop[type-1][id].percentage / 100;
				if(bj >= price * num){
					char token[64];
					int i, j;
					int enemynum = ENEMY_getEnemyNum();
					for(j=0;j<num;j++){
						for( i = 0; i < enemynum; i ++ ) {
							if( ENEMY_getInt( i, ENEMY_ID) == onlineshop[type-1][id].id) {
								break;
							}
						}
						if( i == enemynum ){
							CHAR_talkToCli( char_index, -1, "该宠物不存在。", CHAR_COLORYELLOW);
						break;
						}
					
						int ret = ENEMY_createPetFromEnemyIndex(char_index, i);
						if( !CHAR_CHECKINDEX( ret)){
							CHAR_talkToCli( char_index, -1, "宠物栏位不足。", CHAR_COLORYELLOW);
						break;
						}else{
							for( i = 0; i < CHAR_MAXPETHAVE; i ++ ){
								if( CHAR_getCharPet( char_index, i ) == ret )break;
							}
							if( i == CHAR_MAXPETHAVE )return;
						
							CHAR_setMaxExpFromLevel( ret, CHAR_getInt( ret, CHAR_LV ));
							CHAR_setInt(ret,CHAR_VARIABLEAI,10000);
							CHAR_setInt( ret, CHAR_HP, CHAR_getWorkInt( ret, CHAR_WORKMAXHP ));
							CHAR_complianceParameter( ret );
							snprintf( token, sizeof( token ), "K%d", i );
							CHAR_sendStatusString( char_index, token );
							snprintf( token, sizeof( token ), "W%d", i );
							CHAR_sendStatusString( char_index, token );
#ifdef _ONLINE_SHOP_MYSQL
							sasql_setVipPoint(char_index, sasql_getVipPoint(char_index) - price);
#else
							CHAR_setInt(char_index, CHAR_BJ, CHAR_getInt(char_index, CHAR_BJ) - price);
#endif
							char token[256];
							sprintf(token, "成功购买 %s 宠物", CHAR_getChar(ret, CHAR_NAME));
							CHAR_talkToCli( char_index, -1, token, CHAR_COLORYELLOW);
						}
					}
				}else{
					CHAR_talkToCli( char_index, -1, "你的点数不足!", CHAR_COLORYELLOW);
				}
			}
			break;
		case ONLINESHOP_ITEM:
		case ONLINESHOP_HEALER:
		case ONLINESHOP_COMPOSE:
		case ONLINESHOP_MISSION:
			{
#ifdef _ONLINE_SHOP_MYSQL
				int bj = sasql_getVipPoint(char_index);
#else	
				int bj = CHAR_getInt(char_index, CHAR_BJ);
#endif
				int price = onlineshop[type-1][id].price * onlineshop[type-1][id].percentage / 100;
				if(bj >= price * num){
					int i;
					for(i=0;i<num;i++){
						int emptyitem_indexinchara = CHAR_findEmptyItemBox( char_index );
						
						if( emptyitem_indexinchara < 0 ){
							CHAR_talkToCli( char_index, -1, "物品栏位不足。", CHAR_COLORYELLOW);
							break;
						}
						
						int item_index = ITEM_makeItemAndRegist( onlineshop[type-1][id].id );
						
						if( item_index != -1 ){
#ifdef _MO_LUA_ADDITEM_CALLBACK

							NpcAdditemFunction(char_index, onlineshop[type-1][id].id);

#endif
							 CHAR_setItemIndex( char_index, emptyitem_indexinchara, item_index );
							 ITEM_setWorkInt(item_index, ITEM_WORKOBJINDEX,-1);
							 ITEM_setWorkInt(item_index, ITEM_WORKCHARAINDEX,char_index);
							 CHAR_sendItemDataOne( char_index, emptyitem_indexinchara);
#ifdef _ONLINE_SHOP_MYSQL
							 sasql_setVipPoint(char_index, sasql_getVipPoint(char_index) - price);
#else
							 CHAR_setInt(char_index, CHAR_BJ, CHAR_getInt(char_index, CHAR_BJ) - price);
#endif
							char token[256];
							sprintf(token, "成功购买 %s 道具", ITEM_getChar(item_index, ITEM_NAME));
							CHAR_talkToCli( char_index, -1, token, CHAR_COLORYELLOW);
						}else{
							CHAR_talkToCli( char_index, -1, "该物品不存在!", CHAR_COLORYELLOW);       
							break;
						}
					}
				}else{
					CHAR_talkToCli( char_index, -1, "你的点数不足!", CHAR_COLORYELLOW);
				}
			}
			break;
		case ONLINESHOP_OTHER:
			{
#ifdef _ONLINE_SHOP_MYSQL
				int bj = sasql_getVipPoint(char_index);
#else	
				int bj = CHAR_getInt(char_index, CHAR_BJ);
#endif
				int price = onlineshop[type-1][id].price * onlineshop[type-1][id].percentage / 100 * num;
				if(bj >= price){
#ifdef _ONLINE_SHOP_MYSQL
					sasql_setVipPoint(char_index, bj - price);
#else	
					CHAR_setInt(char_index, CHAR_BJ, bj - price);
#endif
					CHAR_setInt(char_index, CHAR_AMPOINT, CHAR_getInt(char_index, CHAR_AMPOINT) + onlineshop[type-1][id].id * num);
				}
				
			}
			break;

		case ONLINESHOP_AMPOINT:
			{
				int ampoint = CHAR_getInt(char_index, CHAR_AMPOINT);
				int price = onlineshop[type-1][id].price * onlineshop[type-1][id].percentage / 100 * num;
				if(ampoint >= price){
					CHAR_setInt(char_index, CHAR_AMPOINT, ampoint - price);
#ifdef _ONLINE_SHOP_MYSQL
					sasql_setVipPoint(char_index, sasql_getVipPoint(char_index) + onlineshop[type-1][id].id * num);
#else	
					CHAR_setInt(char_index, CHAR_BJ, CHAR_getInt(char_index, CHAR_BJ) + onlineshop[type-1][id].id * num);
#endif
					
				}
			}
			break;

	}
	int i;
	int pagemax=(max[type-1]-1)/SHOPPAGEMAX+1;
	for(i=0;i<ONLINESHOP_NUM-1;i++){
#ifdef _ONLINE_SHOP_MYSQL
		GmsvServer_VIP_SHOP_send( fd, 0, (type-1==ONLINESHOP_AMPOINT)?CHAR_getInt(char_index, CHAR_AMPOINT):sasql_getVipPoint(char_index), i, page, pagemax ,"");
#else	
		GmsvServer_VIP_SHOP_send( fd, 0, (type-1==ONLINESHOP_AMPOINT)?CHAR_getInt(char_index, CHAR_AMPOINT):CHAR_getInt(char_index, CHAR_BJ), i, page, pagemax ,"");
#endif
		
	}
}

#endif


