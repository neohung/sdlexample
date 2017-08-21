#include <stdio.h>
#include "ui.h"
#include "game.h"
#include "stringutil.h"
extern void quit_game(); //extern from main.cpp
UIScreen *in_game_screen = NULL;

UIView *inventoryView = NULL;

static i32 highlightedIdx = 0;

void render_game_map_view(Console *console)
{
		// Setup layer render history
		u8 layerRendered[MAP_WIDTH][MAP_HEIGHT];
		for (u32 x = 0; x < MAP_WIDTH; x++) {
			for (u32 y = 0; y < MAP_HEIGHT; y++) {
				layerRendered[x][y] = LAYER_UNSET;
			}
		}
		//Visibility Comps
		ListElement *e = list_head(visibilityComps);
		while (e != NULL) {
			Visibility *vis = (Visibility *)list_data(e);
			//if ( vis->objectId > 1000)
			//  printf("VIS: %s, [%d, '%c']\n",vis->name, vis->objectId, vis->glyph);
			Position *p = (Position *)game_object_get_component(&gameObjects[vis->objectId], COMP_POSITION);
			if (p != NULL) {
				if (fovMap[p->x][p->y] > 0) {
				//Means in fov, can see
					vis->hasBeenSeen = true;
					//If this p->layer lower then previously render layer before, we don't render it.
					if (p->layer > layerRendered[p->x][p->y]) {
					  console_put_char_at(console, vis->glyph, p->x, p->y, vis->fgColor, vis->bgColor);
					  layerRendered[p->x][p->y] = p->layer;
					}

				} else if (vis->visibleOutsideFOV && vis->hasBeenSeen) {
					//Out of fov but still can see, so use fade color
					//If this p->layer lower then previously render layer before, we don't render it.
					if (p->layer > layerRendered[p->x][p->y]) {
						u32 fullColor = vis->fgColor;
						u32 fadedColor = COLOR_FROM_RGBA(RED(fullColor), GREEN(fullColor), BLUE(fullColor), 0x77);
						console_put_char_at(console, vis->glyph, p->x, p->y, fadedColor, 0x000000FF);
						layerRendered[p->x][p->y] = p->layer;
					}
				}
			}
			//printf("p%d, %d, %d, %d\n",p->objectId,p->layer,p->x,p->y);
			e = list_next(e);
		}

}

void render_inventory_view(Console *console) 
{
	UIRect rect = {0, 0, INVENTORY_WIDTH, INVENTORY_HEIGHT};
	view_draw_rect(console, &rect, 0x222222FF, 0, 0xFF990099);

	// We should load and process the bg image only once, not on each render
	BitmapImage *bgImage = NULL;
	AsciiImage *aiImage = NULL;
	if (bgImage == NULL) {
		bgImage = image_load_from_file((char*)"./scrollBackground.png");
		aiImage = asciify_bitmap(console, bgImage);	
	}
	if (asciiMode) {
		view_draw_ascii_image_at(console, aiImage, 0, 0);
	} else {
		view_draw_image_at(console, bgImage, 0, 0);	
	}

	i32 yIdx = 4;
	i32 currIdx = 0;
	ListElement *e = list_head(carriedItems);
	while (e != NULL) {
		// For each item - Equipped indicator, Item name, [slot], weight
		GameObject *go = (GameObject *)e->data;
		Visibility *v = (Visibility *)game_object_get_component(go, COMP_VISIBILITY);
		Equipment *eq = (Equipment *)game_object_get_component(go, COMP_EQUIPMENT);
		if (v != NULL && eq != NULL) {
			char *equipped = (eq->isEquipped) ? (char *)"*" : (char *)".";
			char *slotStr = String_Create("[%s]", eq->slot);
			char *itemText = String_Create("%s %-10s %-8s wt: %d", equipped, v->name, slotStr, eq->weight);
			if (currIdx == highlightedIdx) {
				if (eq->isEquipped) {
					console_put_string_at(console, itemText, 6, yIdx, 0x98FB98ff, 0x80000099);
				} else {
					console_put_string_at(console, itemText, 6, yIdx, 0xffffffff, 0x80000099);
				}
			} else {
				if (eq->isEquipped) {
					console_put_string_at(console, itemText, 6, yIdx, 0x006400ff, 0x00000000);
				} else {
					console_put_string_at(console, itemText, 6, yIdx, 0x333333ff, 0x00000000);
				}
			}
			yIdx += 1;
		}
		currIdx += 1;
		e = list_next(e);
	}
	// Render additional information at bottom of view
	char *weightInfo = String_Create("Carrying: %d  Max: %d", item_get_weight_carried(), maxWeightAllowed);
	console_put_string_at(console, weightInfo, 10, 23, 0x000044ff, 0x00000000);

	char *instructions = String_Create("[Up/Down] to select item");
	char *i2 = String_Create("[Spc] to (un)equip, [D] to drop");
	console_put_string_at(console, instructions, 5, 25, 0x333333ff, 0x00000000);
	console_put_string_at(console, i2, 5, 26, 0x333333ff, 0x00000000);
}

void hide_inventory_overlay(UIScreen *screen) 
{
	if (inventoryView != NULL) {
		list_remove_element_with_data(screen->views, inventoryView);
		view_destroy(inventoryView);
		inventoryView = NULL;
	}
}
void show_inventory_overlay(UIScreen *screen) 
{
	if (inventoryView == NULL) {
		UIRect overlayRect = {(16 * INVENTORY_LEFT), (16 * INVENTORY_TOP), (16 * INVENTORY_WIDTH), (16 * INVENTORY_HEIGHT)};
		inventoryView = view_new(overlayRect, INVENTORY_WIDTH, INVENTORY_HEIGHT,  (char*)"./terminal16x16.png", 0, render_inventory_view);
		list_insert_after(screen->views, list_tail(screen->views), inventoryView);
	}
}

void handle_event_in_game(UIScreen *activeScreen, UIEvent event)
{
	if (event.type == UI_KEYDOWN) {
		//Extract player position
		Position *playerPos = (Position *)game_object_get_component(player, COMP_POSITION);

		UIKeycode key = KEYSYM(event);
		switch (key) {
			case SDLK_ESCAPE: {
				//ui_set_active_screen(screen_test());
				quit_game();
			}
			break;
			case SDLK_x: {
				//free(currentLevel->mapWalls);
				//level_init(1, player);
				level_descend();
			}
			break;
			case SDLK_LEFT:
			case SDLK_RIGHT:
			case SDLK_DOWN:
			case SDLK_UP: {
				if (inventoryView != NULL) {
					// Handle for inventory view
					switch (key){
					case SDLK_DOWN:
						highlightedIdx += 1;
						break;
					case SDLK_UP:
						highlightedIdx -= 1;
						break;
					default:
						break;
					}
					if (highlightedIdx < 0) { 
						highlightedIdx = 0;
					} 
					if (highlightedIdx > list_size(carriedItems)-1) { 
						highlightedIdx = list_size(carriedItems) - 1; 
					}
				}else{
				char offsetx = 0;
				char offsety = 0;
				switch (key){
					case SDLK_LEFT:
						offsetx = -1;
						break;
					case SDLK_RIGHT:
						offsetx = 1;
						break;
					case SDLK_DOWN:
						offsety = 1;
						break;
					case SDLK_UP:
						offsety = -1;
						break;
					default:
						break;
				}
				Position newPos = {playerPos->objectId, playerPos->x + offsetx, playerPos->y + offsety, LAYER_TOP};
				//printf("Detect can move\n");
				if (can_move(newPos)) {
					game_object_update_component(player, COMP_POSITION, &newPos);
					recalculateFOV = true;
					playerTookTurn = true;
				}else{
					// Check to see what is blocking movement. If NPC - resolve combat!
					List *blockers = game_objects_at_position(playerPos->x+ offsetx, playerPos->y + offsety);
					if (blockers == NULL){
						//printf("blockers=NULL\n");
						break;
					}
					GameObject *blockerObj = NULL;
					ListElement *e = list_head(blockers);
					while (e != NULL) {
						GameObject *go = (GameObject *)list_data(e);
						Combat *cc = (Combat *)game_object_get_component(go, COMP_COMBAT);
						if (cc != NULL) {
							blockerObj = go;
							break;
						}
						e = list_next(e);
					}
					if (blockerObj != NULL) {
						printf("Player do attack\n");
						combat_attack(player, blockerObj);
						playerTookTurn = true;
					}
				}
			  }
			}
			break;
			case SDLK_d: {
				if (inventoryView != NULL) {
					ListElement *le = list_item_at(carriedItems, highlightedIdx);
					if (le != NULL) {
						item_drop((GameObject*)le->data);
					}
				} else {
					// Descend to the next level
					level_descend();
				}
			}
			break;
			case SDLK_SPACE:
			case SDLK_e: {
				if (inventoryView != NULL) {
					ListElement *le = list_item_at(carriedItems, highlightedIdx);
					if (le != NULL) {
						item_toggle_equip((GameObject*)le->data);
					}
				}
			}
			break;
			case SDLK_z: {
				// Player rests
				health_recover_player_only();
				playerTookTurn = true;
			}
			break;
			case SDLK_g: {
				item_get();
			}
			break;
			case SDLK_i: {
				if (inventoryView == NULL) {
					show_inventory_overlay(activeScreen);				
				} else {
					hide_inventory_overlay(activeScreen);
				}
			}
			break;
			default:
				break;
		}
	}
}

void render_stats_view(Console *console)
{
	UIRect rect = {0, 0, STATS_WIDTH, STATS_HEIGHT};
	view_draw_rect(console, &rect, 0x222222FF, 0, 0xFF990099);
// Player name
	if (playerName != NULL) {
		console_put_string_at(console, playerName, 0, 0, 0xffffffff, 0x00000000);
	}

	// HP health bar
	Health *playerHealth = (Health *)game_object_get_component(player, COMP_HEALTH);
	console_put_char_at(console, 'H', 0, 1, 0xFF990099, 0x00000000);
	console_put_char_at(console, 'P', 1, 1, 0xFF990099, 0x00000000);
	i32 leftX = 3;
	i32 barWidth = 16;

	i32 healthCount = ceil(((float)playerHealth->currentHP / (float)playerHealth->maxHP) * barWidth);
	for (i32 x = 0; x < barWidth; x++) {
		if (x < healthCount) {
			console_put_char_at(console, 176, leftX + x, 1, 0x009900FF, 0x00000000);		
			console_put_char_at(console, 3, leftX + x, 1, 0x009900FF, 0x00000000);		
		} else {
			console_put_char_at(console, 176, leftX + x, 1, 0xFF990099, 0x00000000);		
		}
	}
	Combat *playerCombat = (Combat *)game_object_get_component(player, COMP_COMBAT);
	char *att = String_Create("ATT:%d (%d)", playerCombat->attack, playerCombat->attackModifier);
	console_put_string_at(console, att, 0, 2, 0xe6e600FF, 0x00000000);
	String_Destroy(att);

	char *def = String_Create("DEF:%d (%d)", playerCombat->defense, playerCombat->defenseModifier);
	console_put_string_at(console, def, 0, 3, 0xe6e600FF, 0x00000000);
	String_Destroy(def);

	char *level = String_Create("Level:%d", currentLevelNumber);
	console_put_string_at(console, level, 0, 4, 0xffd700ff, 0x00000000);
	String_Destroy(level);

	char *gems = String_Create("Gems:%d", gemsFoundTotal);
	console_put_string_at(console, gems, 10, 4, 0x753aabff, 0x00000000);
	String_Destroy(gems);
}

void render_message_log_view(Console *console)
{
	UIRect rect = {0, 0, LOG_WIDTH, LOG_HEIGHT};
	view_draw_rect(console, &rect, 0x191919FF, 0, 0xFF990099);
	if (messageLog == NULL) { return; }

	// Get the last 5 messages from the log
	ListElement *e = list_tail(messageLog);
	i32 msgCount = list_size(messageLog);
	u32 row = 4;
	u32 col = 0;

	if (msgCount < 5) {
		row -= (5 - msgCount);
	} else {
		msgCount = 5;
	}

	for (i32 i = 0; i < msgCount; i++) {
		if (e != NULL) {
			Message *m = (Message *)list_data(e);
			UIRect rect = {.x = col, .y = row, .w = LOG_WIDTH, .h = 1};
			console_put_string_in_rect(console, m->msg, rect, false, m->fgColor, 0x00000000);
			e = list_prev(e);			
			row -= 1;
		}
	}

}

UIScreen *screen_show_in_game()
{
	if (in_game_screen != NULL)
			return in_game_screen;

	List *igViews = list_new(NULL);

	UIRect mapRect = {0, 0, (16 * MAP_WIDTH), (16 * MAP_HEIGHT)};
	UIView *mapView = view_new(mapRect, MAP_WIDTH, MAP_HEIGHT,(char*)"./terminal16x16.png", 0, render_game_map_view);
	list_insert_after(igViews, NULL, mapView);

	UIRect statsRect = {0, (16 * MAP_HEIGHT), (16 * STATS_WIDTH), (16 * STATS_HEIGHT)};
	UIView *statsView = view_new(statsRect, STATS_WIDTH, STATS_HEIGHT,(char*)"./terminal16x16.png", 0, render_stats_view);
	list_insert_after(igViews, NULL, statsView);

	UIRect logRect = {(16 * 22), (16 * MAP_HEIGHT), (16 * LOG_WIDTH), (16 * LOG_HEIGHT)};
	UIView *logView = view_new(logRect, LOG_WIDTH, LOG_HEIGHT, (char*)"./terminal16x16.png", 0, render_message_log_view);
	list_insert_after(igViews, NULL, logView);

	//handle_event_in_game
	return screen_new(igViews, mapView, handle_event_in_game);
}
