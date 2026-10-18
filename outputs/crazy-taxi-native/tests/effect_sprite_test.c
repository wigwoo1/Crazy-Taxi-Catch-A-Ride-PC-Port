#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "effect_sprite.h"
typedef struct {int calls;} Random;
static void put(uint8_t *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static int32_t random_value(void *ctx,int32_t lo,int32_t hi){Random *r=ctx;unsigned n=(unsigned)r->calls++;
 assert(n<3);assert(lo==(n==0?0:n==1?-5:4));assert(hi==(n==0?65535:n==1?5:8));
 return n==0?5:n==1?-3:7;
}
int main(void){
 TaxiEffectSprite s={0};Random r={0};TaxiPoint p={-10,10,600};
 static TaxiTrig t;t.cosine[0]=256;t.sine[1024]=256;
 TaxiEffectRecord record={{0}};record.bytes[0x34]=5;record.bytes[0x35]=3;
 put(record.bytes+4,(uint32_t)-90);put(record.bytes+8,(uint32_t)-180);put(record.bytes+12,(uint32_t)-270);
 assert(taxi_effect_world_sprite(&record,(TaxiPoint){10,20,30},-1024,-1024,0,&t,100,120,80,0,1,NULL,NULL,&s)==1);
 assert(s.position.x==270&&s.position.z==200&&s.scale==512&&s.position.y==94);
 assert(taxi_effect_sprite(p,100,120,80,1,2,0,0,NULL,NULL,&s)==1);
 assert(s.position.x==119&&s.position.y==79&&s.scale==257);
 assert(s.group==8&&s.frame==2&&s.flags==4&&s.bucket==2);
 assert(taxi_effect_sprite(p,100,120,80,3,99,1138,0,random_value,&r,&s)==1);
 assert(r.calls==3&&s.position.x==116&&s.position.y==72);
 assert(s.group==9&&s.frame==2&&s.palette==1&&s.scale==512);
 r.calls=0;
 assert(taxi_effect_sprite(p,100,120,80,2,99,0,1,random_value,&r,&s)==1);
 assert(!r.calls&&s.frame==0&&s.palette==0);
 assert(taxi_effect_sprite(p,100,120,80,5,3,0,0,NULL,NULL,&s)==1);
 assert(s.scale==384&&s.position.y==67&&s.group==10&&s.frame==8);
 for(unsigned mode=0;mode<256;mode++){
  assert(taxi_effect_sprite(p,100,120,80,(uint8_t)mode,255,0,1,NULL,NULL,&s)==(mode?1:0));
  if(mode==4)assert(s.frame==255);
  if(mode>5)assert(s.frame==4&&s.group==10);
 }
 TaxiEffectSprite saved=s;p.z=100;
 assert(taxi_effect_sprite(p,100,120,80,3,0,0,0,random_value,&r,&s)==0);
 assert(!r.calls&&memcmp(&saved,&s,sizeof s)==0);
 p.z=600;
 assert(taxi_effect_sprite(p,100,120,80,2,0,0,0,NULL,NULL,&s)==-1);
 assert(memcmp(&saved,&s,sizeof s)==0);
 puts("Effect sprite: projection, exact scale, all type branches, palette and ordered RNG PASS");
 return 0;
}
