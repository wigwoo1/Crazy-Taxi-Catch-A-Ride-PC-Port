#ifndef TAXI_SOURCE_FRAME_NATIVE_H
#define TAXI_SOURCE_FRAME_NATIVE_H
#include "world_clip.h"
#include "indexed_pixel.h"
#include "indexed_asset.h"
typedef struct {unsigned bucket;uint16_t flags;uint8_t material_flags;unsigned level;TaxiIndexedTexture texture;TaxiWorldClipVertex vertices[4];} TaxiSourceFramePacket;
typedef struct {const uint8_t *pixels;size_t size;unsigned height;int32_t pitch,yaw;uint32_t fill;TaxiTrafficProjection projection;TaxiTrafficScreenBounds bounds;} TaxiSourceFrameConfig;
typedef struct {unsigned callbacks,halfword_writes;} TaxiSourceFrameResult;
/* Recovered indexed240x160 world frame. Prepared packets/texture owners supplied
   by caller; unsupported material families fail. No instruction execution. */
int taxi_source_frame_native(uint8_t frame[38400],const TaxiSourceFrameConfig *,const TaxiSourceFramePacket *,size_t count,TaxiSourceFrameResult *);
#endif
