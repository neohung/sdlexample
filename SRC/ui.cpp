/*
* list.c
*/

#include "ui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//#include "stb_image_write.h" 
//#include "stb_image.h"

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
    int imgWidth=0;
    int imgHeight=0;
    int numComponents=0;
    
    unsigned char *imgData = stbi_load(filename,&imgWidth, &imgHeight,&numComponents, STBI_rgb_alpha);
    //printf("w=%d, h=%d, c=%d\n",imgWidth , imgHeight, numComponents);
    if ((imgWidth == 0) || (imgHeight == 0) || (numComponents == 0)){
        printf("ERROR: w=%d, h=%d, c=%d\n",imgWidth , imgHeight, numComponents);
        exit(-1);
    }
    // Copy the image data so we can hold onto it
    u32 imgDataSize = imgWidth * imgHeight * sizeof(u32);
    u32 *atlasData = (u32 *)malloc(imgDataSize);
    memcpy(atlasData, imgData, imgDataSize);
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
    stbi_image_free(imgData);
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

UIRect rect_get_for_glyph(asciiChar c, ConsoleFont *font) {
    i32 idx = c - font->firstCharInAtlas;
    i32 charsPerRow = (font->atlasWidth / font->charWidth);
    i32 xOffset = (idx % charsPerRow) * font->charWidth;
    i32 yOffset = (idx / charsPerRow) * font->charHeight;
    UIRect glyphRect = {xOffset, yOffset, font->charWidth, font->charHeight};
    return glyphRect;
}

void ui_fill_blend(u32 *pixels, u32 pixelsPerRow, UIRect *destRect, u32 color)
{
    // For each pixel in the destination rect, alpha blend the 
    // bgColor to the existing color.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    u32 stopX = destRect->x + destRect->w;
    u32 stopY = destRect->y + destRect->h;

    // If the color we're trying to blend is transparent, then bail
    if (ALPHA(color) == 0) return;

    float srcA = ALPHA(color) / 255.0;
    float invSrcA = 1.0 - srcA;

    // Otherwise, blend each pixel in the dest rect
    for (u32 dstY = destRect->y; dstY < stopY; dstY++) {
        for (u32 dstX = destRect->x; dstX < stopX; dstX++) {
            u32 *pixel = &pixels[(dstY * pixelsPerRow) + dstX];

            if (ALPHA(color) == 255) {
                // Just copy the color, no blending necessary
                *pixel = color;
            } else {
                // Do alpha blending
                u32 pixelColor = *pixel;

                float destA = ALPHA(pixelColor) / 255.0;

                float outAlpha = srcA + (destA * invSrcA);
                u8 fRed = ((RED(color) * srcA) + (RED(pixelColor) * destA * invSrcA)) / outAlpha;
                u8 fGreen = ((GREEN(color) * srcA) + (GREEN(pixelColor) * destA * invSrcA)) / outAlpha;
                u8 fBlue = ((BLUE(color) * srcA) + (BLUE(pixelColor) * destA * invSrcA)) / outAlpha;
                u8 fAlpha = outAlpha * 255;

                *pixel = COLOR_FROM_RGBA(fRed, fGreen, fBlue, fAlpha);
            }
        }
    }
}

inline u32 ui_colorize_pixel(u32 dest, u32 src) 
{
    // Colorize the destination pixel using the source color
    if (ALPHA(dest) == 255) {
        return src;
    } else if (ALPHA(dest) > 0) {
        // Scale the final alpha based on both dest & src alphas
        return COLOR_FROM_RGBA(RED(src), 
                               GREEN(src), 
                               BLUE(src), 
                               (u8)(ALPHA(src) * (ALPHA(dest) / 255.0)));
    } else {
        return dest;
    }
}

void ui_copy_blend(u32 *destPixels, UIRect *destRect, u32 destPixelsPerRow,
              u32 *srcPixels, UIRect *srcRect, u32 srcPixelsPerRow,
              u32 *newColor)
{
    // If src and dest rects are not the same size ==> bad things
    assert(destRect->w == srcRect->w && destRect->h == srcRect->h);

    // For each pixel in the destination rect, alpha blend to it the 
    // corresponding pixel in the source rect.
    // ref: https://en.wikipedia.org/wiki/Alpha_compositing

    u32 stopX = destRect->x + destRect->w;
    u32 stopY = destRect->y + destRect->h;

    for (u32 dstY = destRect->y, srcY = srcRect->y; 
         dstY < stopY; 
         dstY++, srcY++) {
        for (u32 dstX = destRect->x, srcX = srcRect->x; 
             dstX < stopX; 
             dstX++, srcX++) {

            u32 srcColor = srcPixels[(srcY * srcPixelsPerRow) + srcX];
            u32 *destPixel = &destPixels[(dstY * destPixelsPerRow) + dstX];
            u32 destColor = *destPixel;

            // Colorize our source pixel before we blend it
            srcColor = ui_colorize_pixel(srcColor, *newColor);

            if (ALPHA(srcColor) == 0) {
                // Source is transparent - so do nothing
                continue;
            } else if (ALPHA(srcColor) == 255) {
                // Just copy the color, no blending necessary
                *destPixel = srcColor;
            } else {
                // Do alpha blending
                float srcA = ALPHA(srcColor) / 255.0;
                float invSrcA = (1.0 - srcA);
                float destA = ALPHA(destColor) / 255.0;

                float outAlpha = srcA + (destA * invSrcA);
                u8 fRed = ((RED(srcColor) * srcA) + (RED(destColor) * destA * invSrcA)) / outAlpha;
                u8 fGreen = ((GREEN(srcColor) * srcA) + (GREEN(destColor) * destA * invSrcA)) / outAlpha;
                u8 fBlue = ((BLUE(srcColor) * srcA) + (BLUE(destColor) * destA * invSrcA)) / outAlpha;
                u8 fAlpha = outAlpha * 255;

                *destPixel = COLOR_FROM_RGBA(fRed, fGreen, fBlue, fAlpha);
            }
        }
    }
}
void console_put_char_at(Console *con, asciiChar c, 
                    i32 cellX, i32 cellY,
                    u32 fgColor, u32 bgColor) {

    i32 x = cellX * con->cellWidth;
    i32 y = cellY * con->cellHeight;
    UIRect destRect = {x, y, con->cellWidth, con->cellHeight};

    // Fill the background with alpha blending
    ui_fill_blend(con->pixels, con->width, &destRect, bgColor);

    // Copy the glyph with alpha blending and desired coloring
    UIRect srcRect = rect_get_for_glyph(c, con->font);
    ui_copy_blend(con->pixels, &destRect, con->width, con->font->atlas, &srcRect, con->font->atlasWidth, &fgColor);
}

void console_put_string_at(Console *con, char *string, 
                      i32 x, i32 y,
                      u32 fgColor, u32 bgColor) {
    i32 len = strlen(string);
    for (i32 i = 0; i < len; i++) {
        console_put_char_at(con, (asciiChar)string[i], x+i, y, fgColor, bgColor);
    }
}

void console_put_string_in_rect(Console *con, char *string,
                           UIRect rect, bool wrap, 
                           u32 fgColor, u32 bgColor) {
    u32 len = strlen(string);
    i32 x = rect.x;
    i32 x2 = x + rect.w;
    i32 y = rect.y;
    i32 y2 = y + rect.h;
    for (u32 i = 0; i < len; i++) {
        bool shouldPut = true;
        if (x >= x2) {
            if (wrap) {
                x = rect.x;
                y += 1;
            } else {
                shouldPut = false;
            }
        }
        if (y >= y2) {
            shouldPut = false;
        }
        if (shouldPut) {
            console_put_char_at(con, (asciiChar)string[i], 
                                  x, y, fgColor, bgColor);
            x += 1;
        }
    }
}

