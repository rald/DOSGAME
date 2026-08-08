#include "gl2d.h"

GL2D_Color sweetie[] = { /* Sweetie-16 Color Palette By GrafxKid */
	{   6,   7,  11}, /*  0 - Dark           */
	{  10,  13,  27}, /*  1 - Dark Blue      */
	{  23,   9,  23}, /*  2 - Purple         */
	{  14,  23,  50}, /*  3 - Blue           */
	{  44,  15,  20}, /*  4 - Red            */
	{  16,  41,  61}, /*  5 - Light Blue     */
	{  59,  31,  21}, /*  6 - Orange         */
	{  28,  59,  61}, /*  7 - Cyan           */
	{  63,  51,  29}, /*  8 - Yellow         */
	{  61,  61,  61}, /*  9 - White          */
	{  41,  60,  28}, /* 10 - Light Green    */
	{  37,  44,  48}, /* 11 - Gray Blue      */
	{  14,  45,  25}, /* 12 - Green          */
	{  21,  27,  33}, /* 13 - Dark Gray Blue */
	{   9,  28,  30}, /* 14 - Teal           */
	{  12,  15,  21}  /* 15 - Darker Gray    */
};


byte *GL2D_VGA=(byte *)0xA0000000L;     /* this points to video memory. */
word *GL2D_MyClock=(word *)0x0000046C;  /* this points to the 18.2hz system
																					 clock. */

volatile unsigned char GL2D_Keys[KEY_ARRAY_SIZE];
static void interrupt (*OldInt9)(void);

static void interrupt NewInt9(void) {
	byte scancode=inportb(0x60);
	if(scancode & 0x80) {
		GL2D_Keys[scancode & 0x7F]=0;
	} else {
		GL2D_Keys[scancode]=1;
	}
	OldInt9();
}

sword GL2D_InitMouse(GL2D_Mouse *mouse) {
  sword dx,dy;
  union REGS regs;

  regs.x.ax = MOUSE_RESET;
  int86(MOUSE_INT, &regs, &regs);
  mouse->on=regs.x.ax;
  mouse->num_buttons=regs.x.bx;
  mouse->button1=0;
  mouse->button2=0;
  mouse->button3=0;
  mouse->x=SCREEN_WIDTH/2;
  mouse->y=SCREEN_HEIGHT/2;
	GL2D_GetMouseMotion(&dx,&dy);
  return mouse->on;
}

void GL2D_GetMouseMotion(sword *dx, sword *dy) {
  union REGS regs;

  regs.x.ax = MOUSE_GETMOTION;
  int86(MOUSE_INT, &regs, &regs);
  *dx=regs.x.cx;
  *dy=regs.x.dx;
}

sword GL2D_GetMousePress(sword button) {
  union REGS regs;

  regs.x.ax = MOUSE_GETPRESS;
  regs.x.bx = button;
  int86(MOUSE_INT, &regs, &regs);
  return regs.x.bx;
}

sword GL2D_GetMouseRelease(sword button) {
  union REGS regs;

  regs.x.ax = MOUSE_GETRELEASE;
  regs.x.bx = button;
  int86(MOUSE_INT, &regs, &regs);
  return regs.x.bx;
}

void GL2D_VWait(void) {
    /* wait until done with vertical retrace */
    while  ((inp(INPUT_STATUS) & VRETRACE));
    /* wait until done refreshing */
    while (!(inp(INPUT_STATUS) & VRETRACE));
}

void GL2D_Init(void) {
	int i;
	for(i=0;i<KEY_ARRAY_SIZE;i++) GL2D_Keys[i]=0;
	OldInt9=getvect(0x09);
	setvect(0x09,NewInt9);
}

void GL2D_Quit(void) {
	setvect(0x09,OldInt9);

	while(kbhit()) {
		getch();
	}
}

void GL2D_SetMode(byte mode) {
	union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  int86(VIDEO_INT, &regs, &regs);
}

void GL2D_SetSweetiePalette() {
	int i;

	outp(PALETTE_INDEX,0);              /* tell the VGA that palette data
                                         is coming. */

	for(i=0;i<16;i++) {
		outp(PALETTE_DATA,sweetie[i].r);
		outp(PALETTE_DATA,sweetie[i].g);
		outp(PALETTE_DATA,sweetie[i].b);
	}

	for(i=16;i<256;i++) {
		outp(PALETTE_DATA,sweetie[0].r);
		outp(PALETTE_DATA,sweetie[0].g);
		outp(PALETTE_DATA,sweetie[0].b);
	}
}

void GL2D_DrawPoint(byte *srf,int x,int y,byte color) {
	srf[y*SCREEN_WIDTH+x]=color;
}

void GL2D_DrawLine(byte *srf,int x0,int y0,int x1,int y1,byte color) {
	int i;
	byte *p=srf+(y0*SCREEN_WIDTH)+x0;
	int dx=abs(x1-x0);
	int dy=abs(y1-y0);
	int xs=(x1>=x0)?1:-1;
	int ys=(y1>=y0)?SCREEN_WIDTH:-SCREEN_WIDTH;

	if(dx>=dy) {
		int d=2*dy-dx;
		int ie=2*dy;
		int in=2*(dy-dx);

		for(i=0;i<=dx;i++) {
			*p=color;
			if(d>=0) {
				p+=ys;
				d+=in;
			} else {
				d+=ie;
			}
			p+=xs;
		}
	} else {
		int d=2*dx-dy;
		int ie=2*dx;
		int in=2*(dx-dy);
		for(i=0;i<=dy;i++) {
			*p=color;
			if(d>=0) {
				p+=xs;
				d+=in;
			} else {
				d+=ie;
			}
			p+=ys;
		}
	}
}

void GL2D_DrawRect(byte *srf,int x,int y,int w,int h,byte color) {
	int i;
	byte *tr=srf+(y*SCREEN_WIDTH)+x;
	byte *br=srf+((y+h-1)*SCREEN_WIDTH)+x;
	byte *le=tr+SCREEN_WIDTH;
	byte *re=tr+SCREEN_WIDTH+(w-1);

	memset(tr,color,w);
	memset(br,color,w);

	for(i=1;i<h-1;i++) {
		*le=color;
		*re=color;
		le+=SCREEN_WIDTH;
		re+=SCREEN_WIDTH;
	}
}

void GL2D_FillRect(byte *srf,int x,int y,int w,int h,byte color) {
	int i;
	byte *dst=srf+(y<<8)+(y<<6)+x;
	for(i=0;i<h;i++) {
		memset(dst,color,w);
		dst+=320;
	}
}

void GL2D_DrawCircle(byte *srf,int xc,int yc,int r,byte color) {
	int x=0;
	int y=r;
	int d=3-(2*r);
	byte *cr=srf+(yc*SCREEN_WIDTH);

	while(x<=y) {
		byte *rpy=cr+(y*SCREEN_WIDTH);
		byte *rmy=cr-(y*SCREEN_WIDTH);
		byte *rpx=cr+(x*SCREEN_WIDTH);
		byte *rmx=cr-(x*SCREEN_WIDTH);

		*(rpy+xc+x)=color;
		*(rpy+xc-x)=color;
		*(rmy+xc+x)=color;
		*(rmy+xc-x)=color;

		*(rpx+xc+y)=color;
		*(rpx+xc-y)=color;
		*(rmx+xc+y)=color;
		*(rmx+xc-y)=color;

		if(d<0) {
			d+=(4*x)+6;
		} else {
			d+=(4*(x-y))+10;
			y--;
		}
		x++;
	}
}

void GL2D_FillCircle(byte *srf,int xc,int yc,int r,byte color) {
	int x=0;
	int y=r;
	int d=3-(2*r);
	byte *cr=srf+(yc*SCREEN_WIDTH);

	while(x<=y) {
		byte *rpy=cr+(y*SCREEN_WIDTH);
		byte *rmy=cr-(y*SCREEN_WIDTH);
		byte *rpx=cr+(x*SCREEN_WIDTH);
		byte *rmx=cr-(x*SCREEN_WIDTH);

		memset(rpy+xc-x,color,(2*x)+1);
		memset(rmy+xc-x,color,(2*x)+1);

		memset(rpx+xc-y,color,(2*y)+1);
		memset(rmx+xc-y,color,(2*y)+1);

		if(d<0) {
			d+=(4*x)+6;
		} else {
			d+=(4*(x-y))+10;
			y--;
		}
		x++;
	}
}
