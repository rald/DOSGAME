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
    int cw = canvas->w;
    int ch = canvas->h;
    int start_x = 0;
    int start_y = 0;
    byte transparency = canvas->t;
    byte *src;
    byte *dst;
    int src_skip;
    int dst_skip;
    register int i;
    register int j;

    /* 1. Out-of-loop Vertical Clipping */
    if (y < 0) {
        start_y = -y;
        ch -= start_y;
        y = 0;
    }
    if (y + ch > 200) {
        ch = 200 - y;
    }
    if (ch <= 0) return; /* Fully off-screen vertically */

    /* 2. Out-of-loop Horizontal Clipping */
    if (x < 0) {
        start_x = -x;
        cw -= start_x;
        x = 0;
    }
    if (x + cw > 320) {
        cw = 320 - x;
    }
    if (cw <= 0) return; /* Fully off-screen horizontally */

    /* 3. Calculate precise starting memory offsets */
    src = canvas->p + ((long)f * canvas->w * canvas->h) + ((long)start_y * canvas->w) + start_x;
    dst = srf + ((long)y * 320) + x;
    
    /* Pre-calculate row stride skips */
    src_skip = canvas->w - cw;
    dst_skip = 320 - cw;

    /* 4. Ultra-lean rendering loops */
    for (j = 0; j < ch; j++) {
        for (i = 0; i < cw; i++) {
            register byte pixel = *src++;
            if (pixel != transparency) {
                *dst = pixel;
            }
            dst++;
        }
        src += src_skip;
        dst += dst_skip;
    }
}
