#ifndef TAXI_WORLD_INDEXED_H
#define TAXI_WORLD_INDEXED_H
#include "world_frame.h"
#include "indexed_asset.h"
#include "pair_column_raster.h"
#include "flat_row_raster.h"
/* Resolve installed callback including depth wrappers' bank0 dispatch.
   Returns0 for unavailable or invalid packet state. Shared by native consumers. */
uint32_t taxi_world_indexed_callback(const TaxiWorldFramePacket *packet,
    const TaxiWorldAsset *world);
/* 1 drawn,0 other family,-1 invalid. Texture1448/1730/198c level0,
   flat1dd4/1f94/20fc material+14 and attribute23d0/2668 material table. */
/* Optional source240 column/unclipped row transfer work; other families report zero.
   Counts native packet work, not original CPU cycles or full-frame fidelity. */
/* Clipped flat family only; original16/32 transfer widths remain separate. */
int taxi_world_indexed_flat_packet_work(uint8_t framebuffer[38400],const TaxiWorldFramePacket *packet,const TaxiWorldAsset *world,TaxiFlatRowWork *work);
int taxi_world_indexed_packet_work(uint8_t framebuffer[38400],
    const TaxiWorldFramePacket *packet,const TaxiWorldAsset *world,
    const TaxiIndexedAsset *textures,const uint8_t lookup[2560],
    int32_t far_depth,int32_t shade_depth,TaxiPairColumnWork *work);
int taxi_world_indexed_packet(uint8_t framebuffer[38400],
    const TaxiWorldFramePacket *packet,const TaxiWorldAsset *world,
    const TaxiIndexedAsset *textures,const uint8_t lookup[2560],
    int32_t far_depth,int32_t shade_depth);
#endif
