#include "source_frame_native.h"
#include "render_queue.h"
#include "raster_dispatch.h"
#include "engine_callback_identity.h"
#include "world_background.h"
#include <stdlib.h>
typedef struct {uint8_t *frame;const TaxiSourceFrameConfig *config;const TaxiSourceFramePacket *packets;size_t count;TaxiSourceFrameResult result;} Context;
static int draw(void *opaque,unsigned kind,size_t index){
 Context *c=opaque;if(kind||index>=c->count)return -1;
 const TaxiSourceFramePacket *p=c->packets+index;
 unsigned n=taxi_raster_dispatch(p->flags).vertices;
 uint32_t routine=taxi_engine_callback_identity(taxi_raster_callback_global(p->flags,p->texture.pixels!=0,p->material_flags));
 if(routine==0x080049f8||routine==0x08004a94){
  const TaxiTrafficScreenBounds *b=&c->config->bounds;
  if(n!=4||p->level||b->left!=0||b->top!=0||b->right!=240||b->bottom!=160)return -1;
  TaxiPairColumnWork work={0};
  if(!taxi_indexed_billboard_work(c->frame,p->vertices[1].screen_x,p->vertices[1].screen_y,p->vertices[3].screen_x,p->vertices[3].screen_y,p->texture.pixels,p->texture.size,p->texture.width,p->texture.height,p->texture.log_width,routine==0x080049f8,&work))return -1;
  c->result.callbacks++;c->result.halfword_writes+=work.framebuffer_halfword_writes;return 0;
 }
 TaxiIndexedVertex v[8];
 if(routine==0x08004704){
  TaxiWorldClippedPolygon clipped={0};int status=taxi_world_clip_depth(p->vertices,n,c->config->projection.far_plane,1,&clipped);if(status<0)return -1;
  c->result.callbacks++;if(!status)return 0;
  const TaxiTrafficScreenBounds *b=&c->config->bounds;
  status=taxi_world_clip_project(&clipped,&c->config->projection,b->left,b->top,b->right,b->bottom);if(status<0)return -1;if(!status)return 0;
  n=clipped.count;for(unsigned i=0;i<n;i++){TaxiWorldClipVertex w=clipped.vertices[i];v[i]=(TaxiIndexedVertex){w.screen_x,w.screen_y,(uint32_t)w.u,(uint32_t)w.v,(uint32_t)w.z};}
  routine=0x08001448;
 }else{
  c->result.callbacks++;for(unsigned i=0;i<n;i++){TaxiWorldClipVertex w=p->vertices[i];v[i]=(TaxiIndexedVertex){w.screen_x,w.screen_y,(uint32_t)w.u,(uint32_t)w.v,(uint32_t)w.z};}
 }
 TaxiIndexedSampling sampling;if(!taxi_indexed_sampling(&sampling,p->texture.width,p->texture.height,p->texture.log_width,p->level))return -1;
 TaxiPairColumnWork work={0};int ok=0;
 if(routine==0x08000c5c)ok=taxi_indexed_perspective_rows_polygon_work(c->frame,v,n,p->texture.pixels,p->texture.size,sampling,&work);
 else if(routine==0x08001448)ok=taxi_indexed_unshaded_clipped_polygon_work(c->frame,v,n,p->texture.pixels,p->texture.size,sampling,&work);
 else if(routine==0x08001730)ok=taxi_indexed_unshaded_polygon_work(c->frame,v,n,p->texture.pixels,p->texture.size,sampling,&work);
 if(!ok)return -1;c->result.halfword_writes+=work.framebuffer_halfword_writes;return 0;
}
int taxi_source_frame_native(uint8_t frame[38400],const TaxiSourceFrameConfig *cfg,const TaxiSourceFramePacket *packets,size_t count,TaxiSourceFrameResult *result){
 if(!frame||!cfg||!packets||!result||count>4096||!count)return -1;
 TaxiRenderPacket *storage=calloc(count,sizeof *storage);if(!storage)return -1;
 TaxiRenderQueue queue;taxi_render_queue_init(&queue,storage,count);
 for(size_t i=0;i<count;i++)if(taxi_render_queue_push(&queue,packets[i].bucket,0,i)){free(storage);return -1;}
 const TaxiTrafficScreenBounds *b=&cfg->bounds;
 if(b->left<0||b->top<0||b->right<0||b->bottom<0||taxi_world_background(frame,cfg->pixels,cfg->size,cfg->height,cfg->pitch,cfg->yaw,(unsigned)b->left,(unsigned)b->top,(unsigned)b->right,(unsigned)b->bottom,cfg->fill)){free(storage);return -1;}
 Context context={frame,cfg,packets,count,{0,0}};int status=taxi_render_queue_consume(&queue,draw,&context);
 free(storage);if(status)return -1;*result=context.result;return 0;
}
