#ifndef TAXI_CHALLENGE_BALL_PALETTE_H
#define TAXI_CHALLENGE_BALL_PALETTE_H
#include <stdint.h>
/* Full 8-frame original indexed atlas using the registered scene palette.
   Source E154 retains the requested palette at registry+16 even when initial
   DMA picks a default. Entry E5A8(1,32,1,1) uses channel caps from that source.
   This helper covers that cap path only; initial DMA/registry ownership,
   preserved-palette masks and hardware brightness remain caller-owned. */
int taxi_challenge_ball_palette_rgba(uint8_t out[32768],const uint8_t indices[8192],const uint16_t registered[256],int16_t cap);
#endif
