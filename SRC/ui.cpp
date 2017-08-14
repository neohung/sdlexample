/*
* list.c
*/

#include "ui.h"

UIScreen *activeScreen = NULL;
bool asciiMode = true;

void ui_set_active_screen(UIScreen *screen) {
    activeScreen = screen;
}

UIScreen *ui_get_active_screen(void) {
    return activeScreen;
}

void ui_fill(u32 *pixels, u32 pixelsPerRow, UIRect *destRect, u32 color)
{
    u32 stopX = destRect->x + destRect->w;
    u32 stopY = destRect->y + destRect->h;

    for (u32 dstY = destRect->y; dstY < stopY; dstY++) {
        for (u32 dstX = destRect->x; dstX < stopX; dstX++) {
            pixels[(dstY * pixelsPerRow) + dstX] = color;
        }
    }
}

void console_clear(Console *con) {
    UIRect r = {0, 0, con->width, con->height};
    ui_fill(con->pixels, con->width, &r, 0x000000ff);
}
