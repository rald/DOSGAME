#include "canvas.h"

Canvas *Canvas_Load(char *fn) {
	Canvas *canvas=malloc(sizeof(*canvas));

	FILE *fi,*fo;
	int c;
	char *hex="0123456789ABCDEF";
	int i,j,k;

	fi=fopen(fn,"r");
	fscanf(fi,"%d,%d,%d,%d",&canvas->f,&canvas->w,&canvas->h,&canvas->t);
	canvas->p=calloc(canvas->f*canvas->w*canvas->h,sizeof(*canvas->p));

	k=0;
	while((c=fgetc(fi))!=EOF) {
		j=-1;
		for(i=0;i<16;i++) {
			if(c==hex[i]) {
				j=i;
				break;
			}
		}
		if(j!=-1) {
			canvas->p[k++]=(byte)j;
		}
	}
	fclose(fi);

	return canvas;
}

void Canvas_Draw(byte *srf,Canvas *canvas,int f,int x,int y) {
	int i,j,k;
	for(i=0;i<canvas->w;i++) {
		for(j=0;j<canvas->h;j++) {
			k=canvas->p[f*canvas->w*canvas->h+j*canvas->w+i];
			if(k!=canvas->t) {
				GL2D_DrawPoint(srf,x+i,y+j,k);
			}
		}
  }
}
