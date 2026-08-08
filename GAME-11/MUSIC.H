#ifndef MUSIC_H
#define MUSIC_H

#include "gl2d.h"

#define FM_REG_PORT  0x388
#define FM_DATA_PORT 0x389

#define NOTE_C   0x16B
#define NOTE_D   0x181
#define NOTE_E   0x1B0
#define NOTE_F   0x1CA
#define NOTE_G   0x202
#define NOTE_A   0x241
#define NOTE_B   0x287
#define NOTE_C5  ((0x16B) | 0x400)

typedef struct {
    word note;
    int duration;
} Note;

void Music_Init(void);
void Music_Update(void);
void Music_PlayLaser(void);  
void Music_Shutdown(void);

#endif

