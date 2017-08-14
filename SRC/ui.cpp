/*
* list.c
*/

#include "ui.h"
#include "stb_image.h"

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

Console *console_new(i32 width, i32 height, i32 rowCount, i32 colCount) {
    Console *con = (Console *)malloc(sizeof(Console));
    con->pixels = (u32*)calloc(width * height, sizeof(u32));
    con->width = width;
    con->height = height;
    con->rowCount = rowCount;
    con->colCount = colCount;
    con->cellWidth = width / colCount;
    con->cellHeight = height / rowCount;
    con->font = NULL;
    con->cells =  (ConsoleCell*)calloc(rowCount * colCount, sizeof(ConsoleCell));
    return con;
}

bool system_is_little_endian() {
    unsigned int x = 0x76543210;
    char *c = (char*) &x;
    if (*c == 0x10) { return true; }
    return false;
}

#define SWAP_U32(x) (((x) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | ((x) << 24))

void console_set_bitmap_font(Console *con, char *filename,
                             asciiChar firstCharInAtlas,
                             i32 charWidth, i32 charHeight) {
    // Load the image data
    int imgWidth, imgHeight, numComponents;
    unsigned char *imgData = stbi_load(filename,&imgWidth, &imgHeight,&numComponents, STBI_rgb_alpha);

    // Copy the image data so we can hold onto it
    u32 imgDataSize = imgWidth * imgHeight * sizeof(u32);
    u32 *atlasData = (u32 *)malloc(imgDataSize);
    //memcpy(atlasData, imgData, imgDataSize);

    // Swap endianness of data if we need to
    u32 pixelCount = imgWidth * imgHeight;
    if (system_is_little_endian()) {
        for (u32 i = 0; i < pixelCount; i++) {
            atlasData[i] = SWAP_U32(atlasData[i]);
        }
    }

    // Turn all black pixels into transparent pixels
    for (u32 i = 0; i < pixelCount; i++) {
        if (atlasData[i] == 0x000000ff) {
            atlasData[i] = 0x00000000;
        }
    }

    // Create and configure the font
    ConsoleFont *font = (ConsoleFont *)malloc(sizeof(ConsoleFont));
    font->atlas = atlasData;
    font->charWidth = charWidth;
    font->charHeight = charHeight;
    font->atlasWidth = imgWidth;
    font->atlasHeight = imgHeight;
    font->firstCharInAtlas = firstCharInAtlas;

    //stbi_image_free(imgData);

    if (con->font != NULL) {
        free(con->font->atlas);
        free(con->font);
    }
    con->font = font;
}

UIView *view_new(UIRect pixelRect, u32 cellCountX, u32 cellCountY, char *fontFile, asciiChar firstCharInAtlas, UIRenderFunction renderFn) {
	UIView *view = (UIView *)malloc(sizeof(UIView));
	UIRect *rect = (UIRect *)malloc(sizeof(UIRect));
	memcpy(rect, &pixelRect, sizeof(UIRect));
	Console *console = console_new(rect->w, rect->h, cellCountY, cellCountX);
	i32 cellWidthPixels = pixelRect.w / cellCountX;
	i32 cellHeightPixels = pixelRect.h / cellCountY;
	console_set_bitmap_font(console, fontFile, firstCharInAtlas, cellWidthPixels, cellHeightPixels);
	view->console = console;
	view->pixelRect = rect;
	view->render = renderFn;
	return view;
}
