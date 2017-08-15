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
#define MAX_GO 10000

#define MONSTER_TYPE_COUNT	100
#define ITEM_TYPE_COUNT		100
#define MAX_DUNGEON_LEVEL	20
#define GEMS_PER_LEVEL		5

#define MAP_WIDTH	80
#define MAP_HEIGHT	40

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
	bool (*mapWalls)[MAP_HEIGHT];
} DungeonLevel;


/* Message Log */
typedef struct {
	char *msg;
	u32 fgColor;
} Message;

static GameObject gameObjects[MAX_GO];
static GameObject *player = NULL;
static List *visibilityComps;
static List *positionComps;

static	i32 currentLevelNumber;
static DungeonLevel *currentLevel;

void game_new();

#endif
