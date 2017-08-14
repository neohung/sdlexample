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
#define SCREEN_WIDTH	320 //1280
#define SCREEN_HEIGHT	240 //720


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
	printf("head=%p\n",e);
    while (e != NULL) {
    	//UIView *v = (UIView *)list_data(e);
    	//console_clear(v->console);
    	//SDL_UpdateTexture(screenTexture, v->pixelRect, v->console->pixels, v->pixelRect->w * sizeof(u32));
    	e = list_next(e);
    	//SDL_Delay(1000);
    }
	printf("tail=%p\n",e);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, screenTexture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


global_variable bool gameIsRunning = true;
void quit_game() {
	gameIsRunning = false;
}

// test
UIScreen *screen_test(void)
{
	//
	List *testViews = list_new(NULL);
	int a;
	//list_insert_after(testViews, NULL, &a);

	UIScreen *testScreen = (UIScreen *)malloc(sizeof(UIScreen));
	testScreen->views = testViews;
	return testScreen;
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
    				case SDLK_q:
    				case SDLK_ESCAPE: {
    					printf("Press q or ESC\n");
    					quit_game();
    					}
    			    	break;
    				case SDLK_t: {
    					//asciiMode = !asciiMode;
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
    			//UIScreen *screenForInput = ui_get_active_screen();
    			//screenForInput->handle_event(screenForInput, event);
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
