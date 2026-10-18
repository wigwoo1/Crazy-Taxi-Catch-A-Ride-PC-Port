#include "traffic_init.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
int main(void){unsigned setting,seed,mask;while(scanf("%u %u %u",&setting,&seed,&mask)==3){
 uint8_t actors[4600],sprites[576];uint32_t params[3];
 for(unsigned i=0;i<4600;i++)actors[i]=(uint8_t)(seed+i*37);
 for(unsigned i=0;i<576;i++)sprites[i]=(uint8_t)(seed+i*37);
 for(unsigned i=0;i<3;i++)params[i]=(mask&(1u<<i))?0:seed+i+1;
 TaxiTrafficActor native[50],expected[50];uint32_t native_params[3];
 for(unsigned i=0;i<sizeof native;i++)((uint8_t *)native)[i]=(uint8_t)(seed+i*37);
 memcpy(expected,native,sizeof native);memcpy(native_params,params,sizeof params);
 unsigned density=taxi_traffic_density((uint8_t)setting);
 assert(!taxi_traffic_initialize(actors,sizeof actors,density,params,sprites));
 assert(!taxi_traffic_actors_reinitialize(native,50,density,native_params));
 for(unsigned i=0;i<density;i++)expected[i].countdown=(int32_t)(180u*(i+1));
 assert(!memcmp(native,expected,sizeof native));assert(!memcmp(native_params,params,sizeof params));
 printf("%u ",density);for(unsigned i=0;i<4600;i++)printf("%02x",actors[i]);printf(" ");
 for(unsigned i=0;i<576;i++)printf("%02x",sprites[i]);printf(" %u %u %u",params[0],params[1],params[2]);
 for(unsigned i=0;i<density;i++)printf(" %d",native[i].countdown);puts("");
 }return 0;}
