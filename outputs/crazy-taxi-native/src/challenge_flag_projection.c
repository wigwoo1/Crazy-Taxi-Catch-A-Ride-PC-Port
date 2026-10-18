#include "challenge_flag_projection.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t asr8(uint32_t u){return bits((u>>8)|((u&0x80000000u)?0xff000000u:0));}
int taxi_challenge_flag_project(const TaxiChallengeFlagRenderRequest *r,TaxiPoint camera,int32_t yaw,int32_t pitch,int32_t horizontal,int32_t vertical,const TaxiTrig *trig,TaxiChallengeFlagProjection *out){
 if(!r||!trig||!out)return -1;
 uint32_t x=(uint32_t)r->x-(uint32_t)camera.x,y=(uint32_t)r->y-(uint32_t)camera.y,z=(uint32_t)r->z-(uint32_t)camera.z;
 unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
 uint32_t yc=(uint32_t)trig->cosine[a],ys=(uint32_t)trig->sine[a],pc=(uint32_t)trig->cosine[b],ps=(uint32_t)trig->sine[b];
 int32_t rx=asr8(x*yc-z*ys),rz=asr8(x*ys+z*yc);
 TaxiPoint v={rx,asr8((uint32_t)rz*ps+y*pc),asr8((uint32_t)rz*pc-y*ps)};
 TaxiTrafficProjection projection=taxi_traffic_projection_normal();
 if(v.z<=projection.near_plane+768||v.z>=projection.far_plane)return 0;
 TaxiChallengeFlagProjection result={0};
 if(taxi_challenge_flag_corners(r,v,trig,result.camera_corners))return -1;
 int32_t hx=asr8((uint32_t)v.z*(uint32_t)horizontal),vy=asr8((uint32_t)v.z*(uint32_t)vertical),nx=bits(0u-(uint32_t)hx),ny=bits(0u-(uint32_t)vy);
 unsigned common=15,any=0;
 for(unsigned i=0;i<4;i++){
  TaxiPoint p=result.camera_corners[i];unsigned flags=(p.x<=nx?1u:0u)|(p.x>=hx?2u:0u)|(p.y<=ny?4u:0u)|(p.y>=vy?8u:0u);common&=flags;any|=flags;
 }
 if(common)return 0;
 for(unsigned i=0;i<4;i++){
  TaxiTrafficBillboard point={result.camera_corners[i],result.camera_corners[i]};TaxiTrafficScreenBounds screen;
  int rc=taxi_traffic_project(&point,&projection,&screen);if(rc<=0)return rc;
  result.screen_corners[i]=(TaxiPoint){screen.left,screen.top,v.z};
 }
 if(taxi_traffic_depth_bucket(v.z,1536,projection.far_plane,trig,&result.bucket))return -1;
 result.packet_flags=(uint16_t)((r->rotation_angle?32:8)|(any?4:0));
 *out=result;return 1;
}
