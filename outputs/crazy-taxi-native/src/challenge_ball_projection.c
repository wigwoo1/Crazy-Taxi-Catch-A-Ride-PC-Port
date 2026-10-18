#include "challenge_ball_projection.h"
#include "indexed_pixel.h"
#include <string.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t shift(uint32_t u,unsigned n){return bits((u>>n)|((u&0x80000000u)?(UINT32_MAX<<(32-n)):0));}
int taxi_challenge_ball_project(const TaxiBallRenderRequest *r,TaxiPoint camera,int32_t yaw,int32_t pitch,int32_t horizontal,int32_t vertical,const TaxiTrig *trig,TaxiBallProjection *out){
 if(!r||!trig||!out)return -1;
 uint32_t x=(uint32_t)r->x-(uint32_t)camera.x,y=(uint32_t)r->y-(uint32_t)camera.y,z=(uint32_t)r->z-(uint32_t)camera.z;
 unsigned a=(0u-(uint32_t)yaw)&4095,b=(0u-(uint32_t)pitch)&4095;
 int32_t rx=shift(x*(uint32_t)trig->cosine[a]-z*(uint32_t)trig->sine[a],8),rz=shift(x*(uint32_t)trig->sine[a]+z*(uint32_t)trig->cosine[a],8);
 TaxiPoint center={rx,shift((uint32_t)rz*(uint32_t)trig->sine[b]+y*(uint32_t)trig->cosine[b],8),shift((uint32_t)rz*(uint32_t)trig->cosine[b]-y*(uint32_t)trig->sine[b],8)};
 TaxiTrafficProjection normal=taxi_traffic_projection_normal();if(center.z<=1024||center.z>=normal.far_plane)return 0;
 int32_t half=shift((uint32_t)r->scale*32u,17),height=shift((uint32_t)r->scale*32u,16);
 TaxiTrafficBillboard rectangle={{bits((uint32_t)center.x-(uint32_t)half),bits((uint32_t)center.y+(uint32_t)height),center.z},{bits((uint32_t)center.x+(uint32_t)half),center.y,center.z}};
 int32_t hx=shift((uint32_t)center.z*(uint32_t)horizontal,8),vy=shift((uint32_t)center.z*(uint32_t)vertical,8),nx=bits(0u-(uint32_t)hx),ny=bits(0u-(uint32_t)vy);
 TaxiPoint corners[4]={rectangle.top_left,{rectangle.bottom_right.x,rectangle.top_left.y,center.z},rectangle.bottom_right,{rectangle.top_left.x,rectangle.bottom_right.y,center.z}};
 unsigned common=15,any=0;for(unsigned i=0;i<4;i++){TaxiPoint p=corners[i];unsigned f=(p.x<=nx?1u:0u)|(p.x>=hx?2u:0u)|(p.y<=ny?4u:0u)|(p.y>=vy?8u:0u);common&=f;any|=f;}
 if(common)return 0;
 TaxiBallProjection p={.packet_flags=(uint16_t)(8|(any?4:0)),.bitmap_identity=r->bitmap_identity};
 int rc=taxi_traffic_project(&rectangle,&normal,&p.bounds);if(rc<=0)return rc;
 if(taxi_traffic_depth_bucket(center.z,1536,normal.far_plane,trig,&p.bucket))return -1;
 *out=p;return 1;
}
int taxi_challenge_ball_indexed(uint8_t pixels[38400],const TaxiBallProjection *p,const uint8_t atlas[8192]){
 if(!pixels||!p||!atlas||(p->packet_flags!=8&&p->packet_flags!=12)||p->bitmap_identity<0x0849b0c9u)return -1;
 uint32_t offset=p->bitmap_identity-0x0849b0c9u;if(offset>=8192||(offset&1023))return -1;
 return taxi_indexed_billboard(pixels,p->bounds.left,p->bounds.top,p->bounds.right,p->bounds.bottom,atlas+offset,1024,32,32,5,(p->packet_flags&4)!=0)?1:-1;
}
