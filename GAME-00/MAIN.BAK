#include "gl2d.h"

int main(void) {
	bool quit=false;
	int x=SCREEN_WIDTH/2,y=SCREEN_HEIGHT/2,speed=2;
	sword dx=0,dy=0;
	byte *buf;
	GL2D_Mouse mouse;
	Canvas *spinCanvas;

	srand(*GL2D_MyClock);                   /* seed the number generator. */

	GL2D_Init();

	GL2D_SetMode(VGA_256_COLOR_MODE);       /* set the video mode. */

	GL2D_SetSweetiePalette();

	GL2D_InitMouse(&mouse);

	buf=calloc(SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(*buf));

	spinCanvas=Canvas_Load("spin.cvs");

	while(!quit) {

		if(GL2D_Keys[GL2DK_ESCAPE])	quit=true;
		if(GL2D_Keys[GL2DK_UP])			y-=speed;
		if(GL2D_Keys[GL2DK_DOWN])		y+=speed;
		if(GL2D_Keys[GL2DK_LEFT])		x-=speed;
		if(GL2D_Keys[GL2DK_RIGHT])	x+=speed;

		if(x<0) x=0;
		if(y<0) y=0;
		if(x>=SCREEN_WIDTH-10) x=SCREEN_WIDTH-10;
		if(y>=SCREEN_HEIGHT-10) y=SCREEN_HEIGHT-10;

		GL2D_GetMouseMotion(&dx,&dy);
		mouse.x+=dx;
		mouse.y+=dy;

		if(mouse.x<5) mouse.x=5;
		if(mouse.y<5) mouse.y=5;
		if(mouse.x>=SCREEN_WIDTH-5) mouse.x=SCREEN_WIDTH-6;
		if(mouse.y>=SCREEN_HEIGHT-5) mouse.y=SCREEN_HEIGHT-6;

		memset(buf,0,SCREEN_WIDTH*SCREEN_HEIGHT);

		GL2D_DrawLine(buf,0,0,319,199,9);
		GL2D_DrawLine(buf,319,0,0,199,9);

		GL2D_FillRect(buf,x,y,10,10,9);

		if(GL2D_GetMousePress(GL2D_BUTTON_LEFT)) {
			mouse.button1=1;
		}

		if(GL2D_GetMouseRelease(GL2D_BUTTON_LEFT)) {
			mouse.button1=0;
		}

		if(mouse.button1) {
			GL2D_FillCircle(buf,mouse.x,mouse.y,5,9);
		} else {
			GL2D_DrawCircle(buf,mouse.x,mouse.y,5,9);
		}

		memcpy(GL2D_VGA,buf,SCREEN_WIDTH*SCREEN_HEIGHT);
	}

	free(buf);

	GL2D_SetMode(TEXT_MODE);                /* set the video mode back to
																				 text mode. */
	GL2D_Quit();

	return 0;
}
