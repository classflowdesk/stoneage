#include "version.h"

static Char player = {
    FALSE, /* use */
    {
        0, /*  CHAR_DATAPLACENUMBER */
        0, /*  CHAR_BASEIMAGENUMBER */
        0, /*  CHAR_BASEBASEIMAGENUMBER */
        0, /*  CHAR_FACEIMAGENUMBER */
        0, /*  CHAR_FLOOR */
        0, /*  CHAR_X */
        0, /*  CHAR_Y */
        5, /*  CHAR_DIRECTION */
        1, /*  CHAR_LV */
        0, /*  CHAR_GOLD */
        1, /*  CHAR_HP */
        0, /*  CHAR_MP */
        0, /*  CHAR_MAXMP */
        0, /*  CHAR_VITAL */
        0, /*  CHAR_STR */
        0, /*  CHAR_TOUGH */
        0, /*  CHAR_DEX  */

        0, /*  CHAR_CHARM  */
        0, /*  CHAR_LUCK  */
        0, /* ��°�� */
        0, /* ��°�� */
        0, /* ��°�� */
        0, /* ��°�� */

        0,                   /* CHAR_SLOT*/
        0,                   /* CHAR_CRITIAL */
        0,                   /* CHAR_COUNTER */
        0,                   /* CHAR_RARE */
        0,                   /*  CHAR_RADARSTRLENGTH */
        0,                   /*  CHAR_CHATVOLUME */
        MAKE2VALUE(100, 20), /*  CHAR_MERCHANTLEVEL  */
        0,                   /*  CHAR_HEALERLEVEL    */
        0,                   /*  CHAR_DETERMINEITEM  */
        -1, /*  CHAR_INDEXOFEQTITLE  */
        0, /*  CHAR_POISON  */
        0, /*  CHAR_PARALYSIS  */
        0, /*  CHAR_SILENCE    */
        0, /*  CHAR_STONE  */
        0, /*  CHAR_DARKNESS   */
        0, /*  CHAR_CONFUSION  */
        0, /*  CHAR_LOGINCOUNT */
        0, /*  CHAR_DEADCOUNT */
        0, /*  CHAR_WALKCOUNT */
        0, /*  CHAR_TALKCOUNT */
        0, /*  CHAR_DAMAGECOUNT */
        0, /*  CHAR_GETPETCOUNT */
        0, /*  CHAR_KILLPETCOUNT */
        0, /*  CHAR_DEADPETCOUNT */
        0, /*  CHAR_SENDMAILCOUNT */
        0, /*  CHAR_MERGEITEMCOUNT */

        0, /*  CHAR_DUELBATTLECOUNT */
        0, /*  CHAR_DUELWINCOUNT */
        0, /*  CHAR_DUELLOSECOUNT */
        0, /*  CHAR_DUELSTWINCOUNT */
        0, /*  CHAR_DUELMAXSTWINCOUNT */
        0, /*  CHAR_DUELMAXSTWINCOUNT */

        CHAR_TYPEPLAYER, /*  CHAR_WHICHTYPE  */
        1000,            /*  CHAR_WALKINTERVAL   */
        1000,            /*  CHAR_LOOPINTERVAL   */
#ifdef _NEWOPEN_MAXEXP
        0, // CHAR_OLDEXP,
#endif
        0,  // CHAR_EXP,
        -1, /*  CHAR_LASTTALKELDER*/
        0,  /*  CHAR_SKILLUPPOINT    */
        0,  /*  CHAR_LEVELUPPOINT */

        0,               /*  CHAR_IMAGETYPE    */
        CHAR_COLORWHITE, /* CHAR_NAMECOLOR */
        CHAR_COLORWHITE, /* CHAR_POPUPNAMECOLOR */
        0,               /*  CHAR_LASTTIMESETLUCK */
        0,               /*  CHAR_DUELPOINT */
        0,               /*  CHAR_EVENT */
        0,               /*  CHAR_EVENT2 */
        0,               /*  CHAR_EVENT3 */
#ifdef _NEWEVENT
        0, /*  CHAR_EVENT4 */
        0, /*  CHAR_EVENT5 */
        0, /*  CHAR_EVENT6 */
#endif
#ifdef _ADD_NEWEVENT // WON �����������
        0,           /*  CHAR_EVENT7  */
        0,           //  CHAR_EVENT8, 224~255 �����ٻ�ר��
#endif
#ifdef _ADD_NEWEVENT_1024
        0, /*  CHAR_EVENT9  */
        0, /*  CHAR_EVENT10 */
        0, /*  CHAR_EVENT11 */
        0, /*  CHAR_EVENT12 */
        0, /*  CHAR_EVENT13 */
        0, /*  CHAR_EVENT14 */
        0, /*  CHAR_EVENT15 */
        0, /*  CHAR_EVENT16 */
        0, /*  CHAR_EVENT17 */
        0, /*  CHAR_EVENT18 */
        0, /*  CHAR_EVENT19 */
        0, /*  CHAR_EVENT20 */
        0, /*  CHAR_EVENT21 */
        0, /*  CHAR_EVENT22 */
        0, /*  CHAR_EVENT23 */
        0, /*  CHAR_EVENT24 */
        0, /*  CHAR_EVENT25 */
        0, /*  CHAR_EVENT26 */
        0, /*  CHAR_EVENT27 */
        0, /*  CHAR_EVENT28 */
        0, /*  CHAR_EVENT29 */
        0, /*  CHAR_EVENT30 */
        0, /*  CHAR_EVENT31 */
        0, /*  CHAR_EVENT32 */
#endif
        0, /*  CHAR_NOWEVENT */
        0, /*  CHAR_NOWEVENT2 */
        0, /*  CHAR_NOWEVENT3 */
#ifdef _NEWEVENT
        0, /*  CHAR_NOWEVENT4 */
        0, /*  CHAR_NOWEVENT5 */
        0, /*  CHAR_NOWEVENT6 */
#endif
#ifdef _ADD_NEWEVENT // WON �����������
        0,           /*  CHAR_NOWEVENT7  */
        0,           //  CHAR_NOWEVENT8, 224~255 �����ٻ�ר��
#endif
#ifdef _ADD_NEWEVENT_1024 // WON �����������
        0,                /*  CHAR_NOWEVENT9 */
        0,                /*  CHAR_NOWEVENT10 */
        0,                /*  CHAR_NOWEVENT11 */
        0,                /*  CHAR_NOWEVENT12 */
        0,                /*  CHAR_NOWEVENT13 */
        0,                /*  CHAR_NOWEVENT14 */
        0,                /*  CHAR_NOWEVENT15 */
        0,                /*  CHAR_NOWEVENT16 */
        0,                /*  CHAR_NOWEVENT17 */
        0,                /*  CHAR_NOWEVENT18 */
        0,                /*  CHAR_NOWEVENT19 */
        0,                /*  CHAR_NOWEVENT20 */
        0,                /*  CHAR_NOWEVENT21 */
        0,                /*  CHAR_NOWEVENT22 */
        0,                /*  CHAR_NOWEVENT23 */
        0,                /*  CHAR_NOWEVENT24 */
        0,                /*  CHAR_NOWEVENT25 */
        0,                /*  CHAR_NOWEVENT26 */
        0,                /*  CHAR_NOWEVENT27 */
        0,                /*  CHAR_NOWEVENT28 */
        0,                /*  CHAR_NOWEVENT29 */
        0,                /*  CHAR_NOWEVENT30 */
        0,                /*  CHAR_NOWEVENT31 */
        0,                /*  CHAR_NOWEVENT32 */
#endif
        0, /* CHAR_TRANSMIGRATION */
        0, /* CHAR_TRANSEQUATION */

        0, /*  CHAR_INITDATA    */

        // #ifdef _PETSKILL_BECOMEPIG
        //         -1,
        // #endif

        0, // CHAR_SILENT,        /* char shutup time */
        0, //    CHAR_FMINDEX,        // �a�� index
        0, //    CHAR_FMLEADERFLAG,
           /* �a�ڦ�������
            * FMMEMBER_NONE  :�S���[�J����a��
            * FMMEMBER_APPLY  :�ӽХ[�J�a�ڤ�
            * FMMEMBER_LEADER  :�ڪ�
            * FMMEMBER_MEMBER  :�@�릨��
            * FMMEMBER_ELDER  :����
            * FMMEMBER_INVITE  :���q      // �Ȯɤ���
            * FMMEMBER_BAILEE  :�]�Ȫ�      // �Ȯɤ���
            * FMMEMBER_VICELEADER  :�Ʊڪ�    // �Ȯɤ���
            */
        0, //    CHAR_FMSPRITE,    // �a�ڦu�@���F

        0, //    CHAR_BANKGOLD,
        0, //    CHAR_RIDEPET,
        0, //    CHAR_LEARNRIDE,
#ifdef _NEW_RIDEPETS
        0, //  CHAR_LOWRIDEPETS,
#endif
        0, //   CHAR_LIMITLEVEL,
#ifdef _PET_FUSION
        0, //  CHAR_FUSIONCODE,    //���ؽs�X
        0, //  CHAR_FUSIONINDEX,    //����d���s��
        0, //  CHAR_FUSIONRAISE,    //���i����
        0, //  CHAR_FUSIONBEIT,    //�d�J�X��
        0, //  CHAR_FUSIONTIMELIMIT,  //���i�ɶ�
#endif
#ifdef _PERSONAL_FAME // Arminius 8.30: �a�ڭӤH�n��
        0,            //    CHAR_FAME,
#endif

#ifdef _NEWSAVE
        0, //    CHAR_SAVEINDEXNUMBER,  /*  SaveFile .0.char or .1.char */
#endif

#ifdef _ATTACK_MAGIC
        0, //    CHAR_EARTH_EXP,            // ���a���a�]�k���m��
        0, //    CHAR_WATER_EXP,            // ���a�����]�k���m��
        0, //    CHAR_FIRE_EXP,            // ���a�����]�k���m��
        0, //    CHAR_WIND_EXP,            // ���a�����]�k���m��
        0, //    CHAR_EARTH_RESIST,          // ���a���a�]�k�ܩ�
        0, //    CHAR_WATER_RESIST,          // ���a�����]�k�ܩ�
        0, //    CHAR_FIRE_RESIST,          // ���a�����]�k�ܩ�
        0, //    CHAR_WIND_RESIST,          // ���a�����]�k�ܩ�
        0, //    CHAR_EARTH_ATTMAGIC_EXP,      //
           //    ���a���a�]�k���m�׸g���
        0, //    CHAR_WATER_ATTMAGIC_EXP,      //
           //    ���a�����]�k���m�׸g���
        0, //    CHAR_FIRE_ATTMAGIC_EXP,        //
           //    ���a�����]�k���m�׸g���
        0, //    CHAR_WIND_ATTMAGIC_EXP,        //
           //    ���a�����]�k���m�׸g���
        0, //     CHAR_EARTH_DEFMAGIC_EXP,      // ���a���a�]�k�ܩʸg���
        0, //    CHAR_WATER_DEFMAGIC_EXP,      // ���a�����]�k�ܩʸg���
        0, //    CHAR_FIRE_DEFMAGIC_EXP,        // ���a�����]�k�ܩʸg���
        0, //    CHAR_WIND_DEFMAGIC_EXP,        // ���a�����]�k�ܩʸg���
#endif

#ifdef _GAMBLE_BANK
        0, //    CHAR_PERSONAGOLD,  //����ӤH�Ȧ�
#endif
#ifdef _DROPSTAKENEW
        0, //    CHAR_GAMBLENUM,    //����n��
#endif
#ifdef _ADD_ACTION // npc�ʧ@
        0,         //    CHAR_ACTIONSTYLE,
#endif
#ifdef _PET_EVOLUTION
        0, //    CHAR_EVOLUTIONBASEVTL,
        0, //    CHAR_EVOLUTIONBASESTR,
        0, //    CHAR_EVOLUTIONBASETGH,
        0, //    CHAR_EVOLUTIONBASEDEX,
#endif

#ifdef _FM_JOINLIMIT
        0, //    CHAR_FMTIMELIMIT,
#endif

#ifdef _CHAR_PROFESSION // WON ADD �H��¾�~
        0,              //    PROFESSION_CLASS,      // ¾�~�O
        0,              //    PROFESSION_LEVEL,      // ¾�~����
        //    0,  //    PROFESSION_EXP,        // ¾�~�g���
        0, //    PROFESSION_SKILL_POINT,    // �ޯ��I��
        0, //    ATTACHPILE,          // �W�[���|
        0, //    PROFESSION_FIRE_P,      // �����m��
        0, //    PROFESSION_ICE_P,      // �B���m��
        0, //    PROFESSION_THUNDER_P,    // �p���m��
        0, //    PROFESSION_FIRE_R,      // ���ܩ�
        0, //    PROFESSION_ICE_R,      // �B�ܩ�
        0, //    PROFESSION_THUNDER_R,    // �p�ܩ�
#endif
#ifdef _ALLDOMAN // (���i�}) Syu ADD �Ʀ�]NPC
        0,       //    CHAR_HEROFLOOR,
#endif
#ifdef _PETSKILL_BECOMEPIG
        -1,     //    CHAR_BECOMEPIG,
        100250, // CHAR_BECOMEPIG_BBI
#endif
        0, //    CHAR_LASTLEAVETIME, // Robin add �̫����u�ɶ�

#ifdef _NEW_MANOR_LAW
        0, //    CHAR_MOMENTUM,
#endif

#ifdef _ITEM_ADDEXP2
        0, //    CHAR_ADDEXPPOWER,
        0, //    CHAR_ADDEXPTIME,
#endif

#ifdef _ANGEL_SUMMON
        0, //    CHAR_HEROCNT, // �����i�̥��Ȫ�����
#endif

#ifdef _TEACHER_SYSTEM
        0, // CHAR_TEACHER_FAME,  // ��ʦ�쵼����
#endif

#ifdef _GM_ITEM
        0, // ���GM�������
#endif
#ifdef _VIP_SERVER
        0, // �ػر���
#endif
#ifdef _VIP_RIDE
        0, // ��Ա��ڼ�
        0,
#endif
#ifdef _PET_BEATITUDE
        0,
#endif
#ifdef _RIDE_CF
        0,
#ifdef _ADD_RIDE_CF
        0,
#endif
#endif

#ifdef _ITEM_PET_LOCKED
        0,
#endif

#ifdef _BOUND_TIME
        0,
#endif

#ifdef _SUPER
        0,
#endif

#ifdef _ONLINE_SHOP
        0,
#endif

#ifdef _LOTTERY_SYSTEM
        0,
        0,
#endif

#ifdef _COMPOUND_EXP
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
#endif

#ifdef _ONLINE_TIME
        0,
#endif

#ifdef _CAMEO_MONEY
        0,
#endif

#ifdef _MISSION_TRAIN
        0,
        0,
#endif
#ifdef _PET_EVOLVE
        0,
#endif
#ifdef _FLOOR_DIFI_FIELD
        0,
#endif

#ifdef _ACTIVE_GAME
        0,
#endif

#ifdef _PLAYER_TITLE
        0,
        0,
#endif
#ifdef _MISSION_TIME
        0,
#endif
#ifdef _REGISTRATION_TIME
        0,
        0,
#endif
#ifdef _SPECIAL_COUNTER
        0,
#endif
#ifdef _SPECIAL_TIMER
        0,
#endif
#ifdef _OFFLINE_TIME
        0,
#endif
    },

    {
        {""}, /*  CHAR_NAME   */
        {""}, /*  CHAR_OWNTITLE   */
        {""}, /*  CHAR_ARGUMENT */
        {""}, /*  CHAR_OWNERCDKEY   */
        {""}, /*  CHAR_OWNERCHARANAME   */

        {""},
#ifdef _UNIQUE_P_I
        {""},
#endif

#ifdef _TEACHER_SYSTEM
        {""}, // ��ʦ�ʺ�
        {""}, // ��ʦ����
#endif
#ifdef _ITEM_SETLOVER
        {""}, // ����ж�
        {""}, // �����ʺ�
        {""}, // ��������
#endif

#ifdef _GM_ITEM
        {""}, // ���GM����
#endif
#ifdef _NEW_NAME // �Զ���ƺ�
        {""},
#endif

#ifdef _LOTTERY_SYSTEM
        {""},
#endif

    },
    {
        SETFLG(1, 1, 1, 1, 1, 1, 0, 0),
        SETFLG(0, 0, 0, 0, 0, 0, 0, 1),
    }};

LevelUpPattern lvplayer00 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer10 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer20 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer30 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer01 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer11 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer21 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer31 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer02 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer12 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer22 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer32 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer03 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer13 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer23 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer33 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer04 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer14 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer24 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer34 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer05 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer15 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer25 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer35 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer06 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer16 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer26 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer36 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer07 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer17 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer27 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer37 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer08 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer18 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer28 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer38 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};

LevelUpPattern lvplayer09 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer19 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer29 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
LevelUpPattern lvplayer39 = {{{100, 10}, {200, 10}, {50, 8}}, 9, 11, 10};
