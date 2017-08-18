#include <stdio.h>
#include "ui.h"
#include "game.h"

extern void quit_game(); //extern from main.cpp
UIScreen *in_game_screen = NULL;

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
				free(currentLevel->mapWalls);
				level_init(1, player);
			}
			break;
			case SDLK_LEFT:
			case SDLK_RIGHT:
			case SDLK_DOWN:
			case SDLK_UP: {
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
						/*
						Combat *cc = (Combat *)game_object_get_component(go, COMP_COMBAT);
						if (cc != NULL) {
							blockerObj = go;
							break;
						}
						*/
						e = list_next(e);
					}
					if (blockerObj != NULL) {
						//combat_attack(player, blockerObj);
						//playerTookTurn = true;
					}
				}
			}
			break;
			default:
				break;
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

	//UIRect statsRect = {0, (16 * MAP_HEIGHT), (16 * STATS_WIDTH), (16 * STATS_HEIGHT)};
	//UIView *statsView = view_new(statsRect, STATS_WIDTH, STATS_HEIGHT,"./terminal16x16.png", 0, render_stats_view);
	//list_insert_after(igViews, NULL, statsView);

	//UIRect logRect = {(16 * 22), (16 * MAP_HEIGHT), (16 * LOG_WIDTH), (16 * LOG_HEIGHT)};
	//UIView *logView = view_new(logRect, LOG_WIDTH, LOG_HEIGHT, "./terminal16x16.png", 0, render_message_log_view);
	//list_insert_after(igViews, NULL, logView);

	//handle_event_in_game
	return screen_new(igViews, mapView, handle_event_in_game);
}
