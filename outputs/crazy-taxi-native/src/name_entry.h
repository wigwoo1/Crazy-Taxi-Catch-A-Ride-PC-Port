#ifndef TAXI_NAME_ENTRY_H
#define TAXI_NAME_ENTRY_H
#include <stdint.h>
typedef struct {int16_t ring[16];int32_t slots[3][2];} TaxiNameLayout;
int taxi_name_layout_load(TaxiNameLayout *layout,const char *path);
typedef struct {uint8_t character,visible,affine;int32_t x,y;int16_t scale;} TaxiNameRingGlyph;
/* Source254bc scale table and25656..25778 glyph requests. Raw X is kept
   for drawing; only the scale lookup clamps to the original0..239 range. */
int taxi_name_ring_glyph(const TaxiNameLayout *layout,uint8_t selection,
    int16_t angle,unsigned slot,uint16_t height,TaxiNameRingGlyph *glyph);
typedef struct {
    uint8_t selection,length,flying,exiting;
    int8_t move,automatic;
    int16_t angle,flight_ticks;
    char name[4];
    int32_t flight_x,flight_y,target_x,target_y,velocity_x,velocity_y;
    int32_t slots[3][2];uint16_t screen_height;
} TaxiNameEntry;
enum { TAXI_NAME_MOVE=1,TAXI_NAME_ACCEPT=2,TAXI_NAME_DELETE=4,TAXI_NAME_FADE_OUT=8 };
int taxi_name_entry_eligible(uint8_t settings,uint8_t rank);
int taxi_name_characters_load(uint8_t characters[43],const char *path);
void taxi_name_entry_begin(TaxiNameEntry *state,const TaxiNameLayout *layout,uint16_t height);
/* Source2584c..25bda logical selection/name writes. Input bytes represent
   original directional/confirm/back edge requests, not raw PC button holds.
   Q8 letter flight follows original slots over16 calls; glyph drawing,
   audio and device edge generation belong to callers. */
unsigned taxi_name_entry_step(TaxiNameEntry *state,const uint8_t characters[43],
    int left,int right,int confirm,int back,int fade_gate);
#endif
