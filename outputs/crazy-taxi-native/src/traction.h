#ifndef TAXI_TRACTION_H
#define TAXI_TRACTION_H
#include "orientation.h"
typedef struct { int16_t values[257]; } TaxiAngleTable;
typedef struct {
    int32_t force_53c,force_540,capacity_570,capacity_574,speed_544,state_564;
    int8_t phase_e6a,phase_e6b;
    uint8_t flags_28;
} TaxiTraction;
typedef struct {
    uint8_t settings,flag_1696,flag_1697,flag_25d4,flag_262c;
    int32_t state_2630;
} TaxiTractionContext;
/* Decode 514 bytes at ROM 087f09ee. No ROM instructions are executed. */
void taxi_angle_table_decode(TaxiAngleTable *table,const uint8_t bytes[514]);
/* Original 0802e00c direction convention. Rejects out-of-table wrapped ratios. */
int taxi_vector_angle(int32_t x,int32_t z,const TaxiAngleTable *table,int32_t *angle);
/* 08014394, including speed damping. Reward playback is a returned request.
   Invalid lookup leaves all state and effects unchanged. */
int taxi_traction_step(TaxiTraction *v,TaxiTractionContext *context,
    const TaxiTrig *trig,const TaxiAngleTable *table,TaxiOrientationEffects *effects);
typedef void (*TaxiTractionObserver)(const TaxiTraction *,const TaxiTractionContext *,const TaxiTraction *,const TaxiTractionContext *,const TaxiOrientationEffects *);
void taxi_traction_observer(TaxiTractionObserver);
#endif
