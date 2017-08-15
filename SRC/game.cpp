#include "game.h"

#include <assert.h>
#include <stdio.h>

void world_state_init() {
	//Init gameObjects[] first
	for (u32 i = 0; i < MAX_GO; i++) {
		gameObjects[i].id = UNUSED;
	}
	positionComps = list_new(free);
	visibilityComps = list_new(free);
	physicalComps = list_new(free);
	movementComps = list_new(free);
	healthComps = list_new(free);
	combatComps = list_new(free);
	equipmentComps = list_new(free);
	treasureComps = list_new(free);

	carriedItems = list_new(free);
}

GameObject *game_object_create() {
	// Find the next available object space
	GameObject *go = NULL;
	for (i32 i = 0; i < MAX_GO; i++) {
		if (gameObjects[i].id == UNUSED) {
			go = &gameObjects[i];
			go->id = i;
			break;
		}
	}

	assert(go != NULL);		// Have we run out of game objects?

	for (i32 i = 0; i < COMPONENT_COUNT; i++) {
		go->components[i] = NULL;
	}

	return go;
}

void game_object_destroy(GameObject *obj) {
	ListElement *elementToRemove = list_search(positionComps, obj->components[COMP_POSITION]);
	if (elementToRemove != NULL ) { list_remove(positionComps, elementToRemove); }

	elementToRemove = list_search(visibilityComps, obj->components[COMP_VISIBILITY]);
	if (elementToRemove != NULL ) { list_remove(visibilityComps, elementToRemove); }

	elementToRemove = list_search(physicalComps, obj->components[COMP_PHYSICAL]);
	if (elementToRemove != NULL ) { list_remove(physicalComps, elementToRemove); }

	elementToRemove = list_search(movementComps, obj->components[COMP_MOVEMENT]);
	if (elementToRemove != NULL ) { list_remove(movementComps, elementToRemove); }

	elementToRemove = list_search(healthComps, obj->components[COMP_HEALTH]);
	if (elementToRemove != NULL ) { list_remove(healthComps, elementToRemove); }

	elementToRemove = list_search(combatComps, obj->components[COMP_COMBAT]);
	if (elementToRemove != NULL ) { list_remove(combatComps, elementToRemove); }

	elementToRemove = list_search(equipmentComps, obj->components[COMP_EQUIPMENT]);
	if (elementToRemove != NULL ) { list_remove(equipmentComps, elementToRemove); }

	elementToRemove = list_search(treasureComps, obj->components[COMP_TREASURE]);
	if (elementToRemove != NULL ) { list_remove(treasureComps, elementToRemove); }

	// TODO: Clean up other components used by this object

	obj->id = UNUSED;
	for (i32 i = 0; i < COMPONENT_COUNT; i++) {
		obj->components[i] = NULL;
	}
}

void *game_object_get_component(GameObject *obj, 
								GameComponentType comp) {
	return obj->components[comp];
}

//Return goPositions[][]
List *game_objects_at_position(u32 x, u32 y) {
	return goPositions[x][y];
}

void game_object_update_component(GameObject *obj, GameComponentType comp, void *compData) {
	assert(obj->id != UNUSED);
	//list_insert_after(visibilityComps, NULL, vis);
	//list_remove_element_with_data(visibilityComps, vis);
	switch (comp) {
		case COMP_POSITION: {
			if (compData != NULL) {
				Position *pos = (Position *)obj->components[COMP_POSITION];
				bool addedNew = false;
				if (pos == NULL) {
					pos = (Position *)malloc(sizeof(Position));
					addedNew = true;
				} else {
					// Remove game obj from the position helper DS
					List *ls = goPositions[pos->x][pos->y];
					list_remove_element_with_data(ls, obj);
				}
				Position *posData = (Position *)compData;
				pos->objectId = obj->id;
				pos->x = posData->x;
				pos->y = posData->y;
				pos->layer = posData->layer;
				// Only insert into world component list if we just allocated a new component
				if (addedNew) {
					//Insert pos to positionComps[]
					list_insert_after(positionComps, NULL, pos);					
				}
				obj->components[comp] = pos;
				// Update our helper DS 
				List *gos = goPositions[posData->x][posData->y];
				if (gos == NULL) {
					gos = list_new(free);
					goPositions[posData->x][posData->y] = gos;
				}
				list_insert_after(gos, NULL, obj);

			}else{
				// Clear component 
				Position *pos = (Position *)obj->components[COMP_POSITION];
				if (pos != NULL) {
					list_remove_element_with_data(positionComps, pos);	
				}
				obj->components[comp] = NULL;
				// Remove game obj from the position helper DS
				List *ls = goPositions[pos->x][pos->y];
				list_remove_element_with_data(ls, obj);
			}
		}
		break;
		case COMP_VISIBILITY: {
			if (compData != NULL) {
				Visibility *vis = (Visibility *)obj->components[COMP_VISIBILITY];
				bool addedNew = false;
				if (vis == NULL)  {
					vis = (Visibility *)malloc(sizeof(Visibility));
					addedNew = true;
				}
				Visibility *visData = (Visibility *)compData;
				vis->objectId = obj->id;
				vis->glyph = visData->glyph;
				vis->fgColor = visData->fgColor;
				vis->bgColor = visData->bgColor;
				vis->hasBeenSeen = visData->hasBeenSeen;
				vis->visibleOutsideFOV = visData->visibleOutsideFOV;
				if (visData->name != NULL) {
					vis->name = (char*)malloc(strlen(visData->name) + 1);
					strcpy(vis->name, visData->name);
				}

				if (addedNew) {
					list_insert_after(visibilityComps, NULL, vis);					
				}
				//Save compData
				obj->components[comp] = vis;
			}else{
				printf("%s\n", "COMP_VISIBILITY, compData=NULL clear component");
				Visibility *vis = (Visibility *)obj->components[COMP_VISIBILITY];
				if (vis != NULL) {
					//Remove compoment from visibilityComps[]
					list_remove_element_with_data(visibilityComps, vis);				
				}
				obj->components[comp] = NULL;
			}

		}
		break;
		case COMP_PHYSICAL: {
		}
		break;
		case COMP_MOVEMENT: {

		}
		break;
		case COMP_HEALTH: {
		}
		break;
		case COMP_COMBAT: {
		}
		break;
		case COMP_EQUIPMENT: {
		}
		break;
		case COMP_TREASURE: {
		}
		break;
		default:
			assert(1==0);
		break;
	}


}

DungeonLevel * level_init(i32 levelToGenerate, GameObject *player) {

	// Clear the previous level data from the world state
	// Note: We start at index 1 because the player is at index 0 and we want to keep them!
	for (u32 i = 1; i < MAX_GO; i++) {
		if ((gameObjects[i].id != player->id) && (gameObjects[i].id != UNUSED) && list_search(carriedItems, &gameObjects[i]) == NULL) {
			game_object_destroy(&gameObjects[i]);
		}
	}

	// Generate a level map into the world state
	//bool (*mapCells)[MAP_HEIGHT] =  malloc(MAP_WIDTH * MAP_HEIGHT);
	//map_generate(mapCells);
	//for (u32 x = 0; x < MAP_WIDTH; x++) {
	//	for (u32 y = 0; y < MAP_HEIGHT; y++) {
	//		if (mapCells[x][y]) {
	//			wall_add(x, y);
	//		} else {
	//			floor_add(x, y);
	//		}
	//	}
	//ß}

	// Create DungeonLevel Object and store relevant info
	DungeonLevel *level = (DungeonLevel *)malloc(sizeof(DungeonLevel));
	level->level = levelToGenerate;
	//level->mapWalls = mapCells;

	// Grab the number of monsters to generate for this level from level config
	i32 monstersToAdd = maxMonsters[levelToGenerate-1];
	for (i32 i = 0; i < monstersToAdd; i++) {
		// Consult our monster appearance data to determine what monster to generate.
		//i32 monsterId = monster_for_level(levelToGenerate);
		//ConfigEntity *monsterEntity = get_entity_with_id(monsterConfig, monsterId);
		//if (monsterEntity != NULL) {
			// Add the monster
			//Point pt = level_get_open_point(mapCells);
			//char *name = config_entity_value(monsterEntity, "name");
			//npc_add(name, pt.x, pt.y, LAYER_TOP, g, c, s, f, hp, rr, hit, 0, att, def, 0, 0, 0);
		//}
	}
	i32 itemsToAdd = maxItems[levelToGenerate-1];
	for (i32 i = 0; i < itemsToAdd; i++) {
		//i32 itemId = item_for_level(levelToGenerate);
		//ConfigEntity *entity = get_entity_with_id(itemConfig, itemId);
		//if (entity != NULL) {
			//item_add(name, pt.x, pt.y, LAYER_MID, g, c, toHitMod, attMod, defMod, dodgeMod, qty, weight, slot);
		//}
	}

	// Place gems in random positions around the level
	//gemsFoundThisLevel = 0;
	for (i32 i = 0; i < GEMS_PER_LEVEL; i++) {
	}

	// Place a staircase in a random position in the level
	GameObject *stairs = game_object_create();
	
	//Point ptStairs = level_get_open_point(mapCells);
	Point ptStairs = {.x=10,.y=10};
	Position stairPos = {.objectId = stairs->id, .x = ptStairs.x, .y = ptStairs.y, .layer = LAYER_GROUND};
	game_object_update_component(stairs, COMP_POSITION, &stairPos);
	Visibility vis = {.objectId = stairs->id, .glyph = '>', .fgColor = 0xffd700ff, .bgColor = 0x00000000, .visibleOutsideFOV = true, .name=(char*)"Stairs"};
	game_object_update_component(stairs, COMP_VISIBILITY, &vis);
	Physical phys = {.objectId = stairs->id, .blocksMovement = false, .blocksSight = false};
	game_object_update_component(stairs, COMP_PHYSICAL, &phys);

	// Place our player in a random position in the level
	//Point pt = level_get_open_point(mapCells);
	Point pt = {.x=0,.y=0};
	Position pos = {.objectId = player->id, .x = pt.x, .y = pt.y, .layer = LAYER_TOP};
	game_object_update_component(player, COMP_POSITION, &pos);

	return level;
}

void game_new()
{
	// -- Start a brand new game --
	world_state_init();
	// Take a object from gameObjects[10000] as Player
	player = game_object_create();
	//
	Visibility vis = {.objectId=player->id, .glyph='@', .fgColor=0x00FF00FF, .bgColor=0x00000000, .hasBeenSeen=true, .name=(char*)"Player"};
	game_object_update_component(player, COMP_VISIBILITY, &vis);
	//
	Physical phys = {player->id, true, true};
	game_object_update_component(player, COMP_PHYSICAL, &phys);
	//
	Health hlth = {.objectId = player->id, .currentHP = 20, .maxHP = 20, .recoveryRate = 1};
	game_object_update_component(player, COMP_HEALTH, &hlth);
	//
	Combat com = {.objectId = player->id, .toHit=80, .toHitModifier=0, .attack = 5, .defense = 2, .attackModifier = 0, .defenseModifier = 0, .dodgeModifier = 0};
	game_object_update_component(player, COMP_COMBAT, &com);

	//playerName = name_create();

	currentLevelNumber = 1;
	currentLevel = level_init(currentLevelNumber, player);
	
	Position *playerPos = (Position *)game_object_get_component(player, COMP_POSITION);

	//fov_calculate(playerPos->x, playerPos->y, fovMap);

	//generate_target_map(playerPos->x, playerPos->y);
}


