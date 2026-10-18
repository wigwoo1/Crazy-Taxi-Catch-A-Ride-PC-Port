#ifndef TAXI_NORMAL_MENU_BACKGROUND_H
#define TAXI_NORMAL_MENU_BACKGROUND_H
#include <stdint.h>
typedef struct {unsigned width,height;int ox,oy;const uint8_t *indices;} TaxiMenuStrip;
/* Source BG0/BG1 at zero scroll, group3 strips. Text/footer excluded. */
int taxi_normal_menu_background(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned count,unsigned selected,unsigned cap);
int taxi_normal_menu_background_motion(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned count,unsigned selected,unsigned cap,int32_t slide_q12);
/* Existing shared BG0/BG1 zero-scroll composition without row sprites.
   Uses the same qualified8/8blend as normal_menu_background. */
int taxi_menu_background_only(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],unsigned cap);
/* Nine grid cells only; profile row strips/text/footer separate.
   grid_sprites is12288bytes from original frame11/12/13 all2banks.
   Uses bank0 and existingqualified8/8 semi-transparent OBJ model. */
int taxi_challenge_grid_buttons(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint8_t grid_sprites[12288],
    unsigned selected,uint16_t badge_mask,unsigned cap);
int taxi_challenge_profile_panels(uint8_t output[153600],const uint8_t front[131072],
    const uint8_t back[131072],const uint16_t palette[256],const uint16_t obj[16],
    const TaxiMenuStrip strips[6],unsigned selected,unsigned cap);
#endif
