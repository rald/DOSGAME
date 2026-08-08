#include "canvas.h"

/* Pre-computed ASCII to Hex lookup table to eliminate runtime loop initialization */
static const char hex_map[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
     0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1, /* '0'-'9' */
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 'A'-'F' */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1, /* 'a'-'f' */
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

Canvas *Canvas_Load(char *fn) {
    FILE *fi;
    Canvas *canvas;
    int c;
    unsigned int total_pixels;
    unsigned int k = 0;

    fi = fopen(fn, "r");
    if (!fi) return NULL;

    canvas = (Canvas *)malloc(sizeof(*canvas)); /* cite: 1 */
    if (!canvas) {
        fclose(fi);
        return NULL;
    }

    if (fscanf(fi, "%d,%d,%d,%d", &canvas->f, &canvas->w, &canvas->h, &canvas->t) != 4) { /* cite: 1 */
        free(canvas);
        fclose(fi);
        return NULL;
    }

    total_pixels = (unsigned int)canvas->f * canvas->w * canvas->h;
    canvas->p = (byte *)malloc(total_pixels);
    if (!canvas->p) {
        free(canvas);
        fclose(fi);
        return NULL;
    }

    while ((c = fgetc(fi)) != EOF && k < total_pixels) { /* cite: 1 */
        char val = hex_map[(unsigned char)c];
        if (val != -1) {
            canvas->p[k++] = (byte)val; /* cite: 1 */
        }
    }

    fclose(fi);
    return canvas;
}

void Canvas_Draw(byte *srf, Canvas *canvas, int f, int x, int y) {
    int cw = canvas->w; /* cite: 1 */
    int ch = canvas->h; /* cite: 1 */
    int start_x = 0; /* cite: 1 */
    int start_y = 0; /* cite: 1 */
    
    /* Force 16-bit registers for critical pointers and loop counters */
    register byte *src;
    register byte *dst;
    register int i;
    int j;
    
    byte transparency = (byte)canvas->t;
    unsigned int src_skip;
    unsigned int dst_skip;

    /* 1. Out-of-loop Vertical Clipping */
    if (y < 0) {
        start_y = -y; /* cite: 1 */
        ch -= start_y; /* cite: 1 */
        y = 0; /* cite: 1 */
    }
    if (y + ch > 200) {
        ch = 200 - y; /* cite: 1 */
    }
    if (ch <= 0) return; /* cite: 1 */

    /* 2. Out-of-loop Horizontal Clipping */
    if (x < 0) {
        start_x = -x; /* cite: 1 */
        cw -= start_x; /* cite: 1 */
        x = 0; /* cite: 1 */
    }
    if (x + cw > 320) {
        cw = 320 - x; /* cite: 1 */
    }
    if (cw <= 0) return; /* cite: 1 */

    /* 3. Pre-calculate source offset without costly 32-bit long math */
    src = canvas->p + ((unsigned int)f * canvas->w * canvas->h) + ((unsigned int)start_y * canvas->w) + start_x;

    /* VGA 320-wide offset optimization: (y * 320) -> (y << 8) + (y << 6) */
    dst = srf + ((unsigned int)y << 8) + ((unsigned int)y << 6) + x;

    src_skip = canvas->w - cw; /* cite: 1 */
    dst_skip = 320 - cw; /* cite: 1 */

    /* 4. Optimized drawing loop with pointer post-increments */
    for (j = 0; j < ch; j++) {
        /* Loop unrolling 2x to reduce instruction overhead on 8086/80286 */
        i = cw;
        while (i >= 2) {
            byte pixel = *src++;
            if (pixel != transparency) *dst = pixel;
            dst++;

            pixel = *src++;
            if (pixel != transparency) *dst = pixel;
            dst++;

            i -= 2;
        }

        /* Clean up leftover pixel if width is odd */
        if (i) {
            byte pixel = *src++;
            if (pixel != transparency) *dst = pixel;
            dst++;
        }

        src += src_skip; /* cite: 1 */
        dst += dst_skip; /* cite: 1 */
    }
}
