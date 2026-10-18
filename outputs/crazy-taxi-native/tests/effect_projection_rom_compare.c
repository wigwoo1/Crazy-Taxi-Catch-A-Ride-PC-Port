#include "../src/effect_sprite.h"
#include <stdio.h>
#include <string.h>
static int calls;
static int32_t random_value(void *c,int32_t lo,int32_t hi){(void)c;(void)lo;(void)hi;return calls++%3==0?5:calls%3==2?-3:7;}
static void put(uint8_t *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(u>>(8*i));}
int main(void){FILE *f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");uint8_t b[16388];if(!f||fread(b,1,sizeof b,f)!=sizeof b)return 2;fclose(f);static TaxiTrig t;taxi_trig_decode(&t,b+4,b+8196);int x,y,z,pitch,yaw,roll,px,py,pz,type,frame,phase,disabled;while(scanf("%d %d %d %d %d %d %d %d %d %d %d %d %d",&x,&y,&z,&pitch,&yaw,&roll,&px,&py,&pz,&type,&frame,&phase,&disabled)==13){TaxiEffectRecord r={{0}};put(r.bytes+4,px);put(r.bytes+8,py);put(r.bytes+12,pz);r.bytes[52]=(uint8_t)type;r.bytes[53]=(uint8_t)frame;TaxiEffectSprite s={0};calls=0;int rc=taxi_effect_world_sprite(&r,(TaxiPoint){x,y,z},(int16_t)pitch,(int16_t)yaw,(int16_t)roll,&t,192,120,80,phase,disabled,random_value,0,&s);printf("%d %d",rc,calls);if(rc==1)printf(" %d %d %d %d %u %u %u %u %u %u",s.position.x,s.position.y,s.position.z,s.scale,s.group,s.frame,s.palette,s.bucket,s.priority,s.flags);puts("");}return 0;}
