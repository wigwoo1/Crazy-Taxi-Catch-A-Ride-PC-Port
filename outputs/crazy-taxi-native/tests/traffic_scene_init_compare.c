#include "traffic_init.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static void hex(const void *p,size_t n){const unsigned char *b=p;for(size_t j=0;j<n;j++)printf("%02x",b[j]);}
int main(void){
 unsigned setting,seed,mask;
 while(scanf("%u %u %u",&setting,&seed,&mask)==3){
  if(setting>255||seed>255||mask>7)return 2;
  uint8_t raw[4600],sprites[576],typed_sprites[576];TaxiTrafficActor actors[50],expected[50];uint32_t params[3],typed_params[3];
  for(unsigned i=0;i<sizeof raw;i++)raw[i]=(uint8_t)(seed+i*37);
  for(unsigned i=0;i<576;i++)sprites[i]=typed_sprites[i]=(uint8_t)(seed+i*37);
  for(size_t i=0;i<sizeof actors;i++)((unsigned char *)actors)[i]=(uint8_t)(seed+i*19);memcpy(expected,actors,sizeof actors);
  for(unsigned i=0;i<3;i++)params[i]=typed_params[i]=mask&(1u<<i)?0:seed+i+1;
  unsigned density=taxi_traffic_density((uint8_t)setting);
  for(unsigned i=0;i<(density?density:1);i++)expected[i].countdown=(int32_t)(180*(i+1));
  assert(!taxi_traffic_initialize(raw,sizeof raw,density,params,sprites));
  assert(!taxi_traffic_scene_reinitialize(actors,50,density,typed_params,typed_sprites));
  assert(!memcmp(actors,expected,sizeof actors)&&!memcmp(params,typed_params,sizeof params)&&!memcmp(sprites,typed_sprites,576));
  TaxiTrafficActor before[50];memcpy(before,actors,sizeof before);uint8_t saved_sprites[576];memcpy(saved_sprites,typed_sprites,576);uint32_t saved_params[3];memcpy(saved_params,params,sizeof params);
  assert(taxi_traffic_scene_reinitialize(actors,density-1,density,typed_params,typed_sprites)==-1&&!memcmp(actors,before,sizeof actors)&&!memcmp(typed_sprites,saved_sprites,576)&&!memcmp(typed_params,saved_params,sizeof params));
  assert(taxi_traffic_scene_reinitialize(actors,50,density,typed_params,NULL)==-1&&!memcmp(actors,before,sizeof actors)&&!memcmp(typed_params,saved_params,sizeof params));
  printf("%u ",density);hex(raw,sizeof raw);putchar(' ');hex(typed_sprites,576);for(unsigned i=0;i<3;i++)printf(" %u",typed_params[i]);putchar('\n');
 }
 puts("DONE");return 0;
}
