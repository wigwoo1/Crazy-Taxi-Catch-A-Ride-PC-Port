#ifndef TAXI_BODY_POSE_H
#define TAXI_BODY_POSE_H
#include "ground.h"
typedef struct {int16_t angle[8193];} TaxiNormalAngles;
typedef struct {
    int32_t position_734,velocity_73c,position_740,velocity_748;
    int32_t response_704,response_70c,denominator_754,denominator_758;
    TaxiPoint orientation_6f4;
    int16_t velocity_730,velocity_732;
} TaxiBodyPose;
/* 8193 signed little-endian halfwords at ROM 087ec9ec, indexed by normal+4096. */
int taxi_normal_angles_decode(TaxiNormalAngles *out,const uint8_t *bytes,size_t size);
/* Complete 0801488c body spring state. Rejects zero denominators atomically. */
int taxi_body_springs(TaxiBodyPose *state);
/* Complete 08014968 normal alignment. Rejects out-of-table normal components
   atomically; retains halfword wrapping before damping/zero-cross checks. */
int taxi_body_align(TaxiBodyPose *state,TaxiPoint normal,int32_t delta,const TaxiNormalAngles *table);
#endif
