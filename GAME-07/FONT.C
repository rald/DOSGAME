#include "font.h"

void Font_DrawChar(byte *srf, Canvas *canvas, char c, int x, int y) {
    static int font_map[256];
		static int map_inited = 0;
		int idx;

    if (!map_inited) {
				int i;
        char *let = "ABCDEFGHIJKLMNOPQRSTUVWXYZ .,!?()[]{}\'\"_-+=|/\\<>:;#^*0123456789~`";
        for (i = 0; i < 256; i++) font_map[i] = -1;
        for (i = 0; let[i]; i++) {
            font_map[(unsigned char)let[i]] = i;
        }
        map_inited = 1;
    }

		idx = font_map[(unsigned char)c];
    if (idx != -1) {
        Canvas_Draw(srf, canvas, idx, x, y);
    }
}

void Font_DrawText(byte *srf, Canvas *canvas, char *t, int x, int y) {
    int i;
    int start_x = x;
    int char_w = canvas->w + 1;
    int char_h = canvas->h + 1;

    for (i = 0; t[i]; i++) {
        if (t[i] == '\n') {
            x = start_x;
            y += char_h;
        } else {
            Font_DrawChar(srf, canvas, t[i], x, y);
            x += char_w;
            if (x + char_w >= 320) {
                x = start_x;
                y += char_h;
            }
        }
    }
}
