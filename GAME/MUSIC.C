#include "music.h"

static const Note birthday_song[] = {
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_D, 8}, {NOTE_C, 8}, {NOTE_F, 8}, {NOTE_E, 16},
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_D, 8}, {NOTE_C, 8}, {NOTE_G, 8}, {NOTE_F, 16},
    {NOTE_C, 6}, {NOTE_C, 2}, {NOTE_C5, 8}, {NOTE_A, 8}, {NOTE_F, 8}, {NOTE_E, 8}, {NOTE_D, 8},
    {NOTE_B, 6}, {NOTE_B, 2}, {NOTE_A, 8}, {NOTE_F, 8}, {NOTE_G, 8}, {NOTE_F, 16},
    {0, 16}
};

static int current_note_idx = 0;
static word last_tick = 0;
static int note_timer = 0;

static int laser_active = 0;
static word laser_freq = 0;

void FM_Write(byte reg, byte data) {
    int i;
    outportb(FM_REG_PORT, reg);
    for (i = 0; i < 6; i++) inportb(FM_REG_PORT);
    outportb(FM_DATA_PORT, data);
    for (i = 0; i < 35; i++) inportb(FM_REG_PORT);
}

void Music_Init(void) {
    int i;
    for (i = 0x01; i <= 0xF5; i++) {
        FM_Write(i, 0x00);
    }
    
    FM_Write(0x01, 0x20); 

    /* --- CHANNEL 0: Background Music --- */
    FM_Write(0x20, 0x01); FM_Write(0x40, 0x10); FM_Write(0x60, 0xF0); FM_Write(0x80, 0x77);
    FM_Write(0x23, 0x01); FM_Write(0x43, 0x00); FM_Write(0x63, 0xF0); FM_Write(0x83, 0x77);
    FM_Write(0xC0, 0x01); 

    /* --- CHANNEL 1: Laser Sound Effect --- */
    FM_Write(0x21, 0x01); 
    FM_Write(0x41, 0x15); 
    FM_Write(0x61, 0xF0); 
    FM_Write(0x81, 0xFF); 
    
    FM_Write(0x24, 0x02); 
    FM_Write(0x44, 0x00); 
    FM_Write(0x64, 0xF0); 
    FM_Write(0x84, 0xFF); 
    FM_Write(0xC1, 0x01); 

    last_tick = *GL2D_MyClock;
    current_note_idx = 0;
    note_timer = 0;
    laser_active = 0;
}

void Music_PlayLaser(void) {
    laser_freq = 0x3FF; 
    laser_active = 1;
    
    FM_Write(0xA1, (byte)(laser_freq & 0xFF));
    FM_Write(0xB1, (byte)((laser_freq >> 8) | 0x34));
}

void Music_Update(void) {
    word current_tick = *GL2D_MyClock;
    
    /* 1. Dynamic Pitch Sliding for Laser Effect */
    if (laser_active) {
        if (laser_freq > 0x0A0) {
            laser_freq -= 0x25; 
            FM_Write(0xA1, (byte)(laser_freq & 0xFF));
            FM_Write(0xB1, (byte)((laser_freq >> 8) | 0x34));
        } else {
            FM_Write(0xB1, 0x00);
            laser_active = 0;
        }
    }

    /* 2. Background Track Updates */
    if (current_tick != last_tick) {
        last_tick = current_tick;
        
        if (note_timer > 0) {
            note_timer--;
            return;
        }

        Note n = birthday_song[current_note_idx];
        
        if (n.duration == 16 && n.note == 0) {
            current_note_idx = 0;
            n = birthday_song[0];
        }
        
        if (n.note == 0) {
            FM_Write(0xB0, 0x00);
        } else {
            FM_Write(0xB0, 0x00);
            FM_Write(0xA0, (byte)(n.note & 0xFF));
            FM_Write(0xB0, (byte)((n.note >> 8) | 0x30)); 
        }
        
        note_timer = n.duration;
        current_note_idx++;
    }
}

void Music_Shutdown(void) {
    FM_Write(0xB0, 0x00);
    FM_Write(0xB1, 0x00);
}

