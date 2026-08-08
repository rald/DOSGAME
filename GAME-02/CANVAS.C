#include "canvas.h"

Canvas *Canvas_Load(char *fn) {
    FILE *fi;
    Canvas *canvas;
    int c;
    long total_pixels;
    long k = 0;
    
    /* FAST ASCII to Hex lookup map to replace the 16-iteration loop */
    static char hex_map[256];
    static int map_inited = 0;
    if (!map_inited) {
        int i;
        for (i = 0; i < 256; i++) hex_map[i] = -1;
        for (i = 0; i < 10; i++)  hex_map['0' + i] = i;
        for (i = 0; i < 6; i++) {
            hex_map['A' + i] = 10 + i;
            hex_map['a' + i] = 10 + i; /* Case insensitivity bonus */
        }
        map_inited = 1;
    }

    fi = fopen(fn, "r");
    if (!fi) return NULL;

    canvas = malloc(sizeof(*canvas));
    if (!canvas) {
        fclose(fi);
        return NULL;
    }

    if (fscanf(fi, "%d,%d,%d,%d", &canvas->f, &canvas->w, &canvas->h, &canvas->t) != 4) {
        free(canvas);
        fclose(fi);
        return NULL;
    }

    total_pixels = (long)canvas->f * canvas->w * canvas->h;
    canvas->p = calloc((size_t)total_pixels, sizeof(*canvas->p));
    if (!canvas->p) {
        free(canvas);
        fclose(fi);
        return NULL;
    }

    while ((c = fgetc(fi)) != EOF && k < total_pixels) {
        char val = hex_map[c];
        if (val != -1) {
            canvas->p[k++] = (byte)val;
        }
    }

    fclose(fi);
    return canvas;
}

void Canvas_Draw(byte *srf, Canvas *canvas, int f, int x, int y) {
    int i, j;
    int cw = canvas->w;
    int ch = canvas->h;
    byte transparency = canvas->t;
    
    /* Calculate starting address of the frame's sprite data */
    byte *src = canvas->p + ((long)f * cw * ch);
    
    /* Calculate starting address on the destination surface */
    /* SCREEN_WIDTH is 320, which is (y << 8) + (y << 6) */
    byte *dst_row = srf + ((long)y * 320) + x;

    for (j = 0; j < ch; j++) {
        /* Check bounds or handle vertical screen clipping if needed */
        if ((y + j) >= 200 || (y + j) < 0) continue; 

        for (i = 0; i < cw; i++) {
            byte pixel = src[i];
            /* Only draw if it's within horizontal screen bounds and not transparent */
            if (pixel != transparency && (x + i) >= 0 && (x + i) < 320) {
                dst_row[i] = pixel;
            }
        }
        src += cw;         /* Next row in sprite data */
        dst_row += 320;    /* Next row in video buffer */
    }
}
