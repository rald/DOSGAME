#ifndef GL2D_H
#define GL2D_H

#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#define bool int
#define false 0
#define true 1

#define VIDEO_INT           0x10      /* the BIOS video interrupt. */
#define SET_MODE            0x00      /* BIOS func to set the video mode. */
#define VGA_256_COLOR_MODE  0x13      /* use to set 256-color mode. */
#define TEXT_MODE           0x03      /* use to set 80x25 text mode. */

#define PALETTE_INDEX       0x03c8
#define PALETTE_DATA        0x03c9
#define INPUT_STATUS        0x03da
#define VRETRACE            0x08

#define SCREEN_WIDTH        320       /* width in pixels of mode 0x13 */
#define SCREEN_HEIGHT       200       /* height in pixels of mode 0x13 */
#define NUM_COLORS          256       /* number of colors in mode 0x13 */

#define DARK				 0
#define DARKBLUE		 1
#define PURPLE			 2
#define BLUE				 3
#define RED					 4
#define LIGHTBLUE		 5
#define ORANGE			 6
#define CYAN				 7
#define YELLOW			 8
#define WHITE				 9
#define LIGHTGREEN	10
#define GRAYBLUE		11
#define GREEN				12
#define DARKGRAY		13
#define TEAL				14
#define DARKERGRAY	15

#define KEY_ARRAY_SIZE 128

#define GL2DK_UP 			0x48
#define GL2DK_LEFT 		0x4B
#define GL2DK_RIGHT 	0x4D
#define GL2DK_DOWN		0x50
#define GL2DK_ESCAPE	0x01

#define MOUSE_INT           0x33
#define MOUSE_RESET         0x00
#define MOUSE_GETPRESS      0x05
#define MOUSE_GETRELEASE    0x06
#define MOUSE_GETMOTION     0x0B

#define GL2D_BUTTON_LEFT         0x00
#define GL2D_BUTTON_RIGHT        0x01
#define GL2D_BUTTON_MIDDLE       0x02

typedef unsigned char  byte;
typedef unsigned short word;
typedef unsigned long  dword;
typedef short sword;

typedef struct {
	byte r,g,b;
} GL2D_Color;

typedef struct {           /* the structure for a mouse. */
	byte on;
  byte button1;
  byte button2;
  byte button3;
  int num_buttons;
  sword x;
  sword y;
} GL2D_Mouse;

extern GL2D_Color sweetie[];
extern byte *GL2D_VGA;
extern word *GL2D_MyClock;

extern volatile unsigned char GL2D_Keys[];

sword GL2D_InitMouse(GL2D_Mouse *mouse);
void GL2D_GetMouseMotion(sword *dx, sword *dy);
sword GL2D_GetMousePress(sword button);
sword GL2D_GetMouseRelease(sword button);

void GL2D_Init(void);
void GL2D_Quit(void);
void GL2D_SetMode(byte mode);
void GL2D_SetSweetiePalette();
void GL2D_DrawPoint(byte *srf,int x,int y,byte color);
void GL2D_DrawLine(byte *srf,int x0,int y0,int x1,int y1,byte color);
void GL2D_DrawRect(byte *srf,int x,int y,int w,int h,byte color);
void GL2D_FillRect(byte *srf,int x,int y,int w,int h,byte color);
void GL2D_DrawCircle(byte *srf,int xc,int yc,int r,byte color);
void GL2D_FillCircle(byte *srf,int xc,int yc,int r,byte color);

#endif	/* GL2D_H */