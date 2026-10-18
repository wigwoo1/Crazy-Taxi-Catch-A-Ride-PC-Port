#include "world_indexed.h"
#include "indexed_pixel.h"
#include "raster_dispatch.h"
uint32_t taxi_world_indexed_callback(const TaxiWorldFramePacket *p,const TaxiWorldAsset *world) {
    if(!p||!world||!world->materials||!p->raster_flags_valid||p->material>=world->nm)return 0;
    uint16_t flags=p->raster_flags;
    uint32_t callback=taxi_raster_callback_global(flags,p->texture!=255,
        world->materials[p->material*28+20]);
    if(p->kind==TAXI_RENDER_WORLD && (callback==0x030069b0 || callback==0x0300691c))
        callback=taxi_raster_callback_global((uint16_t)((flags&~(1u|2u|512u))|4u),
            p->texture!=255,world->materials[p->material*28+20]);
    return callback;
}
int taxi_world_indexed_flat_packet_work(uint8_t framebuffer[38400],const TaxiWorldFramePacket *p,const TaxiWorldAsset *world,TaxiFlatRowWork *work){
    if(!framebuffer||!p||!world||!world->materials)return -1;
    if(work)*work=(TaxiFlatRowWork){0};
    if(p->kind!=TAXI_RENDER_WORLD||taxi_world_indexed_callback(p,world)!=0x03006950)return 0;
    if(p->draw_culled)return 1;
    uint32_t raw=taxi_raster_callback_global(p->raster_flags,p->texture!=255,world->materials[p->material*28+20]);
    unsigned count=(raw==0x030069b0||raw==0x0300691c)?p->polygon.count:taxi_raster_dispatch(p->raster_flags).vertices;
    if(count<3||count>8||p->polygon.count!=count)return -1;
    TaxiIndexedVertex vertices[8];
    for(unsigned i=0;i<count;i++){const TaxiWorldClipVertex *v=p->polygon.vertices+i;vertices[i]=(TaxiIndexedVertex){v->screen_x,v->screen_y,(uint32_t)v->u,(uint32_t)v->v,(uint32_t)v->z};}
    const uint8_t *color=world->materials+p->material*28+14;
    uint16_t pattern=(uint16_t)((unsigned)color[0]|((unsigned)color[1]<<8));
    return taxi_indexed_clipped_flat_polygon_work(framebuffer,vertices,count,pattern,work)?1:-1;
}
int taxi_world_indexed_packet_work(uint8_t framebuffer[38400],
    const TaxiWorldFramePacket *p,const TaxiWorldAsset *world,
    const TaxiIndexedAsset *textures,const uint8_t lookup[2560],
    int32_t far_depth,int32_t shade_depth,TaxiPairColumnWork *work) {
    if(!p || !world || !textures || !framebuffer || !lookup)return -1;
    if(work)*work=(TaxiPairColumnWork){0};
    if(p->draw_culled)return 1; /* Submitted source packet, no prepared screen coverage. */
    if(!p->raster_flags_valid)return 0;
    if(p->material>=world->nm)return -1;
    uint32_t callback=taxi_raster_callback_global(p->raster_flags,p->texture!=255,
        world->materials[p->material*28+20]);
    if(p->kind==TAXI_RENDER_SPECIAL) {
        if(callback!=0x030069a4 && callback!=0x03006a28 && callback!=0x03006a38)return 0;
        TaxiIndexedTexture texture;
        if(taxi_indexed_asset_texture(textures,p->texture,&texture))return -1;
        TaxiTrafficScreenBounds bounds=p->special.screen;
        if(callback==0x03006a38)return taxi_indexed_shaded_billboard(framebuffer,
            bounds.left,bounds.top,bounds.right,bounds.bottom,texture.pixels,
            texture.size,texture.width,texture.height,texture.log_width,lookup,
            (uint32_t)p->special.depth,(uint32_t)far_depth,(uint32_t)shade_depth)?1:-1;
        return taxi_indexed_billboard_work(framebuffer,bounds.left,bounds.top,
            bounds.right,bounds.bottom,texture.pixels,texture.size,texture.width,
            texture.height,texture.log_width,callback==0x030069a4,work)?1:-1;
    }
    if(p->kind!=TAXI_RENDER_WORLD)return 0;
    int depth_wrapper=callback==0x030069b0 || callback==0x0300691c;
    if(depth_wrapper) {
        /* world_frame already performs4424/4704 clipping/reprojection.
           Both46ec and49cc invoke
           material bank+0 even when the original packet had shade flag512. */
        callback=taxi_world_indexed_callback(p,world);
    }
    int attribute_flat=callback==0x03006918 || callback==0x03006a2c;
    int flat=attribute_flat || callback==0x03006950 || callback==0x0300690c || callback==0x0300694c;
    int perspective_rows=callback==0x03006934 || callback==0x03006944;
    int perspective_columns=callback==0x030069ac || callback==0x030069b8;
    if(!flat && !perspective_rows && !perspective_columns && callback!=0x0300693c && callback!=0x03006948 && callback!=0x03006914)return 0;
    unsigned count=depth_wrapper?p->polygon.count:taxi_raster_dispatch(p->raster_flags).vertices;
    if(count<3 || count>8 || p->polygon.count!=count)return -1;
    TaxiIndexedVertex vertices[8];
    for(unsigned i=0;i<count;++i) {
        const TaxiWorldClipVertex *v=p->polygon.vertices+i;
        vertices[i]=(TaxiIndexedVertex){v->screen_x,v->screen_y,(uint32_t)v->u,(uint32_t)v->v,(uint32_t)v->z};
    }
    if(flat) {
        if(attribute_flat) {
            uint32_t attributes[8];
            for(unsigned i=0;i<count;++i)attributes[i]=(uint32_t)p->polygon.vertices[i].attribute;
            return taxi_indexed_attribute_flat_polygon(framebuffer,vertices,
                attributes,count,world->materials+p->material*28,
                callback==0x03006a2c?lookup:0,(uint32_t)far_depth,
                (uint32_t)shade_depth)?1:-1;
        }
        /* Original2b8c6 r7=material+14, passed as callback r3. */
        const uint8_t *color=world->materials+p->material*28+14;
        uint16_t pattern=(uint16_t)((unsigned)color[0]|((unsigned)color[1]<<8));
        if(callback==0x0300694c)return taxi_indexed_shaded_flat_polygon(
            framebuffer,vertices,count,pattern,lookup,(uint32_t)far_depth,
            (uint32_t)shade_depth)?1:-1;
        return taxi_indexed_flat_polygon(framebuffer,vertices,count,pattern,
            callback==0x03006950)?1:-1;
    }
    TaxiIndexedTexture texture;
    if(taxi_indexed_asset_texture(textures,p->texture,&texture))return -1;
    TaxiIndexedSampling sampling;
    if(!taxi_indexed_sampling(&sampling,texture.width,texture.height,texture.log_width,0))return -1;
    if(perspective_rows)return taxi_indexed_perspective_rows_polygon_work(framebuffer,
        vertices,count,texture.pixels,texture.size,sampling,work)?1:-1;
    if(perspective_columns)return taxi_indexed_perspective_columns_polygon_work(framebuffer,
        vertices,count,texture.pixels,texture.size,sampling,work)?1:-1;
    if(callback==0x03006948)return taxi_indexed_unshaded_polygon_work(framebuffer,
        vertices,count,texture.pixels,texture.size,sampling,work)?1:-1;
    if(callback==0x03006914)return taxi_indexed_unshaded_clipped_polygon_work(framebuffer,
        vertices,count,texture.pixels,texture.size,sampling,work)?1:-1;
    return taxi_indexed_polygon(framebuffer,vertices,count,texture.pixels,texture.size,
        sampling,lookup,0,0,240,160,(uint32_t)far_depth,(uint32_t)shade_depth)?1:-1;
}

int taxi_world_indexed_packet(uint8_t framebuffer[38400],const TaxiWorldFramePacket *p,const TaxiWorldAsset *world,const TaxiIndexedAsset *textures,const uint8_t lookup[2560],int32_t far_depth,int32_t shade_depth){return taxi_world_indexed_packet_work(framebuffer,p,world,textures,lookup,far_depth,shade_depth,0);}
