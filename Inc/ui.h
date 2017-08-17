#ifndef _UI_H_
#define _UI_H_

#include <stdlib.h>   //free
#include <string.h>   //malloc
#include <stdint.h>   //uintXX_t, intXX_t

#include "SDL.h"
typedef SDL_Rect UIRect;
typedef SDL_Event UIEvent;
typedef SDL_Keycode UIKeycode;
#define UI_KEYDOWN SDL_KEYDOWN
#define KEYSYM(e) e.key.keysym.sym

#include "list.h"

typedef uint8_t		u8;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef int8_t		i8;
typedef int32_t		i32;
typedef int64_t		i64;

#define RED(c) ((c & 0xff000000) >> 24)
#define GREEN(c) ((c & 0x00ff0000) >> 16)
#define BLUE(c) ((c & 0x0000ff00) >> 8)
#define ALPHA(c) (c & 0xff)
#define COLOR_FROM_RGBA(r, g, b, a) ((r << 24) | (g << 16) | (b << 8) | a)

//ASCII CHAR
typedef unsigned char asciiChar;

typedef struct {
    asciiChar glyph;
    u32 fgColor;
    u32 bgColor;
} ConsoleCell;

typedef struct {
    u32 *atlas;
    u32 atlasWidth;
    u32 atlasHeight;
    u32 charWidth;
    u32 charHeight;
    asciiChar firstCharInAtlas;
} ConsoleFont;

typedef struct {
    u32 *pixels;      // in-memory representation of the screen pixels
    u32 width;
    u32 height;
    u32 rowCount;
    u32 colCount;
    u32 cellWidth;
    u32 cellHeight;
    ConsoleFont *font;
    ConsoleCell *cells;
} Console;

//---------- Image Start -------------
typedef struct {
    u32 *pixels;
    u32 width;
    u32 height;
} BitmapImage;

typedef struct {
    ConsoleCell *cells;
    u32 rows;
    u32 cols;
} AsciiImage;
//---------- Image End -------------


// UIView need to Implemen Render function
typedef void (*UIRenderFunction)(Console *);
typedef struct {
    Console *console;
    UIRect *pixelRect;
    UIRenderFunction render;
} UIView;

//UIScreen need to Implement Event Handle function
struct UIScreen;
typedef struct UIScreen UIScreen;
typedef void (*UIEventHandler)(struct UIScreen *, UIEvent);
struct UIScreen{
    List *views;
    UIView *activeView;                //Set active View
    UIEventHandler handle_event;
};

//extern UIScreen *activeScreen = NULL;
//extern bool asciiMode;
void ui_set_active_screen(UIScreen *screen);
UIScreen *ui_get_active_screen(void);
void console_clear(Console *con);
UIView *view_new(UIRect pixelRect, u32 cellCountX, u32 cellCountY, char *fontFile, asciiChar firstCharInAtlas, UIRenderFunction renderFn);
UIScreen *screen_new(List *views, UIView *activeView, UIEventHandler handle_event);
void console_put_char_at(Console *con, asciiChar c, i32 cellX, i32 cellY, u32 fgColor, u32 bgColor);
void console_put_string_at(Console *con, char *string, i32 x, i32 y, u32 fgColor, u32 bgColor);
void console_put_string_in_rect(Console *con, char *string, UIRect rect, bool wrap, u32 fgColor, u32 bgColor);
void view_draw_rect(Console *console, UIRect *rect, u32 color, i32 borderWidth, u32 borderColor);
BitmapImage* image_load_from_file(char *filename);
AsciiImage* asciify_bitmap(Console *con, BitmapImage *image);
void view_draw_image_at(Console *console, BitmapImage *image, i32 cellX, i32 cellY);
void view_draw_ascii_image_at(Console *console, AsciiImage *image, i32 cellX, i32 cellY);
void ui_set_active_screen(UIScreen *screen);
#endif
