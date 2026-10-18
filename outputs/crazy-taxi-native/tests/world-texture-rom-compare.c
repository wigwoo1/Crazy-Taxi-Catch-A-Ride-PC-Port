#include "../src/world_material.h"
#include <stdio.h>
#include <string.h>
int main(void){unsigned base,count,rate;int phase;while(scanf("%u %u %u %d",&base,&count,&rate,&phase)==4){unsigned char material[28]={0},saved[28];material[16]=(unsigned char)base;material[17]=(unsigned char)count;material[18]=(unsigned char)rate;material[19]=(unsigned char)(rate>>8);memcpy(saved,material,28);unsigned selected=0xdeadbeef;int rc=taxi_world_material_texture(material,phase,256,&selected);if(memcmp(saved,material,28))return 2;printf("%d %u\n",rc,selected);}return 0;}
