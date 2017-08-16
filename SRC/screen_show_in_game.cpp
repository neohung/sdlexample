
//Config *rankConfig = NULL;
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

		ListElement *e = list_head(visibilityComps);
		printf("render_game_map_view: %p\n",visibilityComps);
		//Visibility *vis = (Visibility *)list_data(e);
		//printf("render_game_map_view: %d, [%c]",vis->objectId, vis->glyph);
		//while (e != NULL) {
			//Visibility *vis = (Visibility *)list_data(e);

			//Position *p = (Position *)game_object_get_component(&gameObjects[vis->objectId], COMP_POSITION);
			//printf("p%d, %d, %d, %d\n",p->objectId,p->layer,p->x,p->y);
		//	e = list_next(e);
		//}

}

void handle_event_in_game(UIScreen *activeScreen, UIEvent event)
{
	if (event.type == UI_KEYDOWN) {
		UIKeycode key = KEYSYM(event);
		switch (key) {
			case SDLK_ESCAPE: {
				//ui_set_active_screen(screen_test());
				quit_game();
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
