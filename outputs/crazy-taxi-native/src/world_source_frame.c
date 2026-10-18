#include "world_source_frame.h"
#include <stdlib.h>
#include <string.h>
int taxi_world_source_frame(uint8_t output[38400],const TaxiSourceFrameConfig *config,const TaxiWorldFrame *world,TaxiSourceFrameResult *result){
 if(!output||!config||!world||!world->world||!world->textures||!world->packets||!world->count||world->count>4096||!result)return -1;
 TaxiSourceFramePacket *packets=calloc(world->count,sizeof *packets);if(!packets)return -1;
 for(size_t i=0;i<world->count;i++){
  const TaxiWorldFramePacket *p=world->packets+i;TaxiSourceFramePacket *q=packets+i;
  if(p->kind!=TAXI_RENDER_WORLD||!p->raster_flags_valid||p->material>=world->world->nm||p->texture==255||(p->raster_flags&1)||p->raw_polygon.count!=((p->raster_flags&16)?3u:4u)){free(packets);return -1;}
  q->bucket=p->bucket;q->flags=p->raster_flags;q->material_flags=world->world->materials[28*p->material+20];q->level=0;
  if(taxi_indexed_asset_texture(world->textures,p->texture,&q->texture)){free(packets);return -1;}
  memcpy(q->vertices,p->raw_polygon.vertices,sizeof q->vertices);
 }
 int status=taxi_source_frame_native(output,config,packets,world->count,result);free(packets);return status;
}
