#ifndef TAXI_PASSENGER_SPRITE_ASSET_H
#define TAXI_PASSENGER_SPRITE_ASSET_H
#include <stdint.h>
typedef struct {uint8_t *pixels;} TaxiPassengerSpriteAsset;
/* Load all32 exported RGBA banks (144frames). Zero-initialize out first.
   Reject wrong file lengths; leave out unchanged on failure. No ROM reads. */
int taxi_passenger_sprites_load(TaxiPassengerSpriteAsset *out,const char *directory);
void taxi_passenger_sprites_free(TaxiPassengerSpriteAsset *asset);
const uint8_t *taxi_passenger_sprite_frame(const TaxiPassengerSpriteAsset *asset,
    unsigned kind,unsigned stage,unsigned frame);
/* Recovered normal scale0xb1400: descriptor32x64, halfwidth>>17,
   height>>16 =>354 by709 raw units. Anchored at actor feet. */
enum {TAXI_PASSENGER_FRAME_BYTES=32*64*4,TAXI_PASSENGER_HALF_WIDTH=177,
      TAXI_PASSENGER_HEIGHT=709};
#endif
