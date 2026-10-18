#ifndef TAXI_DESTINATION_GUIDANCE_H
#define TAXI_DESTINATION_GUIDANCE_H
#include "traction.h"
/* Original0800e034 returns whole-unit distance outside destination radius;
   -1 for no selected destination.0800e090 returns camera-relative angle.
   Camera pointer is an explicit original transform, not vehicle heading. */
int32_t taxi_destination_distance(int32_t selected,const uint8_t *record,int32_t x,int32_t z);
/* Original8020738..20756: palette bank and vertical scale near destination. */
unsigned taxi_destination_palette(int32_t distance);
uint16_t taxi_destination_vertical_scale(int32_t distance);
/* Original801f088..f0dc signed halfword scale follow and overshoot clamp. */
uint16_t taxi_guidance_scale_step(uint16_t current,uint16_t target,int32_t delta);
typedef struct {int32_t x0,z0,x1,z1,unit_x,unit_z,length;} TaxiGuidanceSegment;
typedef struct {TaxiGuidanceSegment segments[80];unsigned count;} TaxiGuidanceRoute;
int taxi_guidance_route_load(TaxiGuidanceRoute *out,const char *path);
/* Original203C4 scene lookup of the first ARROW string, then existing closed
   segment math. Immutable descriptor/point bytes are data only. Count resets
   before lookup; unused segment slots retain their prior values, including a
   scene with no ARROW route. Missing native data returns-1, never invents a path. */
int taxi_guidance_scene(TaxiGuidanceRoute *,const uint8_t *,size_t,const uint8_t *,size_t);
/* Original8020714/ARM0800531c. Current/target are quantized32frame indices.
   Retains original asymmetric rounding; does not add invented fractions. */
unsigned taxi_guidance_frame_step(unsigned current,unsigned target,int32_t delta);
/* Original80203c4 closed ARROW loop generation. Includes last-to-first edge.
   Points are projected original route X/Z; no path generation or guesses. */
int taxi_guidance_segments(TaxiGuidanceSegment *out,const TaxiPoint *points,unsigned count);
int taxi_guidance_route_angle(const TaxiGuidanceSegment *segments,unsigned count,
    int32_t selected,const uint8_t *destination,TaxiPoint taxi,TaxiPoint camera,
    int32_t camera_word_10,const TaxiTrig *trig,const TaxiAngleTable *table,int32_t *angle);
int taxi_destination_angle(int32_t selected,const uint8_t *record,TaxiPoint camera,
    int32_t camera_word_10,const TaxiTrig *trig,const TaxiAngleTable *table,int32_t *angle);
#endif
