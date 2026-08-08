#include "music.h"

/* Happy Birthday Melody Sheet */
static const Note birthday_song[] = {
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_D, 8}, {NOTE_C, 8}, {NOTE_F, 8}, {NOTE_E, 16},
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_D, 8}, {NOTE_C, 8}, {NOTE_G, 8}, {NOTE_F, 16},
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_C5, 8}, {NOTE_A, 8}, {NOTE_F, 8}, {NOTE_E, 8}, {NOTE_D, 8},
    {NOTE_B, 6}, {NOTE_B, 2}, {NOTE_A, 8}, {NOTE_F, 8}, {NOTE_G, 8}, {NOTE_F, 16},
    {0, 16} /* Loop delimiter */
};

static int current_note_idx = 0;
static word last_tick = 0;
static int note_timer = 0;

/* Hardware delay write to OPL2 Registers */
void FM_Write(byte reg, byte data) {
    int i;
    outportb(FM_REG_PORT, reg);
    for (i = 0; i < 6; i++) inportb(FM_REG_PORT); /* Delay for register select */
    outportb(FM_DATA_PORT, data);
    for (i = 0; i < 35; i++) inportb(FM_REG_PORT); /* Delay for data write[cite: 7] */
}

void Music_Init(void) {
    int i;
    
    /* 1. Reset all registers */
    for (i = 0x01; i <= 0xF5; i++) {
        FM_Write(i, 0x00);
    }
    
    /* 2. Enable OPL2 Waveform Control */
    FM_Write(0x01, 0x20); 

    /* 3. Configure Channel 0 Sound Parameters (Crisp, clear organ/piano sound) */
    FM_Write(0x20, 0x01); /* Modulator: Multiple = 1 */
    FM_Write(0x40, 0x10); /* Modulator: Medium Volume Level */
    FM_Write(0x60, 0xF0); /* Modulator: Fast Attack / Fast Decay */
    FM_Write(0x80, 0x77); /* Modulator: Medium Sustain / Release */
    
    FM_Write(0x23, 0x01); /* Carrier: Multiple = 1 */
    FM_Write(0x43, 0x00); /* Carrier: Max Volume (Ensures audibility) */
    FM_Write(0x63, 0xF0); /* Carrier: Fast Attack / Fast Decay */
    FM_Write(0x83, 0x77); /* Carrier: Medium Sustain / Release */
    
    /* 4. Connect Modulator and Carrier (Feedback = 0) */
    FM_Write(0xC0, 0x01); 

		last_tick = *GL2D_MyClock;
    current_note_idx = 0;
    note_timer = 0;
}

void Music_Update(void) {
		word current_tick = *GL2D_MyClock;
		Note n;

    if (current_tick != last_tick) {
        last_tick = current_tick;

        if (note_timer > 0) {
            note_timer--;
            return;
        }

				n = birthday_song[current_note_idx];

        /* If we reach the end of the song array, loop it */
        if (n.duration == 16 && n.note == 0) {
            current_note_idx = 0;
            n = birthday_song[0];
        }

        if (n.note == 0) {
            /* Rest note: Turn Key-OFF */
            FM_Write(0xB0, 0x00);
        } else {
            /* FIXED: Force Key-OFF first to cleanly finish the last note */
            FM_Write(0xB0, 0x00);

            /* Send frequency low bits */
            FM_Write(0xA0, (byte)(n.note & 0xFF));

            /* Turn Key-ON + high frequency bits + Octave Block 4 (0x10) */
            FM_Write(0xB0, (byte)((n.note >> 8) | 0x30));
        }

        note_timer = n.duration;
        current_note_idx++;
    }
}

void Music_Shutdown(void) {
    FM_Write(0xB0, 0x00); /* Silence channel */
}

