#include "version.h"
//
#include "common.h"
//
#include "autil.h"
#include "buf.h"
#include "config_file.h"
#include "magic.h"
#include "magic_base.h"

static Magic *MAGIC_magic;
static int sMagicNum;

#ifdef _ATTACK_MAGIC
AttMagic *ATTMAGIC_magic;
int ATTsMagicNum;
#endif

typedef struct tagMagicFunctionTable {
  char *functionname;
  MAGIC_CALLFUNC func;
  int hash;
} MagicFunctionTable;

static MagicFunctionTable sMageicFunctionTable[] = {
#ifdef _MAGIC_FEATHERS
    {"MAGIC_Feathers", MAGIC_Feathers, 0},
#endif
    {"MAGIC_Recovery", MAGIC_Recovery, 0},
    {"MAGIC_OtherRecovery", MAGIC_OtherRecovery, 0},
    {"MAGIC_FieldAttChange", MAGIC_FieldAttChange, 0},
    {"MAGIC_StatusChange", MAGIC_StatusChange, 0},
    {"MAGIC_MagicDef", MAGIC_MagicDef, 0},
    {"MAGIC_StatusRecovery", MAGIC_StatusRecovery, 0},
    {"MAGIC_Ressurect", MAGIC_Ressurect, 0},
    {"MAGIC_AttReverse", MAGIC_AttReverse, 0},
    {"MAGIC_ResAndDef", MAGIC_ResAndDef, 0},

#ifdef _ATTACK_MAGIC
    {"MAGIC_AttMagic", MAGIC_AttMagic, 0},
#endif
#ifdef _OTHER_MAGICSTAUTS
    {"MAGIC_MagicStatusChange", MAGIC_MagicStatusChange, 0},
#endif
#ifdef _ITEM_METAMO
    {"MAGIC_Metamo", MAGIC_Metamo, 0},
#endif
#ifdef _ITEM_ATTSKILLMAGIC
    {"MAGIC_AttSkill", MAGIC_AttSkill, 0},
#endif
#ifdef _MAGIC_WEAKEN // vincent  精灵:虚弱
    {"MAGIC_Weaken", MAGIC_Weaken, 0},
#endif
#ifdef _MAGIC_DEEPPOISON // vincent  精灵:剧毒
    {"MAGIC_StatusChange2", MAGIC_StatusChange2, 0},
#endif
#ifdef _MAGIC_BARRIER // vincent  精灵:魔障
    {"MAGIC_Barrier", MAGIC_Barrier, 0},
#endif
#ifdef _MAGIC_NOCAST // vincent  精灵:沉默
    {"MAGIC_Nocast", MAGIC_Nocast, 0},
#endif
#ifdef _MAGIC_TOCALL // 奔龙阵
    {"MAGIC_ToCallDragon", MAGIC_ToCallDragon, 0},
#endif
};

INLINE BOOL MAGIC_CHECKINDEX(int index) {
  if (sMagicNum <= index || index < 0)
    return FALSE;
  return TRUE;
}

static INLINE BOOL MAGIC_CHECKINTDATAINDEX(int index) {
  if (MAGIC_DATAINTNUM <= index || index < 0)
    return FALSE;
  return TRUE;
}

static INLINE BOOL MAGIC_CHECKCHARDATAINDEX(int index) {
  if (MAGIC_DATACHARNUM <= index || index < 0)
    return FALSE;
  return TRUE;
}

INLINE int MAGIC_getInt(int index, MAGIC_DATAINT element) {
  return MAGIC_magic[index].data[element];
}

INLINE int MAGIC_setInt(int index, MAGIC_DATAINT element, int data) {
  int buf;
  buf = MAGIC_magic[index].data[element];
  MAGIC_magic[index].data[element] = data;
  return buf;
}
/*----------------------------------------------------------------------*/
INLINE char *MAGIC_getChar(int index, MAGIC_DATACHAR element) {
  if (!MAGIC_CHECKINDEX(index))
    return "\0";
  if (!MAGIC_CHECKCHARDATAINDEX(element))
    return "\0";
  return MAGIC_magic[index].string[element].string;
}

/*----------------------------------------------------------------------*/
INLINE BOOL MAGIC_setChar(int index, MAGIC_DATACHAR element, char *new) {
  if (!MAGIC_CHECKINDEX(index))
    return FALSE;
  if (!MAGIC_CHECKCHARDATAINDEX(element))
    return FALSE;
  strcpysafe(MAGIC_magic[index].string[element].string,
             sizeof(MAGIC_magic[index].string[element].string), new);
  return TRUE;
}
/*----------------------------------------------------------------------
 *   芊及醒毛襞月［
 *---------------------------------------------------------------------*/
int MAGIC_getMagicNum(void) { return sMagicNum; }

BOOL MAGIC_initMagic(char *filename) {
  FILE *f;
  char line[256];
  int linenum = 0;
  int magic_readlen = 0;
  int i, j;
  int max_magic_id = 0;
  char token[256];
#ifdef _CRYPTO_DATA
  char realopfile[256];
  BOOL crypto = FALSE;
  sprintf(realopfile, "%s.allblues", filename);
  f = fopen(realopfile, "r");
  if (f != NULL) {
    crypto = TRUE;
  } else
#endif
  {
    f = fopen(filename, "r");
  }
  if (f == NULL) {
    print("文件打开失败\n");
    return FALSE;
  }
  sMagicNum = 0;
  while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    linenum++;
    if (line[0] == '#')
      continue; /* comment */
    if (line[0] == '\n')
      continue; /* none    */
    chomp(line);

#ifdef _MAGIC_OPTIMUM // Robin 取出最大MAGIC ID
    if (getStringFromIndexWithDelim(line, ",", MAGIC_DATACHARNUM + MAGIC_ID + 1,
                                    token, sizeof(token)) == FALSE)
      continue;
    max_magic_id = max(atoi(token), max_magic_id);
#endif

    sMagicNum++;
  }

#ifdef _MAGIC_OPTIMUM
  print("有效魔法:%d 最大魔法:%d ...", sMagicNum, max_magic_id);
  sMagicNum = max_magic_id + 1;
#endif

  if (fseek(f, 0, SEEK_SET) == -1) {
    fprint("搜索错误\n");
    fclose(f);
    return FALSE;
  }

  MAGIC_magic = allocateMemory(sizeof(struct tagMagic) * sMagicNum);
  if (MAGIC_magic == NULL) {
    fprint("无法分配内存 %d\n", sizeof(struct tagMagic) * sMagicNum);
    fclose(f);
    return FALSE;
  }

  for (i = 0; i < sMagicNum; i++) {
    for (j = 0; j < MAGIC_DATAINTNUM; j++) {
      MAGIC_setInt(i, j, -1);
    }
    for (j = 0; j < MAGIC_DATACHARNUM; j++) {
      MAGIC_setChar(i, j, "");
    }
  }

  linenum = 0;
  while (fgets(line, sizeof(line), f)) {
#ifdef _CRYPTO_DATA
    if (crypto == TRUE) {
      DecryptKey(line);
    }
#endif
    linenum++;
    if (line[0] == '#')
      continue; /* comment */
    if (line[0] == '\n')
      continue; /* none    */
    chomp(line);
    replaceString(line, '\t', ' ');
    {
      char buf[256];
      for (i = 0; i < strlen(line); i++) {
        if (line[i] != ' ') {
          break;
        }
        strcpy(buf, &line[i]);
      }
      if (i != 0) {
        strcpy(line, buf);
      }
    }
    {
      char token[256];
      int ret;

#ifdef _MAGIC_OPTIMUM
      if (getStringFromIndexWithDelim(line, ",",
                                      MAGIC_DATACHARNUM + MAGIC_ID + 1, token,
                                      sizeof(token)) == FALSE)
        continue;
      magic_readlen = atoi(token);
#endif

      for (i = 0; i < MAGIC_DATACHARNUM; i++) {

        /*    侬  迕玄□弁件毛苇月    */
        ret =
            getStringFromIndexWithDelim(line, ",", i + 1, token, sizeof(token));
        if (ret == FALSE) {
          fprint("文件语法错误:%s 第%d行\n", filename, linenum);
          break;
        }
        MAGIC_setChar(magic_readlen, i, token);
      }
      /* 4勾  动嫦反醒袄犯□正 */
#define MAGIC_STARTINTNUM 5
      for (i = MAGIC_STARTINTNUM; i < MAGIC_DATAINTNUM + MAGIC_STARTINTNUM;
           i++) {
        ret = getStringFromIndexWithDelim(line, ",", i, token, sizeof(token));

#ifdef _ATTACK_MAGIC

        if (FALSE == ret)

          break;

        if (0 != strlen(token)) {
          MAGIC_setInt(magic_readlen, i - MAGIC_STARTINTNUM, atoi(token));
        }

#else

        if (ret == FALSE) {
          fprint("文件语法错误:%s 第%d行\n", filename, linenum);
          break;
        }
        if (strlen(token) != 0) {
          MAGIC_setInt(magic_readlen, i - MAGIC_STARTINTNUM, atoi(token));
        }

#endif
      }

#ifdef _ATTACK_MAGIC

      if (i != MAGIC_STARTINTNUM + MAGIC_IDX &&
          i != MAGIC_DATAINTNUM + MAGIC_STARTINTNUM)
        continue;

#else

      if (i < MAGIC_DATAINTNUM + MAGIC_STARTINTNUM)
        continue;

#endif
      /* 切斤匀午尕称鼎分仃升仇丹允月［ */
      if (MAGIC_getInt(magic_readlen, MAGIC_TARGET_DEADFLG) == 1) {
        MAGIC_setInt(magic_readlen, MAGIC_TARGET,
                     MAGIC_getInt(magic_readlen, MAGIC_TARGET) + 100);
      }

      magic_readlen++;
    }
  }
  fclose(f);

  sMagicNum = magic_readlen;

  print("有效魔法数是 %d...", sMagicNum);

  /* hash 及瓒   */
  for (i = 0; i < arraysizeof(sMageicFunctionTable); i++) {
    sMageicFunctionTable[i].hash =
        hashpjw(sMageicFunctionTable[i].functionname);
  }
  return TRUE;
}

BOOL MAGIC_reinitMagic(void) {
  freeMemory(MAGIC_magic);
  return (MAGIC_initMagic(getMagicfile()));
}

#ifdef _ATTACK_MAGIC

BOOL ATTMAGIC_initMagic(char *filename) {
  FILE *file;
  // Open file
  if (NULL == (file = fopen(filename, "r"))) {
    ATTsMagicNum = 0;
    ATTMAGIC_magic = NULL;
    return TRUE;
  }
  fseek(file, 0, SEEK_END);
  // Calculate the number of attack magics
  ATTsMagicNum = ftell(file) / sizeof(struct tagAttMagic);
  if (ATTsMagicNum % 2) {
    fprint("打开文件失败\n");
    fclose(file);
    return FALSE;
  }
  fseek(file, 0, SEEK_SET);
  // Allocate memory to attack magics
  ATTMAGIC_magic =
      allocateMemory(sizeof(struct tagAttMagic) * ATTsMagicNum);
  if (NULL == ATTMAGIC_magic) {
    fprint("无法分配内存 %d\n", sizeof(struct tagAttMagic) * ATTsMagicNum);
    fclose(file);
    return FALSE;
  }
  // Read attack magics information
  memset(ATTMAGIC_magic, 0, sizeof(struct tagAttMagic) * ATTsMagicNum);
  fread(ATTMAGIC_magic, 1, sizeof(struct tagAttMagic) * ATTsMagicNum,
        file);
  fclose(file);
  ATTsMagicNum = ATTsMagicNum / 2;
  print("有效的攻击魔法数 %d\n", ATTsMagicNum);
  return TRUE;
}

BOOL ATTMAGIC_reinitMagic(void) {
  freeMemory(ATTMAGIC_magic);
  ATTsMagicNum = 0;
  return ATTMAGIC_initMagic(getAttMagicfileName());
}

#endif

int MAGIC_getMagicArray(int magic_id) {
#ifdef _MAGIC_OPTIMUM
  if (magic_id >= 0 && magic_id < sMagicNum)
    return magic_id;
#else
  int i;
  for (i = 0; i < sMagicNum; i++) {
    if (MAGIC_magic[i].data[MAGIC_ID] == magic_id) {
      return i;
    }
  }
#endif
  return -1;
}

MAGIC_CALLFUNC MAGIC_getMagicFuncPointer(char *name) {
  int i;
  if (name == NULL)
    return NULL;
  int hash = hashpjw(name);
  for (i = 0; i < arraysizeof(sMageicFunctionTable); i++) {
    if (sMageicFunctionTable[i].hash == hash) {
      if (strcmp(sMageicFunctionTable[i].functionname, name) == 0) {
        return sMageicFunctionTable[i].func;
      }
    }
  }
  return NULL;
}

int MAGIC_isTargetValid(int magic_id, int to_index) {
  int magic_array = MAGIC_getMagicArray(magic_id);
#ifdef _ATTACK_MAGIC

  if (to_index >= 0 && to_index <= 19)
    return 0;

  // One side of players
  if (20 == to_index || 21 == to_index) {
    if (MAGIC_TARGET_WHOLEOTHERSIDE ==
            MAGIC_magic[magic_array].data[MAGIC_TARGET] ||
        MAGIC_TARGET_ALL_ROWS == MAGIC_magic[magic_array].data[MAGIC_TARGET])
      return 0;
    else
      return -1;
  }

  // All players
  if (22 == to_index) {
    if (MAGIC_TARGET_ALL == MAGIC_magic[magic_array].data[MAGIC_TARGET])
      return 0;
    else
      return -1;
  }

  // One row
  if (23 == to_index || 24 == to_index || 25 == to_index || 26 == to_index) {
    if (MAGIC_TARGET_ONE_ROW == MAGIC_magic[magic_array].data[MAGIC_TARGET])
      return 0;
    else
      return -1;
  }

#else
  // Single player
  if ((to_index >= 0x0) && (to_index <= 0x13))
    return 0;
  // All players
  if (to_index == 0x16) {
    if (MAGIC_magic[magic_array].data[MAGIC_TARGET] == MAGIC_TARGET_ALL)
      return 0;
    else
      return -1;
  }
  // One side of players
  if ((to_index == 0x14) || (to_index == 0x15)) {
    if (MAGIC_magic[magic_array].data[MAGIC_TARGET] ==
        MAGIC_TARGET_WHOLEOTHERSIDE)
      return 0;
    else
      return -1;
  }
#endif
  // Others: Error
  return -1;
}
// Nuke end
