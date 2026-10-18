#include "world_render.h"
#include "render_queue.h"
#include <string.h>
static uint32_t u16(const uint8_t *p){return p[0]|((uint32_t)p[1]<<8);}
static uint32_t u32(const uint8_t *p){return u16(p)|(u16(p+2)<<16);}
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t asr8(uint32_t u){return bits((u>>8)|((u&0x80000000u)?0xff000000u:0));}
static TaxiPoint point(const uint8_t *p){return (TaxiPoint){bits(u32(p)),bits(u32(p+4)),bits(u32(p+8))};}
int taxi_world_render_special(const TaxiWorldAsset *w,unsigned index,TaxiPoint camera,
    int32_t yaw,int32_t pitch,const TaxiTrig *trig,const TaxiTrafficProjection *p,
    TaxiWorldRenderFrustum f,TaxiWorldRenderSpecial *out){
    if(!w||!w->special_storage||index>=w->special_count||!trig||!p||!out)return -1;
    const uint8_t *record=w->special_storage+32*index;
    if(record[0]>=w->nm)return -1;
    TaxiPoint position=point(record+4);
    uint32_t x=(uint32_t)position.x-(uint32_t)camera.x,y=(uint32_t)position.y-(uint32_t)camera.y;
    uint32_t z=(uint32_t)position.z-(uint32_t)camera.z;
    unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    int32_t rx=asr8(x*yc-z*ys),rz=asr8(x*ys+z*yc);
    int32_t py=asr8((uint32_t)rz*ps+y*pc),pz=asr8((uint32_t)rz*pc-y*ps);
    if(pz<=p->near_plane||pz>=p->far_plane)return 0;
    TaxiWorldRenderSpecial result={.material=record[0],.depth=pz};
    result.billboard=(TaxiTrafficBillboard){
        {bits((uint32_t)rx+u32(record+16)),bits((uint32_t)py+u32(record+28)),pz},
        {bits((uint32_t)rx+u32(record+24)),bits((uint32_t)py+u32(record+20)),pz}};
    int32_t hx=asr8((uint32_t)pz*(uint32_t)f.horizontal),vy=asr8((uint32_t)pz*(uint32_t)f.vertical);
    unsigned common=15;
    for(unsigned i=0;i<2;i++){
        TaxiPoint v=i?result.billboard.bottom_right:result.billboard.top_left;
        unsigned flags=(v.x<=bits(0u-(uint32_t)hx)?1u:0u)|(v.x>=hx?2u:0u)|
            (v.y<=bits(0u-(uint32_t)vy)?4u:0u)|(v.y>=vy?8u:0u);
        common&=flags;result.clip_flags|=flags;
    }
    if(common)return 0;
    TaxiTrafficProjection special_projection=*p;
    /* Same wrapped projection, removing traffic-only near+768 restriction. */
    special_projection.near_plane=bits((uint32_t)p->near_plane-768u);
    if(taxi_traffic_project(&result.billboard,&special_projection,&result.screen)!=1||
        taxi_traffic_depth_bucket(pz,0,p->far_plane,trig,&result.bucket))return -1;
    *out=result;return 1;
}
int taxi_world_render_bounds_corners(const TaxiWorldAsset *w,TaxiPoint camera,
    int32_t yaw,int32_t pitch,const TaxiTrig *trig,
    const int32_t corners[8][4],TaxiWorldRenderBounds *out){
    if(!w||!trig||!corners||!out||!w->nx||!w->nz||w->sx<=0||w->sz<=0)return -1;
    int32_t minx=INT32_MAX,maxx=INT32_MIN,minz=INT32_MAX,maxz=INT32_MIN;
    unsigned a=(unsigned)yaw&4095,b=(unsigned)pitch&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    for(unsigned corner=0;corner<8;corner++){
        int32_t x=corners[corner][0],y=corners[corner][1],z=corners[corner][2];
        int32_t rz=asr8((uint32_t)z*pc-(uint32_t)y*ps);
        /* Rotated Y is unused by original XZ min/max scan. */
        int32_t wx=bits((uint32_t)asr8((uint32_t)x*yc-(uint32_t)rz*ys)+(uint32_t)camera.x);
        int32_t wz=bits((uint32_t)asr8((uint32_t)x*ys+(uint32_t)rz*yc)+(uint32_t)camera.z);
        if(wx<minx)minx=wx;if(wx>maxx)maxx=wx;
        if(wz<minz)minz=wz;if(wz>maxz)maxz=wz;
    }
    TaxiWorldRenderBounds bounds={bits((uint32_t)minx-(uint32_t)w->ox)/w->sx,
        bits((uint32_t)minz-(uint32_t)w->oz)/w->sz,
        bits((uint32_t)maxx-(uint32_t)w->ox)/w->sx,
        bits((uint32_t)maxz-(uint32_t)w->oz)/w->sz};
    if(bounds.min_x<0)bounds.min_x=0;if(bounds.min_z<0)bounds.min_z=0;
    if(bounds.max_x>=(int32_t)w->nx)bounds.max_x=(int32_t)w->nx-1;
    if(bounds.max_z>=(int32_t)w->nz)bounds.max_z=(int32_t)w->nz-1;
    *out=bounds;return 0;
}
int taxi_world_render_bounds(const TaxiWorldAsset *w,TaxiPoint camera,
    int32_t yaw,int32_t pitch,const TaxiTrig *trig,const TaxiTrafficProjection *p,
    int32_t left,int32_t top,int32_t right,int32_t bottom,TaxiWorldRenderBounds *out){
    if(!p||p->focal<=0)return -1;
    int32_t corners[8][4]={{0}};
    for(unsigned i=0;i<8;i++){
        int32_t z=i<4?p->near_plane:p->far_plane;
        int32_t x=(i%4==1||i%4==2)?right:left,y=(i%4>=2)?bottom:top;
        corners[i][0]=bits(((uint32_t)x-(uint32_t)p->center_x)*(uint32_t)z)/p->focal;
        corners[i][1]=bits(((uint32_t)y-(uint32_t)p->center_y)*(uint32_t)z)/p->focal;
        corners[i][2]=z;
    }
    return taxi_world_render_bounds_corners(w,camera,yaw,pitch,trig,corners,out);
}
TaxiWorldRenderFrustum taxi_world_render_frustum(int32_t left,int32_t top,
    int32_t right,int32_t bottom){
    return (TaxiWorldRenderFrustum){
        bits(((uint32_t)right-(uint32_t)left)<<7)/192,
        bits(((uint32_t)bottom-(uint32_t)top)<<7)/192};
}
int taxi_world_polygon_facing(const TaxiWorldAsset *w,unsigned index,TaxiPoint camera){
    if(!w||!w->vertices||!w->polygons||index>=w->np)return -1;
    const uint8_t *poly=w->polygons+24*index;
    if(poly[1]&1)return 1;
    unsigned vertex=u16(poly+8);if(vertex>=w->nv)return -1;
    TaxiPoint v=point(w->vertices+16*vertex);uint32_t dot=0;
    int32_t normals[3]={poly[2]<128?poly[2]:(int32_t)poly[2]-256,
        poly[3]<128?poly[3]:(int32_t)poly[3]-256,poly[4]<128?poly[4]:(int32_t)poly[4]-256};
    dot+=(uint32_t)normals[0]*((uint32_t)camera.x-(uint32_t)v.x);
    dot+=(uint32_t)normals[1]*((uint32_t)camera.y-(uint32_t)v.y);
    dot+=(uint32_t)normals[2]*((uint32_t)camera.z-(uint32_t)v.z);
    return bits(dot)>=0;
}
int taxi_world_render_vertex(TaxiPoint v,TaxiPoint camera,int32_t yaw,int32_t pitch,
    const TaxiTrig *trig,const TaxiTrafficProjection *p,int32_t horizontal,int32_t vertical,
    TaxiPoint *out,unsigned *clip_flags){
    if(!trig||!p||!out||!clip_flags)return -1;
    unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    uint32_t x=(uint32_t)v.x-(uint32_t)camera.x,y=(uint32_t)v.y-(uint32_t)camera.y;
    uint32_t dz=(uint32_t)v.z-(uint32_t)camera.z;
    int32_t rx=asr8(x*yc-dz*ys),rz=asr8(x*ys+dz*yc);
    TaxiPoint view={rx,asr8((uint32_t)rz*ps+y*pc),asr8((uint32_t)rz*pc-y*ps)};
    int32_t hx=asr8((uint32_t)view.z*(uint32_t)horizontal);
    int32_t vy=asr8((uint32_t)view.z*(uint32_t)vertical);
    *clip_flags=(view.x<=bits(0u-(uint32_t)hx)?1u:0u)|(view.x>=hx?2u:0u)|
        (view.y<=bits(0u-(uint32_t)vy)?4u:0u)|(view.y>=vy?8u:0u)|
        (view.z<=p->near_plane?16u:0u)|(view.z>=p->far_plane?32u:0u);
    *out=view;return 0;
}
int taxi_world_render_aggregate(const TaxiPoint *vertices,const unsigned *flags,
    unsigned count,TaxiWorldRenderPolygon *out){
    if(!vertices||!flags||!out||(count!=3&&count!=4))return -1;
    TaxiWorldRenderPolygon result={.count=count};unsigned common=63;int32_t z[4];
    for(unsigned i=0;i<count;i++){
        result.vertices[i]=vertices[i];z[i]=vertices[i].z;
        if(z[i]>result.max_depth)result.max_depth=z[i];
        common&=flags[i];result.clip_flags|=flags[i];
    }
    if(common)return 0;
    if(taxi_world_polygon_depth(z,count,&result.depth))return -1;
    *out=result;return 1;
}
int taxi_world_render_polygon(const TaxiWorldAsset *w,unsigned index,TaxiPoint camera,
    int32_t yaw,int32_t pitch,const TaxiTrig *trig,const TaxiTrafficProjection *p,
    int32_t horizontal,int32_t vertical,TaxiWorldRenderPolygon *out){
    if(!w||!w->vertices||!w->polygons||!w->materials||index>=w->np||!trig||!p||!out)return -1;
    const uint8_t *poly=w->polygons+24*index;
    unsigned material=poly[0];if(material>=w->nm)return -1;
    if(w->materials[material*28+20]&9)return 0;
    unsigned count=(u16(poly+14)&0x8000u)?3:4;
    for(unsigned i=0;i<count;i++)if(u16(poly+8+2*i)>=w->nv)return -1;
    int facing=taxi_world_polygon_facing(w,index,camera);
    if(facing<=0)return facing;
    TaxiWorldRenderPolygon result={.count=count};
    unsigned vertex_flags[4];
    for(unsigned i=0;i<count;i++){
        TaxiPoint v=point(w->vertices+16*u16(poly+8+2*i)),view;unsigned flags;
        if(taxi_world_render_vertex(v,camera,yaw,pitch,trig,p,horizontal,vertical,&view,&flags))return -1;
        result.vertices[i]=view;vertex_flags[i]=flags;
    }
    int aggregate=taxi_world_render_aggregate(result.vertices,vertex_flags,count,&result);
    if(aggregate<=0)return aggregate;
    if(taxi_traffic_depth_bucket(result.depth,0,p->far_plane,trig,&result.bucket))return -1;
    *out=result;return 1;
}
