#include "destination_guidance.h"
#include <string.h>
#include <stdio.h>
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int32_t word(const uint8_t *p){return bits(p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24));}
static int32_t sub(int32_t a,int32_t b){return bits((uint32_t)a-(uint32_t)b);}
static int32_t asr8(int32_t v){return bits(((uint32_t)v>>8)|(v<0?0xff000000u:0));}
unsigned taxi_guidance_frame_step(unsigned current,unsigned target,int32_t delta){
    int32_t a=(int32_t)(current&31)*128,b=(int32_t)(target&31)*128;
    int32_t forward=(b-a)&4095,backward=(a-b)&4095;
    int32_t difference=forward<backward?forward:-backward;
    /* At equal half-turn the original direction depends on ordering. */
    if(forward==backward&&a>b)difference=forward;
    int32_t magnitude=difference<0?-difference:difference;
    int32_t step=bits((uint32_t)delta<<5);
    int32_t result=magnitude<=step?b:bits((uint32_t)a+(difference<0?0u-(uint32_t)step:(uint32_t)step));
    return ((uint32_t)result>>7)&31;
}
int taxi_guidance_route_load(TaxiGuidanceRoute *out,const char *path){
    if(!out||!path)return -1;
    FILE *f=fopen(path,"rb");if(!f)return -1;
    uint8_t header[8];TaxiGuidanceRoute route={0};TaxiPoint points[80];
    if(fread(header,1,8,f)!=8||memcmp(header,"CTG1",4)){fclose(f);return -1;}
    uint32_t count=(uint32_t)word(header+4);
    if(count>80){fclose(f);return -1;}
    for(unsigned i=0;i<count;i++){
        uint8_t raw[16];if(fread(raw,1,16,f)!=16){fclose(f);return -1;}
        points[i]=(TaxiPoint){word(raw+4),0,word(raw+8)};
    }
    int extra=fgetc(f),failed=ferror(f);fclose(f);
    if(extra!=EOF||failed||taxi_guidance_segments(route.segments,points,count))return -1;
    route.count=count;*out=route;return 0;
}
static const uint8_t *scene_range(const uint8_t *rom,size_t size,uint32_t identity,size_t amount){
 if(identity<0x08000000u)return NULL;size_t off=identity-0x08000000u;
 return off>size||amount>size-off?NULL:rom+off;
}
int taxi_guidance_scene(TaxiGuidanceRoute *out,const uint8_t *descriptor,size_t descriptor_size,const uint8_t *rom,size_t rom_size){
 if(!out||!descriptor||descriptor_size!=104||!rom)return -1;
 out->count=0;unsigned paths=descriptor[16]|(unsigned)descriptor[17]<<8;
 const uint8_t *records=scene_range(rom,rom_size,(uint32_t)word(descriptor+52),(size_t)paths*20);
 if(paths&&!records)return -1;
 for(unsigned i=0;i<paths;i++){
  const uint8_t *r=records+i*20;if(memcmp(r+4,"ARROW",6))continue;
  uint32_t count=(uint32_t)word(r);if(count>80)return -1;
  const uint8_t *raw=scene_range(rom,rom_size,(uint32_t)word(r+16),(size_t)count*16);
  if(count&&!raw)return -1;TaxiPoint points[80];
  for(unsigned j=0;j<count;j++)points[j]=(TaxiPoint){word(raw+j*16+4),0,word(raw+j*16+8)};
  if(taxi_guidance_segments(out->segments,points,count))return -1;
  out->count=count;break;
 }
 return 0;
}
static uint32_t root(uint32_t n){uint32_t r=0,b=1u<<30;while(b>n)b>>=2;while(b){if(n>=r+b){n-=r+b;r=(r>>1)+b;}else r>>=1;b>>=2;}return r;}
unsigned taxi_destination_palette(int32_t distance){return distance>500?0:distance>250?1:distance>150?2:distance>100?3:4;}
uint16_t taxi_destination_vertical_scale(int32_t distance){
    if(taxi_destination_palette(distance)!=4)return 256;
    int32_t n=bits((uint32_t)sub(100,distance)<<7);
    return (uint16_t)((uint32_t)(n/100)+256u);
}
uint16_t taxi_guidance_scale_step(uint16_t current,uint16_t target,int32_t delta){
    int32_t a=current<32768?current:(int32_t)current-65536;
    int32_t b=target<32768?target:(int32_t)target-65536;
    uint16_t next=current;
    if(a<b)next=(uint16_t)((uint32_t)current+(uint32_t)delta);
    else if(a>b)next=(uint16_t)((uint32_t)current-(uint32_t)delta);
    int32_t c=next<32768?next:(int32_t)next-65536;
    if((a<b&&c>=b)||(a>b&&c<=b))next=target;
    return next;
}
int taxi_guidance_segments(TaxiGuidanceSegment *out,const TaxiPoint *points,unsigned count){
    if(count>80||(count&&(!out||!points)))return -1;
    TaxiGuidanceSegment segments[80];
    for(unsigned i=0;i<count;i++){
        const TaxiPoint a=points[i],b=points[(i+1)%count];
        int32_t dx=sub(b.x,a.x),dz=sub(b.z,a.z);
        uint32_t x=(uint32_t)asr8(dx),z=(uint32_t)asr8(dz);
        int32_t length=(int32_t)((root(x*x+z*z)&65535u)<<8);
        int32_t nx=bits((uint32_t)dx<<8),nz=bits((uint32_t)dz<<8);
        segments[i]=(TaxiGuidanceSegment){a.x,a.z,b.x,b.z,
            length?nx/length:0,length?nz/length:0,length};
    }
    if(count)memcpy(out,segments,count*sizeof(*out));return 0;
}
int32_t taxi_destination_distance(int32_t selected,const uint8_t *record,int32_t x,int32_t z){
    if(selected==-1||!record)return -1;
    uint32_t dx=(uint32_t)asr8(sub(word(record+24),x)),dz=(uint32_t)asr8(sub(word(record+32),z));
    int32_t d=sub((int32_t)(root(dx*dx+dz*dz)&65535u),asr8(word(record+36)));
    return d<0?0:d;
}
int taxi_destination_angle(int32_t selected,const uint8_t *record,TaxiPoint camera,
    int32_t angle,const TaxiTrig *trig,const TaxiAngleTable *table,int32_t *out){
    if(!out)return -1;
    if(selected==-1||!record){*out=-1;return 0;}
    if(!trig||!table)return -1;
    TaxiPoint relative={sub(word(record+24),camera.x),0,sub(word(record+32),camera.z)};
    relative=taxi_rotate_y(relative,bits(0u-(uint32_t)angle),trig);
    return taxi_vector_angle(asr8(relative.x),asr8(relative.z),table,out);
}
static uint32_t segment_distance(const TaxiGuidanceSegment *s,int32_t x,int32_t z){
    int32_t dx=asr8(sub(x,s->x1)),dz=asr8(sub(z,s->z1));
    int32_t dot=bits((uint32_t)dx*(uint32_t)s->unit_x+(uint32_t)dz*(uint32_t)s->unit_z);
    if(dot<=0){
        dx=asr8(sub(x,s->x0));dz=asr8(sub(z,s->z0));
        dot=bits((uint32_t)dx*(uint32_t)s->unit_x+(uint32_t)dz*(uint32_t)s->unit_z);
        if(dot>=0){
            int32_t px=bits((uint32_t)s->x0+(uint32_t)asr8(bits((uint32_t)s->unit_x*(uint32_t)dot)));
            int32_t pz=bits((uint32_t)s->z0+(uint32_t)asr8(bits((uint32_t)s->unit_z*(uint32_t)dot)));
            dx=asr8(sub(x,px));dz=asr8(sub(z,pz));
        }
    }
    return (uint32_t)dx*(uint32_t)dx+(uint32_t)dz*(uint32_t)dz;
}
static int nearest(const TaxiGuidanceSegment *s,unsigned count,int32_t x,int32_t z){
    uint32_t best=UINT32_MAX;int index=-1;
    for(unsigned i=0;i<count;i++){uint32_t d=segment_distance(s+i,x,z);if(d<best){best=d;index=(int)i;}}
    return index;
}
int taxi_guidance_route_angle(const TaxiGuidanceSegment *s,unsigned count,
    int32_t selected,const uint8_t *dest,TaxiPoint taxi,TaxiPoint camera,
    int32_t camera_angle,const TaxiTrig *trig,const TaxiAngleTable *table,int32_t *out){
    if(!out||count>80||(count&&(!s||!dest)))return -1;
    int32_t angle;
    if(taxi_destination_angle(selected,dest,camera,camera_angle,trig,table,&angle))return -1;
    if(!count){*out=angle&4095;return 0;}
    int target=nearest(s,count,word(dest+24),word(dest+32));
    int current=nearest(s,count,camera.x,camera.z);
    if(target<0||current<0)return -1;
    if(target==current){*out=angle&4095;return 0;}
    uint32_t forward=0,backward=0;
    for(int i=current;i!=target;i=(i+1)%(int)count)forward+=(uint32_t)s[i].length;
    for(int i=current;i!=target;i=(i+(int)count-1)%(int)count)backward+=(uint32_t)s[i].length;
    int next=bits(forward)<bits(backward)?(current+1)%(int)count:(current+(int)count-1)%(int)count;
    TaxiPoint point=bits(forward)<bits(backward)?(TaxiPoint){s[next].x1,0,s[next].z1}:
        (TaxiPoint){s[next].x0,0,s[next].z0};
    point.x=sub(point.x,taxi.x);point.z=sub(point.z,taxi.z);
    point=taxi_rotate_y(point,bits(0u-(uint32_t)camera_angle),trig);
    /* Unlike direct destination helper, original route branch does NOT ASR8. */
    if(taxi_vector_angle(point.x,point.z,table,&angle))return -1;
    *out=angle&4095;return 0;
}
