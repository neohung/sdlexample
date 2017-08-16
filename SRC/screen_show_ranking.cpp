
Config *rankConfig = NULL;
UIScreen *ranking_screen = NULL;

void render_ranking_entries(Console *console) {

	// Loop through all HoF entries, extract the data into a formatted string, and write to screen
	i32 y = 10;

	ListElement *e = list_head(rankConfig->entities);
	while (e != NULL) {
		ConfigEntity *entity = (ConfigEntity *)e->data;

		char *name = config_entity_value(entity, (char*)"name");
		char *level = config_entity_value(entity, (char*)"level");
		char *gems = config_entity_value(entity, (char*)"gems");
		char *date = config_entity_value(entity, (char*)"date");

		char *nameString = String_Create("%20s", name);
		console_put_string_at(console, nameString, 16, y, 0xe2f442ff, 0x00000000);
		char *dateString = String_Create("%10s", date);
		console_put_string_at(console, dateString, 36, y, 0xeeeeeeff, 0x00000000);
		char *levelString = String_Create("Level:%2s", level);
		console_put_string_at(console, levelString, 47, y, 0xffd700ff, 0x00000000);
		char *gemString = String_Create("Gems:%s", gems);
		console_put_string_at(console, gemString, 56, y, 0xdb99fcff, 0x00000000);

		String_Destroy(nameString);
		String_Destroy(dateString);
		String_Destroy(levelString);
		String_Destroy(gemString);

		y += 2;
		e = list_next(e);
	}

}

void render_ranking_bg_view(Console *console)
{
	// We should load and process the bg image only once, not on each render
	BitmapImage *bgImage = NULL;
	AsciiImage *aiImage = NULL;
	if (bgImage == NULL) {
		bgImage = image_load_from_file((char*)"./launch.png");
		aiImage = asciify_bitmap(console, bgImage);
	}

	if (asciiMode) {
		view_draw_ascii_image_at(console, aiImage, 0, 0);
	} else {
		view_draw_image_at(console, bgImage, 0, 0);
	}

	UIRect rect = {10, 5, 60, 34};
	view_draw_rect(console, &rect, 0x363247dd, 2, 0xaad700ff);

	console_put_string_at(console, (char*)"-== Ranking ==-", 28, 7, 0xaad700ff, 0x00000000);

	render_ranking_entries(console);

    console_put_string_at(console, (char*)"-== TEST MSG ==-", 31, 32, 0xaad700ff, 0x00000000);
	console_put_string_at(console, (char*)"This is a test Message, test the message to show", 15, 34, 0xffffffff, 0x00000000);

}

UIScreen *screen_test(void);
void handle_event_ranking(UIScreen *activeScreen, UIEvent event)
{
	if (event.type == UI_KEYDOWN) {
		UIKeycode key = KEYSYM(event);
		switch (key) {
			case SDLK_ESCAPE: {
				ui_set_active_screen(screen_test());
			}
			break;
			default:
				break;
		}
	}
}

UIScreen *screen_show_ranking()
{
	if (ranking_screen != NULL)
		return ranking_screen;
	List *views = list_new(NULL);

	UIRect bgRect = {0, 0, (16 * BG_WIDTH), (16 * BG_HEIGHT)};
	UIView *bgView = view_new(bgRect, BG_WIDTH, BG_HEIGHT, (char*)"./terminal16x16.png", 0, render_ranking_bg_view);
	list_insert_after(views, NULL, bgView);
	if (rankConfig == NULL) {
		rankConfig = config_file_parse((char*)"ranking.txt");
	}
    //handle_event_hof
	return screen_new(views, bgView, handle_event_ranking);
}

