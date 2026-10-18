#ifndef TAXI_IMPACT_H
#define TAXI_IMPACT_H
#include "ground.h"
typedef struct { int32_t sine[4096],cosine[4096]; } TaxiTrig;
typedef struct {
    uint16_t angle_18,angle_1a;
    int32_t angle_1c,angle_69c,speed_544;
    int32_t state_748,state_73c,state_74c,state_750,state_34;
} TaxiImpactState;
typedef struct {
    int32_t projected_x,projected_z;
    unsigned sound_table_index,effect_slot;
} TaxiImpactEffects;
/* Decode original 8192-byte tables at 084a0d08 and 084a2d08. */
void taxi_trig_decode(TaxiTrig *out,const uint8_t *sine,const uint8_t *cosine);
/* Original Y*X*Z rotation and eight-fractional-bit vector transform. */
TaxiPoint taxi_rotate_point(TaxiPoint point,int32_t angle_x,int32_t angle_y,
    int32_t angle_z,const TaxiTrig *trig);
/* 08000aa8 camera rotation: Z*X*Y, retaining intermediate Q8 rounding. */
TaxiPoint taxi_rotate_camera(TaxiPoint point,int32_t angle_x,int32_t angle_y,
    int32_t angle_z,const TaxiTrig *trig);
/* 08000b78 plus 08000a1c: one Y matrix, without extra X/Z products. */
TaxiPoint taxi_rotate_y(TaxiPoint point,int32_t angle,const TaxiTrig *trig);
/* State-changing arithmetic of 0802a6f4, plus 0802abf4 classification.
   Special response is selected by (settings & 0xe0)==0x80 && submode==0.
   Sound/effect outputs are requests only: audio, effect spawning and random
   camera-shake side effects remain unported. */
void taxi_vehicle_impact(TaxiImpactState *state,TaxiPoint normal,
    uint8_t settings,uint8_t submode,const TaxiTrig *trig,TaxiImpactEffects *effects);
#endif
