#include "../src/startup_obj_transport.h"
#include <stdio.h>
int main(void){unsigned total,seed;while(scanf("%u %u",&total,&seed)==2){
 TaxiStartupObjQueue q={.total=(uint8_t)total,.count={17,33,65},.last_width=99,.last_height=165};TaxiStartupObjTransport t={.previous_total=37,.matrix_count=29};
 for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)q.attributes[b][i][j]=(uint16_t)(seed+b*1000u+i*37u+j*53u);
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++){q.depth[i][j]=(uint16_t)(seed+i*37u+j*53u);t.output[i][j]=(uint16_t)(seed+i*91u+j*17u);}
 for(unsigned i=0;i<32;i++){t.matrices[i].state=seed+i;for(unsigned j=0;j<4;j++)t.matrices[i].values[j]=(int16_t)(seed+i*37u+j*53u);}
 if(taxi_startup_obj_cold_clear(&q,&t))return 1;
 printf("%u %u %u %u %u %u %u %u ",q.total,q.count[0],q.count[1],q.count[2],q.last_width,q.last_height,t.previous_total,t.matrix_count);
 for(unsigned b=0;b<3;b++)for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.attributes[b][i][j]&255,q.attributes[b][i][j]>>8);
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",q.depth[i][j]&255,q.depth[i][j]>>8);
 const unsigned char *m=(const unsigned char *)t.matrices;for(unsigned i=0;i<sizeof t.matrices;i++)printf("%02x",m[i]);
 for(unsigned i=0;i<128;i++)for(unsigned j=0;j<4;j++)printf("%02x%02x",t.output[i][j]&255,t.output[i][j]>>8);putchar('\n');
 }return 0;}
