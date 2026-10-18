#include "../src/world_material.h"
#include <stdio.h>
#include <stdint.h>
int main(void){uint8_t m[28]={0};m[26]=81;
 int32_t phases[]={0,1,202,203,300,-1,2147483647,(-2147483647-1)};
 for(unsigned n=0;n<sizeof phases/sizeof phases[0];n++){
 TaxiWorldMaterialPolygon p={.count=4};for(unsigned k=0;k<4;k++){p.vertices[k].u=123<<14;p.vertices[k].v=45<<14;}
 if(taxi_world_material_scroll(&p,m,phases[n],6,6))return 1;
 uint32_t v=((uint64_t)(uint32_t)phases[n]*81)%16384;
 for(unsigned k=0;k<4;k++)if(p.vertices[k].u!=(123<<14)||(uint32_t)p.vertices[k].v!=(45u<<14)+(v<<6))return 2;
 }
 TaxiWorldMaterialPolygon p={.count=4};if(taxi_world_material_scroll(&p,m,1,8,6)!=-1)return 3;
 puts("PASS original Bound material72 scrolling: zero/positive/wrap/negative/INT phases and rotated ARM word, unsupported exponent rejected. Arithmetic fixtures, no independent ROM execution.");return 0;}
