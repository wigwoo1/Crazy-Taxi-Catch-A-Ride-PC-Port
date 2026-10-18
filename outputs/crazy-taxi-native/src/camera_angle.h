#ifndef TAXI_CAMERA_ANGLE_H
#define TAXI_CAMERA_ANGLE_H
#include "impact.h"
/* 8011ae0..1b62: command byte5 bit0 holds rear view; bit1 cycles
   0->2->3->0, with1->2. Returns whether rear-view state changed. */
int taxi_camera_command(uint16_t *flags_64c,uint8_t command_byte5);
/* 8015640..56d6: transform anchor-camera by negative output angles,
   wrapped Q8 Z*X*Y matrix; signed perspective division, source centre.
   Returns0 for zero depth without changing output. */
int taxi_camera_project(TaxiPoint *screen,TaxiPoint anchor,TaxiPoint camera,
    int16_t pitch,int16_t yaw,int16_t roll,int32_t focal,const TaxiTrig *trig);
/* Yaw subroutine 0801530e..080153f2. Inputs are original fixed-point
   angles; alternate_limit corresponds to global02001696. */
int32_t taxi_camera_yaw_step(int32_t camera,int32_t heading,int32_t delta,
    int alternate_limit,const TaxiTrig *trig);
/* Pitch subroutine0801524e..0801530e tracks negative body angle18. */
int32_t taxi_camera_pitch_step(int32_t camera,int16_t body_pitch,int32_t delta,const TaxiTrig *trig);
/* 80154f8..5502 writes camera498+e (02002ada at carbase2634);
   800b358/80295a8 copies its low12bits to guidance transform e18+10. */
int32_t taxi_camera_transform_yaw(int32_t camera_fixed,int32_t offset_2618);
/* Normal80154cc..5548: rotate(0,height778 ASR12,
   base680-distance77c ASR12) by tracked pitch/yaw, then add taxi XYZ.
   Extra yaw2618 applies to the transform angle, not this offset rotation. */
TaxiPoint taxi_camera_position(TaxiPoint taxi,int32_t pitch_fixed,int32_t yaw_fixed,
    int32_t height_778,int32_t distance_77c,int32_t base_680,const TaxiTrig *trig);
/* Rear-view branch8015488..54b8 shares tracked pitch/yaw position
   rotation, but outputs pitch136/yaw(tracked ASR12)+2048/roll0.
   offset690 is a raw coordinate, not a fixed-point distance. */
TaxiPoint taxi_camera_rear_position(TaxiPoint taxi,int32_t pitch_fixed,
    int32_t yaw_fixed,int32_t offset_690,const TaxiTrig *trig);
typedef struct {int32_t zoom_2628,height_76c,distance_770;} TaxiCameraOffsets;
/* Normal mode2 target offsets8014ebc..4f74; current778/77c copytargets
   at8015226..523e, unconditionally overwriting earlier quarter-step smoothing.
   Keep source target offsets directly; do not add offset easing here.
   flags4ac bit5 forces negative zoom. */
void taxi_camera_mode2_offsets(TaxiCameraOffsets *state,int32_t scale_e84,
    uint8_t flags_4ac,int32_t delta,uint8_t freeze,int32_t preset_y,int32_t preset_z);
/* 14DF6..14E4C selection; caller supplies live human count. Returns mode0..3
   or -1 for bypass/unsupported. Demo count mutation remains caller-owned. */
int taxi_camera_select_mode(uint16_t *flags_64c,int32_t human_count,int demo);
/* Fixed target offsets for source modes0/1 only. Does not execute2C880,
   tracking, interpolation or hardware setup. Outputs unchanged on failure. */
int taxi_camera_fixed_offsets(unsigned mode,TaxiCameraOffsets *out,int32_t *roll_768);
/* 14FF6..15034 mode3 target values only; base660/offset670 are raw
   coordinates. Does not compose mode3 view matrices or select this mode. */
void taxi_camera_mode3_offsets(TaxiCameraOffsets *state,int32_t base_660,
    int32_t offset_670,int32_t *roll_768,uint8_t *flags_29);
typedef struct {TaxiPoint position;int16_t pitch,yaw,roll;} TaxiCameraView;
/* Ordinary154CC..15548 final view; output pitch uses ASR12, yaw includes
   global2618 while position rotation does not. */
TaxiCameraView taxi_camera_normal_view(TaxiPoint taxi,int32_t pitch_fixed,
    int32_t yaw_fixed,int32_t yaw_offset,const TaxiCameraOffsets *offsets,
    int32_t base_680,const TaxiTrig *trig);
/* 15036..150C8 mode3 view; corrections x=738,y=69c,z=744.
   Uses original Y*X*Z matrix, translation then local(0,345,offset670 ASR1). */
TaxiCameraView taxi_camera_mode3_view(TaxiPoint taxi,int16_t body_pitch,
    int32_t body_heading,int16_t body_roll,TaxiPoint corrections,
    int32_t offset_670,const TaxiTrig *trig);
/* 153F4..15432: signed target approach by delta*32, clamp on crossing. */
int32_t taxi_camera_yaw_offset_step(int32_t current,int32_t target,int32_t delta);
TaxiCameraView taxi_camera_rear_view(TaxiPoint taxi,int32_t pitch_fixed,int32_t yaw_fixed,int32_t offset_690,const TaxiTrig *trig);
/* Source rear branch547A..5486 clears actor29 bit1 (maskFD). */
TaxiCameraView taxi_camera_mode2_view(TaxiPoint taxi,int32_t pitch,int32_t yaw,int32_t yaw_offset,const TaxiCameraOffsets *offsets,int32_t base,int32_t rear_offset,uint16_t mode_flags,uint8_t *actor_flags_29,const TaxiTrig *trig);
#endif



