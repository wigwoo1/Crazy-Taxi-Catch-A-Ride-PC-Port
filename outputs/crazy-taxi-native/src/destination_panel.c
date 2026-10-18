#include "destination_panel.h"
#include "world_background.h"
#include "distance_bitmap.h"
#include "render_viewport.h"
typedef struct {TaxiWorldFrame *frame;const TaxiIndexedAsset *textures;
    const uint8_t *lookup;uint8_t *image;} PanelDraw;
int taxi_destination_panel_selection(void *context,const uint8_t record[48],
    const uint8_t cache[512]) {
    TaxiDestinationPanelContext *panel=context;
    if(!panel||!panel->engine||!cache)return -1;
    TaxiWorldFrame inherited=*panel->engine;
    inherited.material_cache=cache;inherited.material_cache_size=512;
    return taxi_destination_panel(panel->output,panel->scratch,&inherited,record,
        panel->textures,panel->background_descriptor,panel->lookup,panel->border_color);
}
static int draw(void *context,unsigned kind,size_t payload) {
    PanelDraw *d=context;
    if(payload>=d->frame->count || kind!=d->frame->packets[payload].kind)return -1;
    int rc=taxi_world_indexed_packet(d->image,d->frame->packets+payload,
        d->frame->world,d->textures,d->lookup,d->frame->projection.far_plane,
        d->frame->shade_depth);
    return rc==1?0:-1; /* No silent omitted family in a panel. */
}
int taxi_destination_panel(uint8_t output[4096],uint8_t scratch[38400],
    const TaxiWorldFrame *engine,const uint8_t record[48],
    const TaxiIndexedAsset *textures,int32_t background_descriptor,
    const uint8_t lookup[2560],uint8_t border_color) {
    if(!output||!scratch||!engine||!engine->queue||!engine->material_cache)return -1;
    TaxiViewTransform view;
    if(taxi_view_transform_destination(&view,record,48,engine->trig))return -1;
    TaxiWorldFrame frame=*engine;
    TaxiRenderQueue queue;
    taxi_render_queue_init(&queue,engine->queue->packets,engine->queue->capacity);
    /* Original318AA 2C790 resets projection as well as clip bounds.
       Near/far remain inherited; parent frame stays unchanged. */
    TaxiRenderViewport viewport={0};
    taxi_render_viewport_set(&viewport,0,0,240,160);
    frame.queue=&queue;frame.left=viewport.left;frame.top=viewport.top;
    frame.right=viewport.right;frame.bottom=viewport.bottom;
    frame.projection.center_x=viewport.center_x;
    frame.projection.center_y=viewport.center_y;
    frame.projection.focal=viewport.focal;
    frame.camera=(TaxiPoint){view.words[0],view.words[1],view.words[2]};
    frame.pitch=view.words[3];frame.yaw=view.words[4];
    /* Current frame producer lacks roll. Reject rather than silently omit it. */
    if(view.words[5])return -1;
    /* 318CC ->2D18 advances the copied counter. The parent snapshot restores
       its generation while external polygon stamps retain this new value. */
    if(frame.normal_stamps&&frame.special_stamps)frame.generation+=1u;
    /* Original318CC prepares geometry before318D2 draws the sky and queue. */
    if(taxi_world_frame_build(&frame)||
        taxi_world_background_scene(scratch,textures,background_descriptor,
            frame.pitch,frame.yaw,0,0,240,160))return -1;
    PanelDraw context={&frame,textures,lookup,scratch};
    if(taxi_render_queue_consume(&queue,draw,&context))return -1;
    taxi_distance_bitmap_sample(output,scratch);
    taxi_distance_bitmap_border(output,border_color);
    return 0;
}
