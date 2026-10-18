#include "world_frame.h"
#include "raster_dispatch.h"
#include <string.h>
#include <stdio.h>
static int frame_error(const char *function,int line){fprintf(stderr,"World frame failure: %s line%d\n",function,line);return -1;}
static int submit(void *context,unsigned reference){
    TaxiWorldFrame *f=context;const TaxiWorldAsset *w=f->world;
    TaxiWorldFramePacket packet={0};int rc;
    TaxiWorldRenderFrustum frustum=taxi_world_render_frustum(f->left,f->top,f->right,f->bottom);
    if(reference&0x8000u){
        if(reference&0x4000u)return 0;
        unsigned id=reference&0x3fffu;
        if(id>=w->special_count)return frame_error(__func__,__LINE__);
        if(f->special_seen[id])return 0;
        f->special_seen[id]=1;
        rc=taxi_world_render_special(w,id,f->camera,f->yaw,f->pitch,f->trig,
            &f->projection,frustum,&packet.special);
        if(rc<=0)return rc;
        packet.source_index=id;packet.kind=TAXI_RENDER_SPECIAL;
        packet.material=packet.special.material;packet.bucket=packet.special.bucket;
        if(taxi_world_material_texture(w->materials+28*packet.material,f->phase,
            f->texture_count,&packet.texture))return frame_error(__func__,__LINE__);
        if(f->material_cache) {
            /* Source3a28..3a48; common36d0 submission does not OR cache256. */
            packet.raster_flags=(uint16_t)(8u|(packet.special.clip_flags?4u:0u));
            uint32_t threshold=(uint32_t)f->projection.far_plane-(uint32_t)f->shade_depth;
            int32_t signed_threshold;memcpy(&signed_threshold,&threshold,4);
            if(packet.special.depth>=signed_threshold)packet.raster_flags|=512;
            packet.raster_flags_valid=1;
        }
    }else{
        TaxiWorldRenderPolygon geometry;
        rc=taxi_world_render_polygon(w,reference,f->camera,f->yaw,f->pitch,f->trig,
            &f->projection,frustum.horizontal,frustum.vertical,&geometry);
        if(rc<=0)return rc;
        TaxiWorldMaterialPolygon material;
        const uint8_t *m=w->materials+28*w->polygons[24*reference];
        if((m[24]||m[25]||m[26]||m[27])&&m[16]!=255){
            TaxiIndexedTexture t;
            if(!f->textures||taxi_world_material_polygon_scrolling(w,reference,&geometry,f->phase,f->texture_count,&material)||taxi_indexed_asset_texture(f->textures,material.texture,&t)||taxi_world_material_uv(&material,w->polygons+24*reference,m,f->phase,t.log_width,t.log_height))return frame_error(__func__,__LINE__);
        }else if(taxi_world_material_polygon(w,reference,&geometry,f->phase,f->texture_count,&material))return frame_error(__func__,__LINE__);
        packet.source_index=reference;packet.kind=TAXI_RENDER_WORLD;
        packet.material=w->polygons[24*reference];packet.texture=material.texture;packet.bucket=geometry.bucket;
        packet.raw_polygon.count=material.count;
        memcpy(packet.raw_polygon.vertices,material.vertices,sizeof material.vertices);
        if(!(geometry.clip_flags&48)){
            TaxiTrafficScreenBounds raw_bounds;
            if(taxi_world_project_vertices(&packet.raw_polygon,&f->projection,&raw_bounds))return -1;
        }
        if(geometry.clip_flags&48){
            int far_clip=!(geometry.clip_flags&16);
            rc=taxi_world_clip_depth(material.vertices,material.count,
                far_clip?f->projection.far_plane:f->projection.near_plane,far_clip,&packet.polygon);
            if(rc<0)return rc;
            if(!rc){packet.draw_culled=1;packet.polygon.count=material.count;memcpy(packet.polygon.vertices,material.vertices,sizeof material.vertices);}
        }else{
            packet.polygon.count=material.count;
            memcpy(packet.polygon.vertices,material.vertices,material.count*sizeof(*material.vertices));
        }
        /* Original31EC queues before4424/4704 screen rejection in the consumer.
           Keep that submission even when the prepared polygon draws nothing. */
        if(!packet.draw_culled){
            TaxiTrafficScreenBounds bounds;
            if(taxi_world_project_vertices(&packet.polygon,&f->projection,&bounds))return -1;
            packet.draw_culled=bounds.left>=f->right||bounds.right<f->left||
                bounds.top>=f->bottom||bounds.bottom<f->top;
        }
        if (f->material_cache) {
            if (packet.material>=f->material_cache_size) return frame_error(__func__,__LINE__);
            int32_t min_x=packet.polygon.vertices[0].screen_x,max_x=min_x;
            int32_t min_y=packet.polygon.vertices[0].screen_y,max_y=min_y;
            for (unsigned i=1;i<packet.polygon.count;++i) {
                TaxiWorldClipVertex v=packet.polygon.vertices[i];
                if (v.screen_x<min_x) min_x=v.screen_x;
                if (v.screen_x>max_x) max_x=v.screen_x;
                if (v.screen_y<min_y) min_y=v.screen_y;
                if (v.screen_y>max_y) max_y=v.screen_y;
            }
            uint32_t uw=(uint32_t)max_x-(uint32_t)min_x,uh=(uint32_t)max_y-(uint32_t)min_y;
            int32_t width,height;memcpy(&width,&uw,4);memcpy(&height,&uh,4);
            const uint8_t *poly=w->polygons+24*reference;
            int8_t nx,ny,nz;memcpy(&nx,poly+2,1);memcpy(&ny,poly+3,1);memcpy(&nz,poly+4,1);
            packet.raster_flags=taxi_world_packet_flags(geometry.count,geometry.clip_flags,
                width,height,nx,ny,nz,f->material_cache[packet.material],
                geometry.max_depth,f->projection.far_plane,f->shade_depth);
            packet.raster_flags_valid=1;
        }
    }
    if(f->count>=f->capacity)return frame_error(__func__,__LINE__);
    if(taxi_render_queue_push(f->queue,packet.bucket,packet.kind,f->count))return frame_error(__func__,__LINE__);
    f->packets[f->count++]=packet;return 0;
}
int taxi_world_frame_build(TaxiWorldFrame *f){
    if(!f||!f->world||!f->queue||!f->packets||!f->normal_seen||
        f->normal_seen_size<f->world->np||!f->special_seen||
        f->special_seen_size<f->world->special_count)return frame_error(__func__,__LINE__);
    if((f->normal_stamps||f->special_stamps)&&
        (!f->normal_stamps||!f->special_stamps||f->normal_stamp_count<f->world->np||
         f->special_stamp_count<f->world->special_count))return frame_error(__func__,__LINE__);
    TaxiWorldRenderBounds bounds;
    int bounds_rc=f->bounds_corners?
        taxi_world_render_bounds_corners(f->world,f->camera,f->yaw,f->pitch,f->trig,
            f->bounds_corners,&bounds):
        taxi_world_render_bounds(f->world,f->camera,f->yaw,f->pitch,f->trig,
            &f->projection,f->left,f->top,f->right,f->bottom,&bounds);
    if(bounds_rc)return frame_error(__func__,__LINE__);
    f->count=0;memset(f->special_seen,0,f->world->special_count);
    if(f->normal_stamps)return taxi_world_render_visit_cached(f->world,
        bounds.min_x,bounds.min_z,bounds.max_x,bounds.max_z,
        f->normal_stamps,f->normal_stamp_count,f->special_stamps,f->special_stamp_count,
        f->generation,0,submit,f);
    return taxi_world_render_visit(f->world,bounds.min_x,bounds.min_z,bounds.max_x,bounds.max_z,
        f->normal_seen,f->normal_seen_size,0,submit,f);
}
