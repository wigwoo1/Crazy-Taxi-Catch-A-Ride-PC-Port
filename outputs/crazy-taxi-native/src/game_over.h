#ifndef TAXI_GAME_OVER_H
#define TAXI_GAME_OVER_H
#include <stdint.h>
typedef struct {
    int32_t position[16],velocity[16],current_x,center;
    int16_t scale,hold;
    uint8_t count,glyph,steps,exiting;
} TaxiGameOver;
/* Original240f4..241ce. Widths are original fbac suffix measurements,
   including full string at0. Input viewport width is original logical240. */
int taxi_game_over_begin(TaxiGameOver *state,unsigned count,unsigned width,
    const uint16_t suffix_widths[16]);
/* Original242c4..2436a. Eight calls per glyph; Q8 scale +32/call and
   position += precomputed velocity. After all glyphs, signed hold>60
   requests original fade-out once. Returns1 for that request. */
int taxi_game_over_step(TaxiGameOver *state,int32_t delta,int fade_gate);
#endif
