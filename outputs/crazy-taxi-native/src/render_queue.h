#ifndef TAXI_RENDER_QUEUE_H
#define TAXI_RENDER_QUEUE_H
#include <stddef.h>
#include <stdint.h>
/* World031ec: wrapped vertex-Z sum; triangles use signed divide3,
   quads use arithmetic shift2. Depth is calculated before clipping. */
int taxi_world_polygon_depth(const int32_t *vertex_z,unsigned count,int32_t *depth);
typedef struct {size_t next,payload;unsigned kind;} TaxiRenderPacket;
typedef struct {size_t heads[128],count,capacity;TaxiRenderPacket *packets;} TaxiRenderQueue;
void taxi_render_queue_init(TaxiRenderQueue *queue,TaxiRenderPacket *storage,size_t capacity);
/* Original source producers prepend; consumer2b6a0 visits127..0. */
int taxi_render_queue_push(TaxiRenderQueue *queue,unsigned bucket,unsigned kind,size_t payload);
typedef int (*TaxiRenderDispatch)(void *context,unsigned kind,size_t payload);
int taxi_render_queue_consume(TaxiRenderQueue *queue,TaxiRenderDispatch dispatch,void *context);
#endif
