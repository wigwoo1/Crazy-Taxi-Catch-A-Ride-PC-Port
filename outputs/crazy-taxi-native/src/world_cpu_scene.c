#include "world_cpu_scene.h"
#include "world_background.h"
#include "indexed_pixel.h"
#include <string.h>
typedef struct {uint8_t *fb;TaxiWorldFrame *frame;const uint8_t *lookup;const TaxiShadow *shadow;const TaxiShadowRender *view;unsigned shadow_frame,shadow_kind;TaxiWorldCpuWork work;} Context;
static int draw(void *arg,unsigned kind,size_t payload){Context *c=arg;TaxiPairColumnWork w={0};TaxiFlatRowWork f={0};int rc;
 if(kind==c->shadow_kind){if(payload||!c->shadow||!c->view||c->shadow_frame>=64)return -1;TaxiTrafficScreenBounds s=c->view->screen;
 rc=taxi_indexed_billboard_work(c->fb,s.left,s.top,s.right,s.bottom,c->shadow->pixels+c->shadow_frame*2048,2048,64,32,6,0,&w)?1:-1;
 }else{if((kind!=TAXI_RENDER_WORLD&&kind!=TAXI_RENDER_SPECIAL)||payload>=c->frame->count)return -1;
 const TaxiWorldFramePacket *p=c->frame->packets+payload;
 if(p->draw_culled)rc=1;
 else if(taxi_world_indexed_callback(p,c->frame->world)==0x03006950)rc=taxi_world_indexed_flat_packet_work(c->fb,p,c->frame->world,&f);
 else rc=taxi_world_indexed_packet_work(c->fb,p,c->frame->world,c->frame->textures,c->lookup,0,0,&w);
 }
 if(rc!=1)return -1;c->work.packets++;c->work.textured.texture_byte_reads+=w.texture_byte_reads;c->work.textured.framebuffer_halfword_writes+=w.framebuffer_halfword_writes;c->work.flat.framebuffer_halfword_writes+=f.framebuffer_halfword_writes;c->work.flat.framebuffer_word_writes+=f.framebuffer_word_writes;return 0;
}
int taxi_world_cpu_scene(uint8_t fb[38400],TaxiWorldFrame *frame,int32_t background,const uint8_t lookup[2560],const TaxiShadow *shadow,const TaxiShadowRender *view,unsigned shadow_frame,unsigned shadow_kind,TaxiWorldCpuWork *work){
 if(!fb||!frame||!frame->queue||!frame->world||!frame->textures||!lookup||!work||frame->left!=0||frame->top!=0||frame->right!=240||frame->bottom!=160||shadow_kind==TAXI_RENDER_WORLD||shadow_kind==TAXI_RENDER_SPECIAL)return -1;
 if(taxi_world_background_scene(fb,frame->textures,background,frame->pitch,frame->yaw,0,0,240,160))return -1;
 Context c={.fb=fb,.frame=frame,.lookup=lookup,.shadow=shadow,.view=view,.shadow_frame=shadow_frame,.shadow_kind=shadow_kind};
 if(taxi_render_queue_consume(frame->queue,draw,&c))return -1;*work=c.work;return 0;
}
