#ifndef _GAME_H_
#define _GAME_H_

#include "list.h"

#define UNUSED	-1

#define LAYER_UNSET		0
#define LAYER_GROUND	1
#define LAYER_MID		2
#define LAYER_AIR		3
#define LAYER_TOP		4
//======================================

#define FPS_LIMIT		20
#define SCREEN_WIDTH	640 //320 //1280
#define SCREEN_HEIGHT	480 //240 //720
#define BG_WIDTH    (SCREEN_WIDTH / 16)
#define BG_HEIGHT   (SCREEN_HEIGHT / 16)

#define MAX_GO 10000

#define MONSTER_TYPE_COUNT	100
#define ITEM_TYPE_COUNT		100
#define MAX_DUNGEON_LEVEL	20
#define GEMS_PER_LEVEL		5

//#define MAP_WIDTH	80
//#define MAP_HEIGHT	40
#define MAP_WIDTH	(SCREEN_WIDTH / 16)
#define MAP_HEIGHT	((SCREEN_HEIGHT / 16) - 5)


//=======================================

typedef enum {
	COMP_POSITION = 0,
	COMP_VISIBILITY,
	COMP_PHYSICAL,
	COMP_HEALTH,
	COMP_MOVEMENT,
	COMP_COMBAT,
	COMP_EQUIPMENT,
	COMP_TREASURE,
	/* Define other components above here */
	COMPONENT_COUNT
} GameComponentType;

typedef struct {
	i32 id;
	void *components[COMPONENT_COUNT];
} GameObject;

typedef struct {
	i32 objectId;
	u8 x, y;
	u8 layer;				// 1 is bottom layer
} Position;

typedef struct {
	i32 x, y;
} Point;

typedef struct {
	Point start;
	Point mid;
	Point end;
	i32 roomFrom;
	i32 roomTo;
	bool hasWaypoint;
} Segment;

typedef unsigned char asciiChar;

typedef struct {
	i32 objectId;
	asciiChar glyph;
	u32 fgColor;
	u32 bgColor;
	bool hasBeenSeen;
	bool visibleOutsideFOV;
	char *name;
} Visibility;

typedef struct {
	i32 objectId;
	bool blocksMovement;
	bool blocksSight;
} Physical;

typedef struct {
	i32 objectId;
	i32 speed;				// How many spaces the object can move when it moves.
	i32 frequency;			// How often the object moves. 1=every tick, 2=every other tick, etc.
	i32 ticksUntilNextMove;	// Countdown to next move. Moves when = 0.
	Point destination;
	bool hasDestination;
	bool chasingPlayer;
	i32 turnsSincePlayerSeen;
} Movement;

typedef struct {
	i32 objectId;
	i32 currentHP;
	i32 maxHP;
	i32 recoveryRate;		// HP recovered per tick.
	i32 ticksUntilRemoval;	// Countdown to removal from world state
} Health;

typedef struct {
	i32 objectId;
	i32 toHit;				// chance to hit
	i32 toHitModifier;
	i32 attack;				// attack = damage inflicted per hit
	i32 attackModifier;		// based on weapons/items
	i32 defense;			// defense = damage absorbed before HP is affected
	i32 defenseModifier;	// based on armor/items
	i32 dodgeModifier;		// % that attack was missed/dodged/etc
} Combat;

typedef struct {
	i32 objectId;
	i32 quantity;
	i32 weight;
	char *slot;
	bool isEquipped;
} Equipment;

typedef struct {
	i32 objectId;
	i32 value;
} Treasure;

/* Level Support */

typedef struct {
	i32 level;
	bool **mapWalls;
} DungeonLevel;


/* Message Log */
typedef struct {
	char *msg;
	u32 fgColor;
} Message;


extern GameObject gameObjects[MAX_GO];
extern List *visibilityComps;
extern GameObject *player;
extern List *positionComps;
extern List *physicalComps;
extern List *movementComps;
extern List *healthComps;
extern List *combatComps;
extern List *equipmentComps;
extern List *treasureComps;

extern List *carriedItems;

extern i32 maxMonsters[MAX_DUNGEON_LEVEL];
extern i32 maxItems[MAX_DUNGEON_LEVEL];

extern i32 currentLevelNumber;
extern DungeonLevel *currentLevel;

extern u32 fovMap[MAP_WIDTH][MAP_HEIGHT];
extern i32 (*targetMap)[MAP_HEIGHT];
extern List *goPositions[MAP_WIDTH][MAP_HEIGHT];
extern char* playerName;

void game_new();
DungeonLevel * level_init(i32 levelToGenerate, GameObject *player);
void *game_object_get_component(GameObject *obj, GameComponentType comp);
void game_object_update_component(GameObject *obj, GameComponentType comp, void *compData);
List *game_objects_at_position(u32 x, u32 y);
bool can_move(Position pos);
#endif
