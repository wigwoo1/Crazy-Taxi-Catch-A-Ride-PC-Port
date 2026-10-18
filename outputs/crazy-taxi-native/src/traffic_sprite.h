#ifndef TAXI_TRAFFIC_SPRITE_H
#define TAXI_TRAFFIC_SPRITE_H
#include <stdint.h>
typedef struct {uint16_t palette[256];uint8_t pixels[16*128*64];} TaxiTrafficSprites;
int taxi_traffic_sprites_load(TaxiTrafficSprites *sprites,const char *filename);
/* Source6890..6898; direction angle from original2e00c caller. */
unsigned taxi_traffic_sprite_frame(int32_t camera_yaw,int32_t actor_heading,int32_t direction);
/* Keyed original indices expanded using caller's current display palette.
   Embedded palette may be supplied for diagnostics, not assumed at runtime. */
int taxi_traffic_sprite_rgba(const TaxiTrafficSprites *sprites,unsigned frame,
    const uint16_t palette[256],uint8_t rgba[128*64*4]);
#endif
