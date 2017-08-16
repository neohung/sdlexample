#include "SDL.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    //time()
#include <stdbool.h>
#include <stdint.h>

#include "list.h"
#include "ui.h"
#include "config.h"
#include "game.h"

typedef uint8_t		u8;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef int8_t		i8;
typedef int32_t		i32;
typedef int64_t		i64;

#ifdef __MINGW32__
#define OS "MINGW32"
#undef main /* Prevents SDL from overriding main() */
#else
#define OS ""
#endif

#define global_variable static
#define internal static
#define FPS_LIMIT		20
#define SCREEN_WIDTH	1280 //320 //1280
#define SCREEN_HEIGHT	720 //240 //720

bool asciiMode = true;
//Render the Screen from UIScreen
internal void render_screen(SDL_Renderer *renderer, SDL_Texture *screenTexture, UIScreen *screen)
{
	// Render views from back to front for the current screen
    if (screen == NULL){
    	printf("Can't get any Screen to Render!\n");
    	return;
    }
    if (screen->views == NULL){
        printf("Can't get any View in Screen to Render!\n");
        return;
    }
    ListElement *e = list_head(screen->views);
	//printf("head=%p\n",e);
    while (e != NULL) {
    	UIView *v = (UIView *)list_data(e);
    	console_clear(v->console);
        v->render(v->console);
    	SDL_UpdateTexture(screenTexture, v->pixelRect, v->console->pixels, v->pixelRect->w * sizeof(u32));
    	e = list_next(e);
    	//SDL_Delay(1000);
    }
	//printf("tail=%p\n",e);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


global_variable bool gameIsRunning = true;
void quit_game() {
	gameIsRunning = false;
}

// test
#define BG_WIDTH    80
#define BG_HEIGHT   45

#define MENU_LEFT	7//50
#define MENU_TOP	5//28
#define MENU_WIDTH	24
#define MENU_HEIGHT	10

void render_menu_view(Console *console) 
{
    UIRect rect = {0, 0, MENU_WIDTH, MENU_HEIGHT};
    //view_draw_rect(console, &rect, 0x363247FF, 0, 0xFFFFFFFF);
    view_draw_rect(console, &rect, 0x363247FF, 1, 0xFF000088);
    console_put_string_at(console, (char*)"Start a (N)ew game", 2, 3, 0xbca285FF, 0x00000000);
    console_put_string_at(console, (char*)"View (R)anking", 2, 6, 0xbca285FF, 0x00000000);
}

void render_bg_view(Console *console)  
{

    // We should load and process the bg image only once, not on each render
    BitmapImage *bgImage = NULL;
    AsciiImage *aiImage = NULL;
    if (bgImage == NULL) {
        bgImage = image_load_from_file((char*)"./launch.png");
        aiImage = asciify_bitmap(console, bgImage);
    }

    if (asciiMode) {
    //	printf("asciiMode=%d\n",asciiMode);
        view_draw_ascii_image_at(console, aiImage, 0, 0);
    } else {
   // 	printf("asciiMode=%d\n",asciiMode);
        view_draw_image_at(console, bgImage, 0, 0);
    }

    //console_put_string_at(console, "Dark Caverns", 52, 18, 0x556d76FF, 0x00000000);
    console_put_string_at(console, (char*)"Dark Caverns", 2, 18, 0x556d76FF, 0x00000000);

}

#include "screen_show_ranking.cpp"
#include "screen_show_in_game.cpp"
void handle_event_launch(UIScreen *activeScreen, UIEvent event)
{
    if (event.type == UI_KEYDOWN) {
		UIKeycode key = KEYSYM(event);
		switch (key) {
			case SDLK_n: {
				// Start a new game and transition to in-game screen
				game_new();
				ui_set_active_screen(screen_show_in_game());
				//currentlyInGame = true;
				}
				break;
			case SDLK_r: {
				ui_set_active_screen(screen_show_ranking());
				}
				break;
			case SDLK_ESCAPE: {
				quit_game();
				}
				break;
			default:
				break;
		}
	}
}

UIScreen *test_screen = NULL;

UIScreen *screen_test(void)
{
	if (test_screen != NULL)
			return test_screen;
	//
	List *testViews = list_new(NULL);
	UIRect menuRect = {(16 * MENU_LEFT), (16 * MENU_TOP), (16 * MENU_WIDTH), (16 * MENU_HEIGHT)};
	UIView *menuView = view_new(menuRect, MENU_WIDTH, MENU_HEIGHT,(char*)"./terminal16x16.png", 0, render_menu_view);
	list_insert_after(testViews, NULL, menuView);

    UIRect bgRect = {0, 0, (16 * BG_WIDTH), (16 * BG_HEIGHT)};
    UIView *bgView = view_new(bgRect, BG_WIDTH, BG_HEIGHT, (char*)"./terminal16x16.png", 0, render_bg_view);
    list_insert_after(testViews, NULL, bgView);

	return screen_new(testViews, menuView, handle_event_launch);
}
//

int main(int argc, char* argv[]) {

	srand((unsigned)time(NULL));
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window *window = SDL_CreateWindow("Test",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			SCREEN_WIDTH, SCREEN_HEIGHT,
			0);  //SDL_WINDOW_OPENGL

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    //Create Render
    SDL_Renderer *renderer = SDL_CreateRenderer(window, 0, SDL_RENDERER_SOFTWARE);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_Texture *screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

    //Before running loop, init activeScreen
    ui_set_active_screen(screen_test());

    bool currentlyInGame = false;
    while (gameIsRunning) {
    	SDL_Event event;
    	u32 timePerFrame = 1000 / FPS_LIMIT;
    	u32 frameStart = 0;
    	while (SDL_PollEvent(&event) != 0) {
    		frameStart = SDL_GetTicks();
    		if (event.type == SDL_QUIT) {
    			printf("Quit\n");
    			quit_game();
    			break;
    		}
    		if (event.type == SDL_KEYDOWN) {
    			SDL_Keycode key = event.key.keysym.sym;
    			switch (key) {
    				//case SDLK_q:
    				//case SDLK_ESCAPE: {
    				//	printf("Press q or ESC\n");
    				//	quit_game();
    				//	}
    			    //	break;
    				case SDLK_t: {
    					asciiMode = !asciiMode;
    					//printf("Press t, asciiMode=%d\n",asciiMode);
    					}
    					break;
    				case SDLK_SPACE: {
    					printf("Press SPACE\n");
    					}
    					break;
    				case SDLK_RIGHT: {
    				    printf("Press RIGHT\n");
    					}
    					break;
    				case SDLK_LEFT: {
    					printf("Press LEFT\n");
    					}
    					break;
    				case SDLK_UP: {
    					printf("Press UP\n");
    					}
    					break;
    				case SDLK_DOWN: {
    					printf("Press DOWN\n");
    					}
    					break;
    				default:
    					break;
    			}
    			// Send the event to the currently active screen for handling
    			UIScreen *screenForInput = ui_get_active_screen();
    			if (screenForInput->handle_event != NULL)
    				screenForInput->handle_event(screenForInput, event);
    		}
    	}
    	if (currentlyInGame) {
    		//game_update();
    	}
    	//Render active screen
    	render_screen(renderer, screenTexture, ui_get_active_screen());
    	//
    	i32 sleepTime = timePerFrame - (SDL_GetTicks() - frameStart);
    	if (sleepTime > 0) {
    		SDL_Delay(sleepTime);
    	}
    }

    //SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example
    // Close and destroy the window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    // Clean up
    SDL_Quit();
    return 0;
}
