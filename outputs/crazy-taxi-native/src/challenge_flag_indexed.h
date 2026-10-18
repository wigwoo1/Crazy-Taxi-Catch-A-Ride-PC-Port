#ifndef TAXI_CHALLENGE_FLAG_INDEXED_H
#define TAXI_CHALLENGE_FLAG_INDEXED_H
#include "challenge_flag_projection.h"
#include "indexed_pixel.h"
/* Original31e20..31e60 UV order for the unflipped32x64 flag.
   Packet32/36 selects1730/1448 respectively. Source-resolution consumer;
   palette interpretation and live world queue remain caller-owned. */
int taxi_challenge_flag_vertices(const TaxiChallengeFlagProjection *projection,TaxiIndexedVertex out[4]);
int taxi_challenge_flag_indexed(uint8_t framebuffer[38400],const TaxiChallengeFlagProjection *projection,const uint8_t indices[2048]);
/* Native widened affine span producer for the existing axis shader.
   Even bounds [-1024,1024],160 source rows; 0/-1 preserve output.
   Widened side visibility must already have been resolved by caller. */
int taxi_challenge_flag_rows(TaxiIndexedSpan spans[160],const TaxiChallengeFlagProjection *projection,int32_t left,int32_t right);
#endif
