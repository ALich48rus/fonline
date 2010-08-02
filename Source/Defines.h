#ifndef __DEFINES__
#define __DEFINES__

#pragma warning (disable : 4786)

#include "ItemPid.h"
#include <windows.h>
#include <map>
#include <vector>
#include <deque>
#include <set>
#include <string>
using namespace std;

typedef	DWORD MSGTYPE;
union FILETIMELI
{
	FILETIME ft;
	ULARGE_INTEGER ul;
};

typedef map<string,BYTE,less<string>> StrByteMap;
typedef map<string,BYTE,less<string>>::iterator StrByteMapIt;
typedef map<string,BYTE,less<string>>::value_type StrByteMapVal;
typedef map<BYTE,string,less<BYTE>> ByteStrMap;
typedef map<BYTE,string,less<BYTE>>::iterator ByteStrMapIt;
typedef map<BYTE,string,less<BYTE>>::value_type ByteStrMapVal;
typedef map<string,string,less<string>> StrStrMap;
typedef map<string,string,less<string>>::iterator StrStrMapIt;
typedef map<string,string,less<string>>::value_type StrStrMapVal;
typedef map<DWORD,string,less<DWORD>> StringMap;
typedef map<DWORD,string,less<DWORD>>::iterator StringMapIt;
typedef map<DWORD,string,less<DWORD>>::value_type StringMapVal;
typedef multimap<DWORD,string,less<DWORD>> StringMulMap;
typedef multimap<DWORD,string,less<DWORD>>::iterator StringMulMapIt;
typedef multimap<DWORD,string,less<DWORD>>::value_type StringMulMapVal;
typedef set<string> StringSet;
typedef map<string,WORD,less<string>> StrWordMap;
typedef map<string,WORD,less<string>>::iterator StrWordMapIt;
typedef map<string,WORD,less<string>>::value_type StrWordMapVal;
typedef map<string,DWORD,less<string>> StrDWordMap;
typedef map<string,DWORD,less<string>>::iterator StrDWordMapIt;
typedef map<string,DWORD,less<string>>::value_type StrDWordMapVal;
typedef map<WORD,string,less<WORD>> WordStrMap;
typedef map<WORD,string,less<WORD>>::iterator WordStrMapIt;
typedef map<WORD,string,less<WORD>>::value_type WordStrMapVal;
typedef map<string,DWORD,less<string>> StrDwordMap;
typedef map<string,DWORD,less<string>>::iterator StrDwordMapIt;
typedef map<string,DWORD,less<string>>::value_type StrDwordMapVal;

typedef map<WORD,char*,less<WORD>> CharMap;
typedef map<WORD,char*,less<WORD>>::iterator CharMapIt;
typedef map<WORD,char*,less<WORD>>::value_type CharMapVal;
typedef map<WORD,DWORD,less<WORD>> WordMap;
typedef map<WORD,DWORD,less<WORD>>::iterator WordMapIt;
typedef map<WORD,DWORD,less<WORD>>::value_type WordMapVal;
typedef map<DWORD,DWORD,less<DWORD>> DwordMap;
typedef map<DWORD,DWORD,less<DWORD>>::iterator DwordMapIt;
typedef map<DWORD,DWORD,less<DWORD>>::value_type DwordMapVal;
typedef map<int,int,less<int>> IntMap;
typedef map<int,int,less<int>>::iterator IntMapIt;
typedef map<int,int,less<int>>::value_type IntMapVal;

typedef vector<void*> PtrVec;
typedef vector<void*>::iterator PtrVecIt;
typedef vector<int> IntVec;
typedef vector<int>::iterator IntVecIt;
typedef vector<BYTE> ByteVec;
typedef vector<BYTE>::iterator ByteVecIt;
typedef vector<short> ShortVec;
typedef vector<WORD> WordVec;
typedef vector<WORD>::iterator WordVecIt;
typedef vector<DWORD> DwordVec;
typedef vector<DWORD>::iterator DwordVecIt;
typedef vector<char> CharVec;
typedef vector<string> StrVec;
typedef vector<string>::iterator StrVecIt;
typedef vector<char*> PCharVec;
typedef vector<BYTE*> PByteVec;
typedef vector<float> FloatVec;
typedef vector<float>::iterator FloatVecIt;

typedef set<BYTE> ByteSet;
typedef set<BYTE>::iterator ByteSetIt;
typedef set<WORD> WordSet;
typedef set<WORD>::iterator WordSetIt;
typedef set<DWORD> DwordSet;
typedef set<DWORD>::iterator DwordSetIt;
typedef set<int> IntSet;
typedef set<int>::iterator IntSetIt;

typedef deque<DWORD> DwordDeq;

typedef pair<int,int> IntPair;
typedef pair<WORD,WORD> WordPair;
typedef pair<DWORD,DWORD> DwordPair;
typedef pair<char,char> CharPair;
typedef pair<char*,char*> PCharPair;
typedef pair<BYTE,BYTE> BytePair;

typedef vector<WordPair> WordPairVec;
typedef vector<WordPair>::iterator WordPairVecIt;
typedef vector<WordPair>::value_type WordPairVecVal;
typedef vector<IntPair> IntPairVec;
typedef vector<IntPair>::iterator IntPairVecIt;
typedef vector<IntPair>::value_type IntPairVecVal;
typedef vector<DwordPair> DwordPairVec;
typedef vector<DwordPair>::iterator DwordPairVecIt;
typedef vector<DwordPair>::value_type DwordPairVecVal;
typedef vector<PCharPair> PCharPairVec;
typedef vector<PCharPair>::iterator PCharPairVecIt;
typedef vector<PCharPair>::value_type PCharPairVecVal;
typedef vector<BytePair> BytePairVec;
typedef vector<BytePair>::iterator BytePairVecIt;
typedef vector<BytePair>::value_type BytePairVecVal;

//��� ������ � ��������� �����������
#define BIN__N(x)                     (x) | x>>3 | x>>6 | x>>9
#define BIN__B(x)                     (x) & 0xf | (x)>>12 & 0xf0
#define BIN8(v)                       (BIN__B(BIN__N(0x##v)))
#define BIN16(bin16,bin8)             ((BIN8(bin16)<<8)|(BIN8(bin8)))
#define BIN32(bin32,bin24,bin16,bin8) ((BIN8(bin32)<<24)|(BIN8(bin24)<<16)|(BIN8(bin16)<<8)|(BIN8(bin8)))

//��� ������ � ������ (�� ���� - �������)
#define FLAG(x,y)        (((x)&(y))!=0)
#define FLAGS(x,y)       (((x)&(y))==y)
#define SETFLAG(x,y)     ((x)=(x)|(y))
#define UNSETFLAG(x,y)   ((x)=((x)|(y))^(y))

//�������� ������
#define ABS(x) ((x)>=0?(x):-(x))

//��� ��������
#define BREAK_BEGIN do{
#define BREAK_END   }while(0)

#define MAKEDWORD(a,b,c,d) ((DWORD)(BYTE)(d) | ((DWORD)(BYTE)(c) << 8) | ((DWORD)(BYTE)(b) << 16) | ((DWORD)(BYTE)(a) << 24 ))

//#define CONVERT_GRAMM(lbs) (UINT(float(lbs)*0.4535924f*1000))
#define CONVERT_GRAMM(x) ((x)*453) //373?

#define MAX_BYTE  (0xFF)
#define MAX_WORD  (0xFFFF)
#define MAX_DWORD (0xFFFFFFFF)
#define MAX_INT   (0x7FFFFFFF)
#define MIN_INT   (0x80000000)

#define CLAMP(x,low,high) (((x)>(high))?(high):(((x)<(low))?(low):(x)))
//#define DECLARE_COMPARE_PREDICATE(name,type,comp) class name{public:bool operator()(const type l, const type r)const{return comp;}}

/************************************************************************/
/* Special item pids                                                    */
/************************************************************************/

#define SP_SCEN_LIGHT				(F2PROTO_OFFSET_SCENERY+141) //Light Source
#define SP_SCEN_LIGHT_STOP          (4592)
#define SP_SCEN_BLOCK				(F2PROTO_OFFSET_SCENERY+67) //Secret Blocking Hex
#define SP_SCEN_IBLOCK				(F2PROTO_OFFSET_SCENERY+344) //Block Hex Auto Inviso
#define SP_SCEN_TRIGGER				(3852)
#define SP_WALL_BLOCK_LIGHT			(F2PROTO_OFFSET_WALL+621) //Wall s.t. with light
#define SP_WALL_BLOCK				(F2PROTO_OFFSET_WALL+622) //Wall s.t.
#define SP_GRID_EXITGRID			(F2PROTO_OFFSET_SCENERY+49) //Exit Grid Map Marker
#define SP_GRID_ENTIRE				(3853)
#define SP_MISC_SCRBLOCK			(F2PROTO_OFFSET_MISC+12) //Scroll block
#define SP_MISC_GRID_MAP_BEG		(F2PROTO_OFFSET_MISC+16) //Grid to LocalMap begin
#define SP_MISC_GRID_MAP_END		(F2PROTO_OFFSET_MISC+23) //Grid to LocalMap end
#define SP_MISC_GRID_MAP(pid)		((pid)>=SP_MISC_GRID_MAP_BEG && (pid)<=SP_MISC_GRID_MAP_END)
#define SP_MISC_GRID_GM_BEG			(F2PROTO_OFFSET_MISC+31) //Grid to GlobalMap begin
#define SP_MISC_GRID_GM_END			(F2PROTO_OFFSET_MISC+46) //Grid to GlobalMap end
#define SP_MISC_GRID_GM(pid)		((pid)>=SP_MISC_GRID_GM_BEG && (pid)<=SP_MISC_GRID_GM_END)

#define WORLD_SAVE_V1           (0x01AB0F01)
#define WORLD_SAVE_V2           (0x01AB0F02)
#define WORLD_SAVE_V3           (0x01AB0F03)
#define WORLD_SAVE_V4           (0x01AB0F04)
#define WORLD_SAVE_V5           (0x01AB0F05)
#define WORLD_SAVE_V6           (0x01AB0F06)
#define WORLD_SAVE_V7           (0x01AB0F07)
#define WORLD_SAVE_V8           (0x01AB0F08)
#define WORLD_SAVE_LAST         WORLD_SAVE_V8

#define MAX_FOPATH              (1024)
#define CRAFT_SEND_TIME         (60000)
#define LEXEMS_SIZE             (128)
#define MAX_HOLO_INFO           (250)
#define MAX_PARAMETERS_ARRAYS   (100)
#define AMBIENT_SOUND_TIME      (60000) // Random(X/2,X);

#define GENDER_MALE             (0)
#define GENDER_FEMALE           (1)
#define GENDER_IT               (2)
#define AGE_MAX                 (60)
#define AGE_MIN                 (14)
#define CRITICALS_ROLLS         (6)

#define AGGRESSOR_TICK          (60000)
#define WORLD_START_TIME        "07:00 30:10:2246 x00"
#define MAX_ENEMY_STACK         (30)

// Items
#define MAX_ADDED_NOGROUP_ITEMS (30)
#define ITEM_SLOT_BEGIN         (1000)
#define ITEM_SLOT_END           (1099)
#define ITEM_DEF_SLOT           (1000)
#define ITEM_DEF_ARMOR          (1100)
#define UNARMED_PUNCH           (1000)
#define UNARMED_KICK            (1020)

// Maps
#define TIME_CAN_FOLLOW_GM      (20000) //Can less than Map timeout
#define MAX_RADIUS              (3)

// Critter find types
#define FIND_LIFE               (0x01)
#define FIND_KO                 (0x02)
#define FIND_DEAD               (0x04)
#define FIND_ONLY_PLAYERS       (0x10)
#define FIND_ONLY_NPC           (0x20)
#define FIND_ALL                (0x0F)

#define LOOT_DIST				(1)
#define PUSH_DIST				(1)
#define STEAL_EXP_TICK          (30000)

// Proto maps
#define MAP_PROTO_EXT			".map"
#define MAX_PROTO_MAPS          (10000)

// Type entires
#define ENTIRE_DEFAULT			(0)
#define ENTIRE_LOG_OUT			(241)

// Sendmap info
#define SENDMAP_TILES           BIN8(00000001)
#define SENDMAP_WALLS           BIN8(00000010)
#define SENDMAP_SCENERY         BIN8(00000100)

// Ping
#define PING_PING               (0)
#define PING_WAIT               (1)
#define PING_CLIENT             (2)
#define PING_UID_FAIL           (3)

// Say types
#define SAY_NORM                (1)
#define SAY_NORM_ON_HEAD        (2)
#define SAY_SHOUT               (3)
#define SAY_SHOUT_ON_HEAD       (4)
#define SAY_EMOTE               (5)
#define SAY_EMOTE_ON_HEAD       (6)
#define SAY_WHISP               (7)
#define SAY_WHISP_ON_HEAD       (8)
#define SAY_SOCIAL              (9)
#define SAY_RADIO               (10)
#define SAY_NETMSG              (11)
#define SAY_DIALOG              (12)
#define SAY_APPEND              (13)
#define SAY_ENCOUNTER_ANY       (14)
#define SAY_ENCOUNTER_RT        (15)
#define SAY_ENCOUNTER_TB        (16)
#define SAY_FIX_RESULT          (17)
#define SAY_DIALOGBOX_TEXT      (18)
#define SAY_DIALOGBOX_BUTTON(b) (19+(b)) // Max 20 buttons (0..19)
#define SAY_SAY_TITLE           (39)
#define SAY_SAY_TEXT            (40)
#define SAY_FLASH_WINDOW        (41)

#define MAX_DLGBOX_BUTTONS       (20)

// Transfer types
#define TRANSFER_CLOSE          (0)
#define TRANSFER_HEX_CONT_UP    (1)
#define TRANSFER_HEX_CONT_DOWN  (2)
#define TRANSFER_SELF_CONT      (3)
#define TRANSFER_CRIT_LOOT      (4)
#define TRANSFER_CRIT_STEAL     (5)
#define TRANSFER_CRIT_BARTER    (6)
#define TRANSFER_FAR_CONT       (7)
#define TRANSFER_FAR_CRIT       (8)

// Take flags
#define CONT_GET                (1)
#define CONT_PUT                (2)
#define CONT_GETALL             (3)
#define CONT_PUTALL             (4)
//#define CONT_UNLOAD             (5) //TODO:

// Target types
#define TARGET_SELF             (0)
#define TARGET_SELF_ITEM        (1)
#define TARGET_CRITTER          (2)
#define TARGET_ITEM             (3)
#define TARGET_SCENERY          (4)

// Pick types
#define PICK_CRIT_LOOT          (0)
#define PICK_CRIT_PUSH          (1)

// Craft results
#define CRAFT_RESULT_NONE       (0)
#define CRAFT_RESULT_SUCC       (1)
#define CRAFT_RESULT_FAIL       (2)
#define CRAFT_RESULT_TIMEOUT    (3)

// Critters
#define CRITTER_INV_VOLUME      (1000)

// Locker
#define LOCKER_ISOPEN           (0x01)
#define LOCKER_LOCKED           (0x02)
#define LOCKER_JAMMED           (0x04)
#define LOCKER_BROKEN           (0x08)
#define LOCKER_NOOPEN           (0x10)
#define LOCKER_ELECTRO          (0x20)

// Hit locations
#define HIT_LOCATION_NONE       (0)
#define HIT_LOCATION_HEAD       (1)
#define HIT_LOCATION_LEFT_ARM   (2)
#define HIT_LOCATION_RIGHT_ARM  (3)
#define HIT_LOCATION_TORSO      (4)
#define HIT_LOCATION_RIGHT_LEG  (5)
#define HIT_LOCATION_LEFT_LEG   (6)
#define HIT_LOCATION_EYES       (7)
#define HIT_LOCATION_GROIN      (8)
#define HIT_LOCATION_UNCALLED   (9)
#define MAX_HIT_LOCATION        (10)

// Locations
#define MAX_PROTO_LOCATIONS     (10000)
#define MAX_MAPS_IN_LOCATION    (50)

// Global map
#define GM_MAXX                 (GameOpt.GlobalMapWidth*GameOpt.GlobalMapZoneLength)
#define GM_MAXY                 (GameOpt.GlobalMapHeight*GameOpt.GlobalMapZoneLength)
#define GM_ZONE_LEN             (GameOpt.GlobalMapZoneLength) //������ ���� ������ GM_MAXX � GM_MAXY
#define GM__MAXZONEX            (100)
#define GM__MAXZONEY            (100)
#define GM_ZONES_FOG_SIZE       (((GM__MAXZONEX/4)+((GM__MAXZONEX%4)?1:0))*GM__MAXZONEY)
#define GM_FOG_FULL             (0)
#define GM_FOG_SELF             (1)
#define GM_FOG_SELF2            (2)
#define GM_FOG_NONE             (3)
#define GM_MAX_GROUP_COUNT      (GameOpt.GlobalMapMaxGroupCount)
#define GM_MOVE_PROC_TIME       (500)
#define GM_ANSWER_WAIT_TIME     (20000)
#define GM_LIGHT_TIME           (5000)
#define GM_ZONE(x)              ((x)/GM_ZONE_LEN)
#define GM_ENTRANCES_SEND_TIME  (60000)
#define GM_TRACE_TIME           (1000)

// Follow
#define FOLLOW_DIST				(10)
#define FOLLOW_FORCE            (1)
#define FOLLOW_PREP             (2)

// GM Info
#define GM_INFO_LOCATIONS       (0x01)
#define GM_INFO_CRITTERS        (0x02)
#define GM_INFO_GROUP_PARAM     (0x04)
#define GM_INFO_ZONES_FOG       (0x08)
#define GM_INFO_ALL             (0x0F)
#define GM_INFO_FOG             (0x10)
#define GM_INFO_LOCATION        (0x20)

// Global process types
#define GLOBAL_PROCESS_MOVE         (0)
#define GLOBAL_PROCESS_ENTER        (1)
#define GLOBAL_PROCESS_START_FAST   (2)
#define GLOBAL_PROCESS_START        (3)
#define GLOBAL_PROCESS_SET_MOVE     (4)
#define GLOBAL_PROCESS_STOPPED      (5)
#define GLOBAL_PROCESS_NPC_IDLE     (6)
#define GLOBAL_PROCESS_NEW_ZONE     (7)

// GM Rule command
#define GM_CMD_SETMOVE     (1) //+r-a*x,y//���������
#define GM_CMD_STOP        (2) //+r-a//������������
#define GM_CMD_TOLOCAL     (3) //+r-a*num_city,num_map//������� �� �������� �����
#define GM_CMD_KICKCRIT    (4) //+r-a*cr_id//�������� ����� �� ������
#define GM_CMD_FOLLOW_CRIT (5) //+r+a*cr_id//������� � ������ �����
#define GM_CMD_FOLLOW      (6) //
#define GM_CMD_GIVE_RULE   (7) //
#define GM_CMD_ANSWER      (8)
#define GM_CMD_ENTRANCES   (9)
#define GM_CMD_VIEW_MAP    (10)

// GM Walk types
#define GM_WALK_GROUND          (0)
#define GM_WALK_FLY             (1)
#define GM_WALK_WATER           (2)

// Flags Hex
	// Static
#define FH_BLOCK                       BIN8(00000001)
#define FH_NOTRAKE                     BIN8(00000010)
#define FH_WALL                        BIN8(00000100)
#define FH_SCEN                        BIN8(00001000)
#define FH_SCEN_GRID                   BIN8(00010000)
#define FH_TRIGGER                     BIN8(00100000)
	// Dynamic
#define FH_CRITTER            BIN8(00000001)
#define FH_DEAD_CRITTER       BIN8(00000010)
#define FH_ITEM               BIN8(00000100)
#define FH_DOOR               BIN8(00001000)
#define FH_BLOCK_ITEM         BIN8(00010000)
#define FH_NRAKE_ITEM         BIN8(00100000)
#define FH_WALK_ITEM          BIN8(01000000)
#define FH_GAG_ITEM           BIN8(10000000)

#define FH_NOWAY             BIN16(00010001,00000001)
#define FH_NOSHOOT           BIN16(00100000,00000010)

// Client map
#define SERVER_MAP_EXT			".map"
#define CLIENT_MAP_FORMAT_VER	(6)

// Coordinates
#define MAXHEX_DEF              (200)
#define MAXHEX_MIN              (10)
#define MAXHEX_MAX              (60000)
#define HEX_POS(hx,hy)          ((hy)*MAXHEX_MAX*3+(hx)*3) // Todo: bad precision on big values
#define HEX_X_POS(pos)          ((pos)%(MAXHEX_MAX*3)/3)
#define HEX_Y_POS(pos)          ((pos)/(MAXHEX_MAX*3))

// Client parameters
#define MAX_NAME				(30)
#define MIN_NAME				(1)
#define MAX_NET_TEXT			(100)
#define MAX_SAY_NPC_TEXT		(25)
#define MAX_SCENERY				(5000)
#define MAX_DIALOG_TEXT			(MAX_FOTEXT)
#define MAX_DLG_LEN_IN_BYTES	(64*1024)
#define MAX_DLG_LEXEMS_TEXT     (1000)
#define MAX_BUF_LEN				(4096)

// Critters
#define MAX_CRIT_TYPES          (1000)
#define NPC_START_ID            (5000001)
#define USERS_START_ID          (1)
#define IS_USER_ID(id)          ((id)>0 && (id)<NPC_START_ID)
#define IS_NPC_ID(id)           ((id)>=NPC_START_ID)
#define CRIT_HEAL_TIME          (2*60*1000)
#define CLIENT_KICK_TIME        (3*60*1000)
#define MAX_ANSWERS				(100)
#define PROCESS_TALK_TICK       (1000)
#define DIALOG_BARTER_MIN_TIME  (30000)
#define DIALOG_TALK_MIN_TIME    (30000)
#define DIALOGS_LST_NAME		"dialogs.lst"
#define MAX_SCRIPT_NAME			(64)
#define SCRIPTS_LST             "scripts.cfg"
#define MAX_START_SPECIAL		(40)
#define TURN_BASED_TIMEOUT      (GameOpt.Breaktime)

#define RESPOWN_TIME_PLAYER     (3)
#define RESPOWN_TIME_NPC        (120)
#define RESPOWN_TIME_INFINITY   (4*24*60*60000)

// Combat modes
#define COMBAT_MODE_ANY         (0)
#define COMBAT_MODE_REAL_TIME   (1)
#define COMBAT_MODE_TURN_BASED  (2)

// Turn based
#define COMBAT_TB_END_TURN      (0)
#define COMBAT_TB_END_COMBAT    (1)

// Answer
#define ANSWER_BEGIN            (0xF0)
#define ANSWER_END              (0xF1)
#define ANSWER_BARTER           (0xF2)

// Time AP
#define AP_DIVIDER              (100)

// Crit conditions
#define COND_LIFE               (1)
#define COND_LIFE_NONE           (1)
#define COND_KNOCKOUT           (2)
#define COND_KNOCKOUT_FRONT      (1)
#define COND_KNOCKOUT_BACK       (2)
#define COND_DEAD               (3)
#define COND_DEAD_FRONT          (1)
#define COND_DEAD_BACK           (2)
#define COND_DEAD_BURST          (3)
#define COND_DEAD_BLOODY_SINGLE  (4)
#define COND_DEAD_BLOODY_BURST   (5)
#define COND_DEAD_PULSE          (6)
#define COND_DEAD_PULSE_DUST     (7)
#define COND_DEAD_LASER          (8)
#define COND_DEAD_EXPLODE        (9)
#define COND_DEAD_FUSED          (10)
#define COND_DEAD_BURN           (11)
#define COND_DEAD_BURN2          (12)
#define COND_DEAD_BURN_RUN       (13)
#define COND_NOT_IN_GAME        (4)

// Other
#define MIN_VISIBLE_CRIT        (6)
#define MIN_HIT                 (5)
#define MAX_HIT                 (95)
#define FADING_PERIOD           (1000)

// Run-time critters flags
#define FCRIT_PLAYER            (0x00010000) // Player
#define FCRIT_NPC               (0x00020000) // Npc
#define FCRIT_DISCONNECT        (0x00080000) // In offline
#define FCRIT_CHOSEN            (0x00100000) // Chosen
#define FCRIT_RULEGROUP         (0x00200000) // Group rule

// Slots
#define SLOT_INV                (0)
#define SLOT_HAND1              (1)
#define SLOT_HAND2              (2)
#define SLOT_ARMOR              (3)
#define SLOT_GROUND             (255)

// Body Types
// Slot protos offsets:
#define SLOT_MAIN_PROTO_OFFSET	(1000)
#define SLOT_EXT_PROTO_OFFSET	(1030)
#define SLOT_ARMOR_PROTO_OFFSET	(1060)
//1000 - 1100 protos reserved

// Players barter
#define BARTER_DIST             (1)
	// Types
#define BARTER_TRY              (0) //opponentId, isHide
#define BARTER_ACCEPTED         (1) //opponentId, isHide
#define BARTER_BEGIN            (2)
#define BARTER_END              (3)
#define BARTER_SET_SELF         (4)
#define BARTER_SET_OPPONENT     (5)
#define BARTER_UNSET_SELF       (6)
#define BARTER_UNSET_OPPONENT   (7)
#define BARTER_OFFER            (8) //isSet, isOpponent
#define BARTER_REFRESH          (9)

// Scores
#define SCORE_EVIL_OF_HOUR      (0) //Hour
#define SCORE_HERO_OF_HOUR      (1)
#define SCORE_KARMA_ON_HOUR     (2)
#define SCORE_SPEAKER           (3) //+ //Best
#define SCORE_TRADER            (4) //+
#define SCORE_ZOMBY             (5) //+
#define SCORE_PATY              (6)
#define SCORE_MANIAC            (7) //+
#define SCORE_SCAUT             (8) //+
#define SCORE_DOCTOR            (9) //+
#define SCORE_SHOOTER           (10) //+
#define SCORE_MELEE             (11) //+
#define SCORE_UNARMED           (12) //+
#define SCORE_THIEF             (13) //+
#define SCORE_DRIVER            (14) //+ //Profs
#define SCORE_KILLER            (15) //+
#define SCORE_SNIPER            (16) //+
#define SCORE_ADVENTURER        (17)
#define SCORE_CRACKER           (18) //+
#define SCORE_UNARMED_DAMAGE    (19) //+
#define SCORE_RITCH             (20)
#define SCORE_CHOSEN_ONE        (21) //Best of the best
#define SCORE_SIERRA_CUR        (40) //+
#define SCORE_MARIPOSA_CUR      (41) //+
#define SCORE_CATHEDRAL_CUR     (42) //+
#define SCORE_SIERRA_BEST       (43) //+
#define SCORE_MARIPOSA_BEST     (44) //+
#define SCORE_CATHEDRAL_BEST    (45) //+
#define SCORE_SIERRA_ORG        (46) //+
#define SCORE_MARIPOSA_ORG      (47) //+
#define SCORE_CATHEDRAL_ORG     (48) //+
#define SCORE_BASE_BEST_ORG     (49)
#define SCORES_MAX              (50)
#define SCORES_SEND_TIME        (60000)
#define SCORE_NAME_LEN          (64)

struct ScoreType
{
	DWORD ClientId;
	char ClientName[SCORE_NAME_LEN];
	int Value;
};

// Show screen modes
// Ouput: it is 'uint param' in Critter::ShowScreen.
// Input: I - integer value 'uint answerI', S - string value 'string& answerS' in 'answer_' function.
#define SHOW_SCREEN_CLOSE                (0)  // Close top window.
#define SHOW_SCREEN_TIMER                (1)  // Timer box. Output: picture index in INVEN.LST. Input I: time in game minutes (1..599).
#define SHOW_SCREEN_DIALOGBOX            (2)  // Dialog box. Output: buttons count - 0..20 (exit button added automatically). Input I: Choosed button - 0..19.
#define SHOW_SCREEN_SKILLBOX             (3)  // Skill box. Input I: selected skill.
#define SHOW_SCREEN_BAG                  (4)  // Bag box. Input I: id of selected item.
#define SHOW_SCREEN_SAY                  (5)  // Say box. Output: all symbols - 0 or only numbers - any other number. Input S: typed string.
#define SHOW_ELEVATOR                    (6)  // Elevator. Output: look ELEVATOR_* macro. Input I: Choosed level button.
#define SHOW_SCREEN_INVENTORY            (7)  // Inventory.
#define SHOW_SCREEN_CHARACTER            (8)  // Character.
#define SHOW_SCREEN_FIXBOY               (9)  // Fix-boy.
#define SHOW_SCREEN_PIPBOY               (10) // Pip-boy.
#define SHOW_SCREEN_MINIMAP              (11) // Mini-map.

// Item perks
#define WEAPON_PERK_FAST_RELOAD     (6)

// Parameters
#define MAX_PARAMS                  (1000)
#define SKILL_OFFSET(skill)         ((skill)-(GameOpt.AbsoluteOffsets?0:SKILL_BEGIN))
#define PERK_OFFSET(skill)          ((skill)-(GameOpt.AbsoluteOffsets?0:PERK_BEGIN))

// Stats
#define ST_STRENGTH                 (0)
#define ST_PERCEPTION               (1)
#define ST_ENDURANCE                (2)
#define ST_CHARISMA                 (3)
#define ST_INTELLECT                (4)
#define ST_AGILITY                  (5)
#define ST_LUCK                     (6)
#define ST_MAX_LIFE                 (7)
#define ST_ACTION_POINTS            (8)
#define ST_ARMOR_CLASS              (9)
#define ST_MELEE_DAMAGE             (10)
#define ST_CARRY_WEIGHT             (11)
#define ST_SEQUENCE                 (12)
#define ST_HEALING_RATE             (13)
#define ST_CRITICAL_CHANCE          (14)
#define ST_MAX_CRITICAL             (15)
#define ST_NORMAL_ABSORB            (16)
#define ST_LASER_ABSORB             (17)
#define ST_FIRE_ABSORB              (18)
#define ST_PLASMA_ABSORB            (19)
#define ST_ELECTRO_ABSORB           (20)
#define ST_EMP_ABSORB               (21)
#define ST_EXPLODE_ABSORB           (22)
#define ST_NORMAL_RESIST            (23)
#define ST_LASER_RESIST             (24)
#define ST_FIRE_RESIST              (25)
#define ST_PLASMA_RESIST            (26)
#define ST_ELECTRO_RESIST           (27)
#define ST_EMP_RESIST               (28)
#define ST_EXPLODE_RESIST           (29)
#define ST_RADIATION_RESISTANCE     (30)
#define ST_POISON_RESISTANCE        (31)
#define ST_STRENGTH_EXT             (32)
#define ST_PERCEPTION_EXT           (33)
#define ST_ENDURANCE_EXT            (34)
#define ST_CHARISMA_EXT             (35)
#define ST_INTELLECT_EXT            (36)
#define ST_AGILITY_EXT              (37)
#define ST_LUCK_EXT                 (38)
#define ST_MAX_LIFE_EXT             (39)
#define ST_ACTION_POINTS_EXT        (40)
#define ST_ARMOR_CLASS_EXT          (41)
#define ST_MELEE_DAMAGE_EXT         (42)
#define ST_CARRY_WEIGHT_EXT         (43)
#define ST_SEQUENCE_EXT             (44)
#define ST_HEALING_RATE_EXT         (45)
#define ST_CRITICAL_CHANCE_EXT      (46)
#define ST_MAX_CRITICAL_EXT         (47)
#define ST_NORMAL_ABSORB_EXT        (48)
#define ST_LASER_ABSORB_EXT         (49)
#define ST_FIRE_ABSORB_EXT          (50)
#define ST_PLASMA_ABSORB_EXT        (51)
#define ST_ELECTRO_ABSORB_EXT       (52)
#define ST_EMP_ABSORB_EXT           (53)
#define ST_EXPLODE_ABSORB_EXT       (54)
#define ST_NORMAL_RESIST_EXT        (55)
#define ST_LASER_RESIST_EXT         (56)
#define ST_FIRE_RESIST_EXT          (57)
#define ST_PLASMA_RESIST_EXT        (58)
#define ST_ELECTRO_RESIST_EXT       (59)
#define ST_EMP_RESIST_EXT           (60)
#define ST_EXPLODE_RESIST_EXT       (61)
#define ST_RADIATION_RESISTANCE_EXT (62)
#define ST_POISON_RESISTANCE_EXT    (63)
#define ST_AGE                      (70)
#define ST_GENDER					(71)
#define ST_CURRENT_HP				(72)
#define ST_POISONING_LEVEL          (73)
#define ST_RADIATION_LEVEL          (74)
#define ST_CURRENT_AP				(75)
#define ST_EXPERIENCE				(76)
#define ST_LEVEL                    (77)
#define ST_UNSPENT_SKILL_POINTS     (78)
#define ST_UNSPENT_PERKS            (79)
#define ST_KARMA                    (80)
#define ST_FOLLOW_CRIT              (81)
#define ST_REPLICATION_MONEY        (82)
#define ST_REPLICATION_COUNT        (83)
#define ST_REPLICATION_TIME         (84)
#define ST_REPLICATION_COST         (85)
#define ST_TURN_BASED_AC            (86)
#define ST_MAX_MOVE_AP              (87)
#define ST_MOVE_AP                  (88)
#define ST_NPC_ROLE                 (89)
#define ST_BONUS_LOOK               (101)
#define ST_RATE_ITEM                (102)
#define ST_FREE_BARTER_PLAYER       (103)
#define ST_DIALOG_ID                (104)
#define ST_AI_ID                    (105)
#define ST_TEAM_ID                  (106)
#define ST_BAG_ID                   (107)
#define ST_LAST_WEAPON_ID           (110)
#define ST_BASE_CRTYPE              (112)
#define ST_TALK_DISTANCE            (115)
#define ST_ANIM3D_LAYER_BEGIN       (150)
#define ST_ANIM3D_LAYER_END         (179)

// Skills
#define SKILL_BEGIN                 (GameOpt.SkillBegin)
#define SKILL_END                   (GameOpt.SkillEnd)
#define SKILL_COUNT                 (SKILL_END-SKILL_BEGIN+1)
#define MAX_SKILL_VAL               (300)
#define SK_SMALL_GUNS               (200)
#define SK_BIG_GUNS                 (201)
#define SK_ENERGY_WEAPONS           (202)
#define SK_UNARMED                  (203)
#define SK_MELEE_WEAPONS            (204)
#define SK_THROWING                 (205)
#define SK_FIRST_AID                (206)
#define SK_DOCTOR                   (207)
#define SK_SNEAK                    (208)
#define SK_LOCKPICK                 (209)
#define SK_STEAL                    (210)
#define SK_TRAPS                    (211)
#define SK_SCIENCE                  (212)
#define SK_REPAIR                   (213)
#define SK_SPEECH                   (214)
#define SK_BARTER                   (215)
#define SK_OUTDOORSMAN              (217)

// Tag skills
#define TAG_SKILL1                  (226)
#define TAG_SKILL2                  (227)
#define TAG_SKILL3                  (228)
#define TAG_SKILL4                  (229)

// Timeouts
#define TIMEOUT_BEGIN               (GameOpt.TimeoutBegin)
#define TIMEOUT_END                 (GameOpt.TimeoutEnd)
#define TO_SK_REPAIR                (232)
#define TO_SK_SCIENCE               (233)
#define TO_BATTLE                   (238)
#define TO_TRANSFER                 (239)
#define TO_REMOVE_FROM_GAME         (240)
#define TO_KARMA_VOTING             (242)
#define TB_BATTLE_TIMEOUT           (10000000)
#define TB_BATTLE_TIMEOUT_CHECK(to) ((to)>100000)

// Kills
#define KILL_BEGIN                  (GameOpt.KillBegin)
#define KILL_END                    (GameOpt.KillEnd)

// Perks
#define PERK_BEGIN                  (GameOpt.PerkBegin)
#define PERK_END                    (GameOpt.PerkEnd)
#define PERK_COUNT                  (PERK_END-PERK_BEGIN+1)
#define PE_BONUS_HTH_ATTACKS        (302)
#define PE_BONUS_RATE_OF_FIRE       (306)
#define PE_SILENT_RUNNING           (316)
#define PE_MASTER_TRADER            (318)
#define PE_HEAVE_HO                 (336)
#define PE_PATHFINDER               (344)
#define PE_SCOUT                    (346)
#define PE_QUICK_POCKETS            (349)
#define PE_SMOOTH_TALKER            (350)
#define PE_ADRENALINE_RUSH          (380)
#define PE_HTH_EVADE                (394)

// Addiction
#define ADDICTION_BEGIN             (GameOpt.AddictionBegin)
#define ADDICTION_END               (GameOpt.AddictionEnd)

// Karma
#define KARMA_BEGIN                 (GameOpt.KarmaBegin)
#define KARMA_END                   (GameOpt.KarmaEnd)

// Damages
#define DAMAGE_BEGIN                (GameOpt.DamageBegin)
#define DAMAGE_END                  (GameOpt.DamageEnd)
#define DAMAGE_POISONED             (500)
#define DAMAGE_RADIATED             (501)
#define DAMAGE_EYE                  (502)
#define DAMAGE_RIGHT_ARM            (503)
#define DAMAGE_LEFT_ARM             (504)
#define DAMAGE_RIGHT_LEG            (505)
#define DAMAGE_LEFT_LEG             (506)

// Modes
#define MODE_HIDE                   (510)
#define MODE_NO_STEAL               (511)
#define MODE_NO_BARTER              (512)
#define MODE_NO_ENEMY_STACK         (513)
#define MODE_NO_PVP                 (514)
#define MODE_END_COMBAT             (515)
#define MODE_DEFAULT_COMBAT         (516)
#define MODE_NO_HOME                (517)
#define MODE_GECK                   (518)
#define MODE_NO_FAVORITE_ITEM       (519)
#define MODE_NO_ITEM_GARBAGER       (520)
#define MODE_DLG_SCRIPT_BARTER      (521)
#define MODE_UNLIMITED_AMMO         (522)
#define MODE_NO_HEAL                (526)
#define MODE_INVULNERABLE           (527)
#define MODE_NO_FLATTEN             (528) // Client
#define MODE_RANGE_HTH              (530)
#define MODE_NO_LOOT                (532)
#define MODE_NO_PUSH                (536)
#define MODE_NO_UNARMED             (537)

// Traits
#define TRAIT_BEGIN                 (GameOpt.TraitBegin)
#define TRAIT_END                   (GameOpt.TraitEnd)
#define TRAIT_COUNT                 (TRAIT_END-TRAIT_BEGIN+1)
#define TRAIT_SMALL_FRAME           (552)
#define TRAIT_FAST_SHOT             (557)
#define TRAIT_SEX_APPEAL            (563)
#define TRAIT_NIGHT_PERSON          (565)

// Reputation
#define REPUTATION_BEGIN            (GameOpt.ReputationBegin)
#define REPUTATION_END              (GameOpt.ReputationEnd)

// Special send params
#define OTHER_BREAK_TIME            (0+MAX_PARAMS)
#define OTHER_WAIT_TIME             (1+MAX_PARAMS)
#define OTHER_FLAGS                 (2+MAX_PARAMS)
#define OTHER_BASE_TYPE             (3+MAX_PARAMS)
#define OTHER_YOU_TURN              (4+MAX_PARAMS)
#define OTHER_CLEAR_MAP             (5+MAX_PARAMS)
#define OTHER_TELEPORT              (6+MAX_PARAMS)

// Critter actions
// Flags for chosen:
// l - hardcoded local call
// s - hardcoded server call
// for all others critters actions call only server
//                                          flags    actionExt                                                      item
#define ACTION_MOVE                 (0)  // l
#define ACTION_RUN                  (1)  // l
#define ACTION_SHOW_ITEM            (2)  // l s      prev slot                                                      +
#define ACTION_HIDE_ITEM            (3)  // l s      prev slot                                                      +
#define ACTION_MOVE_ITEM            (4)  // l s      prev slot                                                      +
#define ACTION_USE_ITEM             (5)  // l s                                                                     +
#define ACTION_DROP_ITEM            (6)  // l s      prev slot                                                      +
#define ACTION_USE_WEAPON           (7)  // l        fail attack 8 bit, use index (0-2) 4-7 bits, aim 0-3 bits      +
#define ACTION_RELOAD_WEAPON        (8)  // l s                                                                     +
#define ACTION_USE_SKILL            (9)  // l s      skill index (see SK_*)
#define ACTION_PICK_ITEM            (10) // l s                                                                     +
#define ACTION_PICK_CRITTER         (11) // l        0 - loot, 1 - steal, 2 - push
#define ACTION_OPERATE_CONTAINER    (12) // l s      transfer type * 10 + [0 - get, 1 - get all, 2 - put]           + (exclude get all)
#define ACTION_BARTER               (13) //   s      0 - item taken, 1 - item given                                 +
#define ACTION_DODGE                (14) //          0 - front, 1 - back
#define ACTION_DAMAGE               (15) //          0 - front, 1 - back
#define ACTION_KNOCKOUT             (16) //   s      0 - front, 1 - back
#define ACTION_STANDUP              (17) //   s
#define ACTION_FIDGET               (18) // l
#define ACTION_DEAD                 (19) //   s      dead type (see COND_DEAD_*)
#define ACTION_CONNECT              (20) //
#define ACTION_DISCONNECT           (21) //
#define ACTION_RESPAWN              (22) //   s
#define ACTION_REFRESH              (23) //   s

/************************************************************************/
/* Script defines                                                       */
/************************************************************************/

// Look checks
#define LOOK_CHECK_DIR              (0x01)
#define LOOK_CHECK_SNEAK_DIR        (0x02)
#define LOOK_CHECK_SNEAK_WEIGHT     (0x04)
#define LOOK_CHECK_TRACE            (0x08)
#define LOOK_CHECK_SCRIPT           (0x10)

// In SendMessage
#define MESSAGE_TO_VISIBLE_ME       (0) //�������� ��������� ���� ��� ����� ��������.
#define MESSAGE_TO_IAM_VISIBLE      (1) //�������� ��������� ���� ���� ����� �������.
#define MESSAGE_TO_ALL_ON_MAP       (2) //�������� ���� �� �����.

// Special skill values
#define SKILL_PICK_ON_GROUND       (-1)
#define SKILL_PUT_CONT             (-2)
#define SKILL_TAKE_CONT            (-3)
#define SKILL_TAKE_ALL_CONT        (-4)
#define SKILL_LOOT_CRITTER         (-5)
#define SKILL_PUSH_CRITTER         (-6)

/************************************************************************/
/* ScenToSend                                                           */
/************************************************************************/

struct ScenToSend // 32 bytes
{
	WORD ProtoId;
	BYTE Flags;
	BYTE Reserved0;
	WORD MapX;
	WORD MapY;
	short OffsetX;
	short OffsetY;
	DWORD LightColor;
	BYTE LightDistance;
	BYTE LightFlags;
	char LightIntensity;
	BYTE InfoOffset;
	BYTE AnimStayBegin;
	BYTE AnimStayEnd;
	WORD AnimWait;
	DWORD PicMapHash;
	short Dir;
	WORD Reserved1;
};
typedef vector<ScenToSend> ScenToSendVec;

// Flags
#define SCEN_CAN_USE                (0x01)
#define SCEN_CAN_TALK               (0x02)

/************************************************************************/
/* Animations                                                           */
/************************************************************************/

// BA - � �������� (������ �����) 
// BB - � �������� (������ ������) 
// BC - � �������� (������ ����� - ������ HFCMBT) 
// BD - ���� (� ������ Bloody Mess - �������� ����� ��������) BloodySingle
// BE - ���� (������� �� ������)                              Burn
// BF - ���� (�������� ������)                                BloodyBurst
// BG - ���� (�� ��������)                                    Burst
// BH - ���� (���������� �������)                             Pulse
// BI - ���� (�������� ������� - ���������)                   Laser
// BJ - ���� (������� �� ������)                              Burn
// BK - ���� (���������� ������� - ����� �����)               PulseDust
// BL - ���� (�������)                                        Explode
// BM - ���� (����������)                                     Fused
// BN - ���� (����� � �����)                                  BurnRun
// BO - ���� (����� �� ����� � �������� ������ (����� �����)) 
// BP - ���� (����� �� ����� � �������� ������ (����� ����))
// _ABCDEFGHIJKLMNOPQRST
// 012345678901234567890

#define ABC_SIZE                        (26)

// Anim1
#define ANIM1_UNARMED                   (1)
#define ANIM1_DEAD                      (2) // Not used in 3d animations
#define ANIM1_KNOCKOUT                  (3) // Not used in 3d animations
#define ANIM1_KNIFE                     (4)
#define ANIM1_CLUB                      (5)
#define ANIM1_HAMMER                    (6)
#define ANIM1_SPEAR                     (7)
#define ANIM1_PISTOL                    (8)
#define ANIM1_UZI                       (9)
#define ANIM1_SHOOTGUN                  (10)
#define ANIM1_RIFLE                     (11)
#define ANIM1_MINIGUN                   (12)
#define ANIM1_ROCKET_LAUNCHER           (13)
#define ANIM1_AIM                       (14) // Not used in 3d animations
// Anim2
	// 2d animations
#define ANIM2_2D_STAY                   (1)
#define ANIM2_2D_WALK                   (2)
#define ANIM2_2D_SHOW                   (3)
#define ANIM2_2D_HIDE                   (4)
#define ANIM2_2D_DODGE_WEAPON           (5)
#define ANIM2_2D_THRUST                 (6)
#define ANIM2_2D_SWING                  (7)
#define ANIM2_2D_PREPARE_WEAPON         (8)
#define ANIM2_2D_TURNOFF_WEAPON         (9)
#define ANIM2_2D_SHOOT                  (10)
#define ANIM2_2D_BURST                  (11)
#define ANIM2_2D_FLAME                  (12)
#define ANIM2_2D_THROW_WEAPON           (13)
#define ANIM2_2D_DAMAGE_FRONT           (15)
#define ANIM2_2D_DAMAGE_BACK            (16)
#define ANIM2_2D_KNOCK_FRONT            (1)  // Only with ANIM1_DEAD
#define ANIM2_2D_KNOCK_BACK             (2)
#define ANIM2_2D_STANDUP_BACK           (8)  // Only with ANIM1_KO
#define ANIM2_2D_STANDUP_FRONT          (10)
#define ANIM2_2D_PICKUP                 (11) // Only with ANIM1_UNARMED
#define ANIM2_2D_USE                    (12)
#define ANIM2_2D_DODGE_EMPTY            (14)
#define ANIM2_2D_PUNCH                  (17)
#define ANIM2_2D_KICK                   (18)
#define ANIM2_2D_THROW_EMPTY            (19)
#define ANIM2_2D_RUN                    (20)
#define ANIM2_2D_DEAD_FRONT             (1)  // Only with ANIM1_DEAD
#define ANIM2_2D_DEAD_BACK              (2)
#define ANIM2_2D_DEAD_BLOODY_SINGLE     (4)
#define ANIM2_2D_DEAD_BURN              (5)
#define ANIM2_2D_DEAD_BLOODY_BURST      (6)
#define ANIM2_2D_DEAD_BURST             (7)
#define ANIM2_2D_DEAD_PULSE             (8)
#define ANIM2_2D_DEAD_LASER             (9)
#define ANIM2_2D_DEAD_BURN2             (10)
#define ANIM2_2D_DEAD_PULSE_DUST        (11)
#define ANIM2_2D_DEAD_EXPLODE           (12)
#define ANIM2_2D_DEAD_FUSED             (13)
#define ANIM2_2D_DEAD_BURN_RUN          (14)
#define ANIM2_2D_DEAD_FRONT2            (15)
#define ANIM2_2D_DEAD_BACK2             (16)
	// 3d animations
#define ANIM2_3D_IDLE                   (1)
#define ANIM2_3D_IDLE_STUNNED           (2)
#define ANIM2_3D_WALK                   (3)
#define ANIM2_3D_LIMP                   (4)
#define ANIM2_3D_RUN                    (5)
#define ANIM2_3D_PANIC_RUN              (6)
#define ANIM2_3D_SHOW                   (7)
#define ANIM2_3D_HIDE                   (8)
#define ANIM2_3D_FIDGET1                (9)
#define ANIM2_3D_FIDGET2                (10)
#define ANIM2_3D_CLIMBING               (11)
#define ANIM2_3D_PICKUP                 (12)
#define ANIM2_3D_USE                    (13)
#define ANIM2_3D_SWITCH_ITEMS           (14)
#define ANIM2_3D_BEGIN_COMBAT           (15)
#define ANIM2_3D_IDLE_COMBAT            (16)
#define ANIM2_3D_END_COMBAT             (17)
#define ANIM2_3D_PUNCH_RIGHT            (18)
#define ANIM2_3D_PUNCH_LEFT             (19)
#define ANIM2_3D_PUNCH_COMBO            (20)
#define ANIM2_3D_KICK_HI                (21)
#define ANIM2_3D_KICK_LO                (22)
#define ANIM2_3D_KICK_COMBO             (23)
#define ANIM2_3D_THRUST_1H              (24)
#define ANIM2_3D_THRUST_2H              (25)
#define ANIM2_3D_SLASH_1H               (26)
#define ANIM2_3D_SLASH_2H               (27)
#define ANIM2_3D_THROW                  (28)
#define ANIM2_3D_SINGLE                 (29)
#define ANIM2_3D_BURST                  (30)
#define ANIM2_3D_SWEEP                  (31)
#define ANIM2_3D_BUTT                   (32)
#define ANIM2_3D_FLAME                  (33)
#define ANIM2_3D_NO_RECOIL              (34)
#define ANIM2_3D_RELOAD                 (35)
#define ANIM2_3D_REPAIR                 (36)
#define ANIM2_3D_DODGE_FRONT            (40)
#define ANIM2_3D_DODGE_BACK             (41)
#define ANIM2_3D_DAMAGE_FRONT           (42)
#define ANIM2_3D_DAMAGE_BACK            (43)
#define ANIM2_3D_DAMAGE_MUL_FRONT       (44)
#define ANIM2_3D_DAMAGE_MUL_BACK        (45)
#define ANIM2_3D_WALK_DAMAGE_FRONT      (46)
#define ANIM2_3D_WALK_DAMAGE_BACK       (47)
#define ANIM2_3D_LIMP_DAMAGE_FRONT      (48)
#define ANIM2_3D_LIMP_DAMAGE_BACK       (49)
#define ANIM2_3D_RUN_DAMAGE_FRONT       (50)
#define ANIM2_3D_RUN_DAMAGE_BACK        (51)
#define ANIM2_3D_KNOCK_FRONT            (52)
#define ANIM2_3D_KNOCK_BACK             (53)
#define ANIM2_3D_LAYDOWN_FRONT          (54)
#define ANIM2_3D_LAYDOWN_BACK           (55)
#define ANIM2_3D_IDLE_PRONE_FRONT       (56)
#define ANIM2_3D_IDLE_PRONE_BACK        (57)
#define ANIM2_3D_STANDUP_FRONT          (58)
#define ANIM2_3D_STANDUP_BACK           (59)
#define ANIM2_3D_DAMAGE_PRONE_FRONT     (60)
#define ANIM2_3D_DAMAGE_PRONE_BACK      (61)
#define ANIM2_3D_DAMAGE_MUL_PRONE_FRONT (62)
#define ANIM2_3D_DAMAGE_MUL_PRONE_BACK  (63)
#define ANIM2_3D_TWITCH_PRONE_FRONT     (64)
#define ANIM2_3D_TWITCH_PRONE_BACK      (65)
#define ANIM2_3D_DEAD_PRONE_FRONT       (66)
#define ANIM2_3D_DEAD_PRONE_BACK        (67)
#define ANIM2_3D_DEAD_BLOODY_SINGLE     (70)
#define ANIM2_3D_DEAD_BLOODY_BURST      (71)
#define ANIM2_3D_DEAD_BURST             (72)
#define ANIM2_3D_DEAD_PULSE             (73)
#define ANIM2_3D_DEAD_PULSE_DUST        (74)
#define ANIM2_3D_DEAD_LASER             (75)
#define ANIM2_3D_DEAD_FUSED             (76)
#define ANIM2_3D_DEAD_EXPLODE           (77)
#define ANIM2_3D_DEAD_BURN              (78)
#define ANIM2_3D_DEAD_BURN_RUN          (79)

#endif // __DEFINES__