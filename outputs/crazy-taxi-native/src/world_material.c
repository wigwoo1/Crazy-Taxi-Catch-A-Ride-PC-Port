#include "world_material.h"
#include <string.h>
static uint32_t u16(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8);}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
int taxi_world_material_texture(const uint8_t *m,int32_t phase,unsigned total,unsigned *out){
    if(!m||!out)return -1;
    int32_t index=m[16];
    if(index==255){*out=255;return 0;}
    if(m[17]>1){
        uint32_t product=u16(m+18)*(uint32_t)phase;
        int32_t frame=bits((product>>12)|((product&0x80000000u)?0xfff00000u:0));
        index+=frame%(int32_t)m[17];
    }
    if(index<0||(unsigned)index>=total)return -1;
    *out=(unsigned)index;return 0;
}
static int material_polygon(int scrolling,const TaxiWorldAsset *w,unsigned index,
    const TaxiWorldRenderPolygon *g,int32_t phase,unsigned total,TaxiWorldMaterialPolygon *out){
    if(!w||!w->polygons||!w->materials||index>=w->np||!g||!out||(g->count!=3&&g->count!=4))return -1;
    const uint8_t *poly=w->polygons+24*index;
    if(poly[0]>=w->nm)return -1;
    const uint8_t *m=w->materials+28*poly[0];
    TaxiWorldMaterialPolygon result={.count=g->count};
    if(taxi_world_material_texture(m,phase,total,&result.texture))return -1;
    if(!scrolling&&result.texture!=255&&(m[24]||m[25]||m[26]||m[27]))return -1;
    /* Original3800..3834 writes all four flat-shade attributes, including
       the retained fourth slot on triangles. Texture255 represents NULL. */
    if(result.texture==255)for(unsigned i=0;i<4;i++)
        result.vertices[i].attribute=bits(u16(poly+16+2*i)<<12);
    for(unsigned i=0;i<g->count;i++){
        TaxiWorldClipVertex *v=result.vertices+i;
        v->x=g->vertices[i].x;v->y=g->vertices[i].y;v->z=g->vertices[i].z;
        if(result.texture!=255){v->u=(int32_t)poly[16+2*i]<<14;v->v=(int32_t)poly[17+2*i]<<14;}
    }
    *out=result;return 0;
}

int taxi_world_material_scroll(TaxiWorldMaterialPolygon *p,const uint8_t *m,int32_t phase,unsigned lw,unsigned lh){
 if(!p||!m||lw>7||lh>7)return -1;
 uint32_t word=m[24]|((uint32_t)m[25]<<8)|((uint32_t)m[26]<<16)|((uint32_t)m[27]<<24);
 uint32_t u=word*(uint32_t)phase&((1u<<(lw+8))-1),v=((word>>16)|(word<<16))*(uint32_t)phase&((1u<<(lh+8))-1);
 for(unsigned n=0;n<p->count;n++){p->vertices[n].u=bits((uint32_t)p->vertices[n].u+(u<<6));p->vertices[n].v=bits((uint32_t)p->vertices[n].v+(v<<6));}return 0;
}

int taxi_world_material_polygon(const TaxiWorldAsset *w,unsigned n,const TaxiWorldRenderPolygon *g,int32_t phase,unsigned total,TaxiWorldMaterialPolygon *out){return material_polygon(0,w,n,g,phase,total,out);}
int taxi_world_material_polygon_scrolling(const TaxiWorldAsset *w,unsigned n,const TaxiWorldRenderPolygon *g,int32_t phase,unsigned total,TaxiWorldMaterialPolygon *out){return material_polygon(1,w,n,g,phase,total,out);}
int taxi_world_material_uv(TaxiWorldMaterialPolygon *p,const uint8_t *poly,
 const uint8_t *m,int32_t phase,unsigned lw,unsigned lh){
 if(!p||!poly||!m||(p->count!=3&&p->count!=4)||lw>7||lh>7)return -1;
 uint32_t word=m[24]|((uint32_t)m[25]<<8)|((uint32_t)m[26]<<16)|((uint32_t)m[27]<<24);
 uint32_t u=word*(uint32_t)phase&((1u<<(lw+8))-1);
 uint32_t v=((word>>16)|(word<<16))*(uint32_t)phase&((1u<<(lh+8))-1);
 /* Original unaligned descriptor loads replace SL when rates are nonzero.
    Valid log dimensions0..7 leave mask0x10 clear, writing four UV slots. */
 unsigned slots=word?4:p->count;
 for(unsigned i=0;i<slots;i++){
  p->vertices[i].u=bits(((uint32_t)poly[16+i*2]<<14)+(u<<6));
  p->vertices[i].v=bits(((uint32_t)poly[17+i*2]<<14)+(v<<6));
 }
 return 0;
}
