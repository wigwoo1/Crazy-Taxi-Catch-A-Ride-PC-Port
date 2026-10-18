#include "passenger_render.h"
#include "passenger_frame_banks.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t shift(uint32_t u,unsigned n){return bits((u>>n)|((u&0x80000000u)?(UINT32_MAX<<(32-n)):0));}
static int32_t magnitude(int32_t x){return x<0?bits(0u-(uint32_t)x):x;}
static int render_frustum(const uint8_t *p,TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out,int qualified){
    if(!p||!trig||!out)return -1;
    if(!qualified&&(p[0]&3)!=1)return 0;
    unsigned kind=p[1],stage=p[10],frame=p[8];
    static const unsigned starts[4]={0,1,5,9},counts[4]={1,4,4,9};
    if(kind>=8||stage>=4||frame>=counts[stage])return -1;
    int32_t xyz[3];memcpy(xyz,p+16,12);
    int32_t dx=bits((uint32_t)player.x-(uint32_t)xyz[0]),dz=bits((uint32_t)player.z-(uint32_t)xyz[2]);
    if(!qualified&&(magnitude(dx)>12799||magnitude(dz)>12799))return 0;
    uint32_t x=(uint32_t)xyz[0]-(uint32_t)camera.x,y=(uint32_t)xyz[1]-(uint32_t)camera.y,z=(uint32_t)xyz[2]-(uint32_t)camera.z;
    unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    int32_t rx=shift(x*yc-z*ys,8),rz=shift(x*ys+z*yc,8);
    TaxiPoint v={rx,shift((uint32_t)rz*ps+y*pc,8),shift((uint32_t)rz*pc-y*ps,8)};
    unsigned selected_texture=kind*18+starts[stage]+frame;
    uint32_t source_pointer=(uint32_t)p[36]|((uint32_t)p[37]<<8)|((uint32_t)p[38]<<16)|((uint32_t)p[39]<<24);
    /* Zero is the explicit legacy/bootstrap fixture. Live actor updates
       retain the original selected ROM pointer; rendering must consume it. */
    if(source_pointer&&taxi_passenger_texture_from_pointer(source_pointer,&selected_texture))return -1;
    TaxiPassengerRender result={.texture=selected_texture,.scale=scale?scale:0xb1400u};
    int32_t half_width=shift(result.scale*32u,17),height=shift(result.scale*64u,16);
    result.billboard=(TaxiTrafficBillboard){
        {bits((uint32_t)v.x-(uint32_t)half_width),bits((uint32_t)v.y+(uint32_t)height),v.z},
        {bits((uint32_t)v.x+(uint32_t)half_width),v.y,v.z}};
    int32_t hx=shift((uint32_t)v.z*(uint32_t)horizontal,8);
    int32_t vy=shift((uint32_t)v.z*(uint32_t)vertical,8);
    TaxiPoint tl=result.billboard.top_left,br=result.billboard.bottom_right;
    /* Four local corners have the same Z; AND of their side flags is
       equivalent to testing both endpoints independently on each axis. */
    int32_t nx=bits(0u-(uint32_t)hx),ny=bits(0u-(uint32_t)vy);
    if((tl.x<=nx&&br.x<=nx)||(tl.x>=hx&&br.x>=hx)||
       (tl.y<=ny&&br.y<=ny)||(tl.y>=vy&&br.y>=vy))return 0;
    TaxiTrafficProjection projection=taxi_traffic_projection_normal();
    int rc=taxi_traffic_project(&result.billboard,&projection,&result.screen);
    if(rc<=0)return rc;
    if(taxi_traffic_depth_bucket(v.z,1536,projection.far_plane,trig,&result.bucket))return -1;
    if(stage==3){
        unsigned a=(unsigned)heading&4095;
        uint32_t dot=((uint32_t)xyz[0]-(uint32_t)player.x)*(uint32_t)trig->cosine[a]+
            ((uint32_t)xyz[2]-(uint32_t)player.z)*(uint32_t)trig->sine[a];
        result.flip=bits(dot)>=0;
    }
    result.u[0]=result.u[1]=result.flip?(32<<14):0;
    result.u[2]=result.u[3]=result.flip?0:(32<<14);
    result.v[0]=result.v[3]=64<<14;
    result.v[1]=result.v[2]=0;
    *out=result;return 1;
}
int taxi_passenger_render_frustum(const uint8_t *p,TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out){
    return render_frustum(p,player,heading,camera,yaw,pitch,scale,trig,horizontal,vertical,out,0);
}
int taxi_passenger_render_qualified_frustum(const uint8_t *p,TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out){
    return render_frustum(p,player,heading,camera,yaw,pitch,scale,trig,horizontal,vertical,out,1);
}
int taxi_passenger_render(const uint8_t *p,TaxiPoint player,int32_t heading,
    TaxiPoint camera,int32_t yaw,int32_t pitch,uint32_t scale,const TaxiTrig *trig,
    TaxiPassengerRender *out){
    return taxi_passenger_render_frustum(p,player,heading,camera,yaw,pitch,scale,trig,160,106,out);
}

/* Original31AEE descriptor width64/height64, normal flags/flip0. */
int taxi_passenger_marker_render_frustum(const TaxiPassengerMarkerRequest *request,
    TaxiPoint camera,int32_t yaw,int32_t pitch,const TaxiTrig *trig,
    int32_t horizontal,int32_t vertical,TaxiPassengerRender *out){
    if(!request||!trig||!out||request->flip||request->flags)return -1;
    unsigned texture;
    if(request->descriptor>=0x087ec59cu&&request->descriptor<=0x087ec5ccu&&
        (request->descriptor-0x087ec59cu)%12u==0)texture=(request->descriptor-0x087ec59cu)/12u;
    else if(request->descriptor>=0x087ec5d8u&&request->descriptor<=0x087ec608u&&
        (request->descriptor-0x087ec5d8u)%12u==0)texture=5+(request->descriptor-0x087ec5d8u)/12u;
    else return -1;
    int32_t xyz[3]={request->x,request->y,request->z};
    uint32_t x=(uint32_t)xyz[0]-(uint32_t)camera.x,y=(uint32_t)xyz[1]-(uint32_t)camera.y,z=(uint32_t)xyz[2]-(uint32_t)camera.z;
    unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
    uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a];
    uint32_t pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
    int32_t rx=shift(x*yc-z*ys,8),rz=shift(x*ys+z*yc,8);
    TaxiPoint v={rx,shift((uint32_t)rz*ps+y*pc,8),shift((uint32_t)rz*pc-y*ps,8)};
    TaxiPassengerRender result={.texture=texture,.scale=request->scale};
    int32_t half_width,height;taxi_passenger_marker_dimensions(result.scale,&half_width,&height);
    result.billboard=(TaxiTrafficBillboard){
        {bits((uint32_t)v.x-(uint32_t)half_width),bits((uint32_t)v.y+(uint32_t)height),v.z},
        {bits((uint32_t)v.x+(uint32_t)half_width),v.y,v.z}};
    int32_t hx=shift((uint32_t)v.z*(uint32_t)horizontal,8);
    int32_t vy=shift((uint32_t)v.z*(uint32_t)vertical,8);
    TaxiPoint tl=result.billboard.top_left,br=result.billboard.bottom_right;
    /* Four local corners have the same Z; AND of their side flags is
       equivalent to testing both endpoints independently on each axis. */
    int32_t nx=bits(0u-(uint32_t)hx),ny=bits(0u-(uint32_t)vy);
    if((tl.x<=nx&&br.x<=nx)||(tl.x>=hx&&br.x>=hx)||
       (tl.y<=ny&&br.y<=ny)||(tl.y>=vy&&br.y>=vy))return 0;
    TaxiTrafficProjection projection=taxi_traffic_projection_normal();
    int rc=taxi_traffic_project(&result.billboard,&projection,&result.screen);
    if(rc<=0)return rc;
    if(taxi_traffic_depth_bucket(v.z,1536,projection.far_plane,trig,&result.bucket))return -1;
    result.u[0]=result.u[1]=result.flip?(64<<14):0;
    result.u[2]=result.u[3]=result.flip?0:(64<<14);
    result.v[0]=result.v[3]=64<<14;
    result.v[1]=result.v[2]=0;
    *out=result;return 1;
}