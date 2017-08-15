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

void game_object_update_component(GameObject *obj, GameComponentType comp, void *compData) {
	assert(obj->id != UNUSED);
	//list_insert_after(visibilityComps, NULL, vis);
	//list_remove_element_with_data(visibilityComps, vis);
	switch (comp) {
		case COMP_POSITION: {
			if (compData != NULL) {
				
			}else{
				// Clear component 
				Position *pos = (Position *)obj->components[COMP_POSITION];
				if (pos != NULL) {
					list_remove_element_with_data(positionComps, pos);	
				}
				obj->components[comp] = NULL;
				// Remove game obj from the position helper DS
				//List *ls = goPositions[pos->x][pos->y];
				//list_remove_element_with_data(ls, obj);
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


	// Create DungeonLevel Object and store relevant info
	DungeonLevel *level = (DungeonLevel *)malloc(sizeof(DungeonLevel));
	level->level = levelToGenerate;
	//level->mapWalls = mapCells;

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
	//
	game_object_update_component(player, COMP_VISIBILITY, &vis);

	currentLevelNumber = 1;
	currentLevel = level_init(currentLevelNumber, player);
	

}

void game_object_destroy(GameObject *obj) {
	//elementToRemove = list_search(visibilityComps, obj->components[COMP_VISIBILITY]);
	//if (elementToRemove != NULL ) { list_remove(visibilityComps, elementToRemove); }
}
