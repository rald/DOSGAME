#ifndef FONT_H
#define FONT_H

#include "gl2d.h"
#include "canvas.h"

void Font_DrawChar(byte *srf, Canvas *canvas, char c, int x, int y);
void Font_DrawText(byte *srf, Canvas *canvas, char *t, int x, int y);

#endif