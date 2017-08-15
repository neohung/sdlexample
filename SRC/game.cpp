#include "game.h"


void world_state_init() {
	//for (u32 i = 0; i < MAX_GO; i++) {
	//	gameObjects[i].id = UNUSED;
	//}
	//positionComps = list_new(free);
	visibilityComps = list_new(free);

}
void game_new()
{
	// -- Start a brand new game --
	world_state_init();
}

void game_object_update_component(GameObject *obj, GameComponentType comp, void *compData) {
	//list_insert_after(visibilityComps, NULL, vis);
	//list_remove_element_with_data(visibilityComps, vis);
}

void game_object_destroy(GameObject *obj) {
	//elementToRemove = list_search(visibilityComps, obj->components[COMP_VISIBILITY]);
	//if (elementToRemove != NULL ) { list_remove(visibilityComps, elementToRemove); }
}

