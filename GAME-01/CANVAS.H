#ifndef CANVAS_H
#define CANVAS_H

#include "gl2d.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct {
	int f,w,h,t;
	byte *p;
} Canvas;

Canvas *Canvas_Load(char *fn);
void Canvas_Draw(byte *srf,Canvas *canvas,int f,int x,int y);

#endif
