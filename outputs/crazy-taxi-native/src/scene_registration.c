#include "scene_registration.h"
#include <stdio.h>
#include <string.h>
static uint16_t word(const uint8_t *p){return (uint16_t)(p[0]|p[1]<<8);}
int taxi_scene_registration_set(TaxiSceneRegistration *out,const uint8_t *bytes,size_t size){
 if(!out||!bytes||size!=104||word(bytes)!=4||word(bytes+10)>12288||word(bytes+12)>4096||word(bytes+14)>128||word(bytes+4)>512)return -1;
 TaxiSceneRegistration next={0};memcpy(next.descriptor,bytes,104);
 next.materials=word(bytes+4);next.textures=word(bytes+6);next.vertices=word(bytes+8);next.polygons=word(bytes+10);next.special=word(bytes+12);next.groups=word(bytes+14);
 uint16_t bits=word(bytes+20);memcpy(&next.background,&bits,2);next.registered=1;*out=next;return 0;
}
int taxi_scene_registration_load(TaxiSceneRegistration *out,const char *path){
 if(!path)return -1;FILE *f=fopen(path,"rb");if(!f)return -1;
 uint8_t bytes[104];int valid=fread(bytes,1,sizeof bytes,f)==sizeof bytes&&fgetc(f)==EOF;fclose(f);
 return valid?taxi_scene_registration_set(out,bytes,sizeof bytes):-1;
}
