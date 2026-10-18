#include "../src/startup_obj_transport.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 for(unsigned affine=0;affine<2;affine++){
 TaxiStartupObjQueue q={0};TaxiStartupObjTransport t={0};
 uint16_t base[128][4];TaxiObjMatrixSlot matrices[32];
 memset(base,0x91,sizeof base);memset(matrices,0x37,sizeof matrices);
 q.total=4;q.count[0]=1;q.count[1]=1;q.count[2]=2;q.last_width=64;
 q.attributes[0][0][0]=10;q.attributes[1][0][0]=20;
 q.attributes[2][0][0]=30;q.attributes[2][1][0]=40;
 q.depth[0][2]=5;q.depth[1][2]=1;q.depth[0][3]=123;q.depth[1][3]=456;
 t.matrix_count=(uint8_t)affine;
 for(unsigned i=0;i<32;i++)for(unsigned j=0;j<4;j++)t.matrices[i].values[j]=(int16_t)(i*4+j+100);
 assert(taxi_startup_obj_publish(&q,&t,base,matrices)==0);
 assert(t.output[0][0]==10&&t.output[1][0]==20&&t.output[2][0]==40&&t.output[3][0]==30);
 assert(q.depth[0][3]==456&&q.depth[1][3]==123&&q.attributes[2][0][0]==40);
 for(unsigned i=0;i<128;i++)assert(t.output[i][3]==(affine?i+100:0));
 assert(!memcmp(base,q.attributes[0],sizeof base)&&!memcmp(matrices,t.matrices,sizeof matrices));
 assert(t.previous_total==4&&!q.total&&!t.matrix_count&&!q.count[0]&&!q.count[1]&&!q.count[2]&&q.last_width==64);
 }
 puts("PASS active publication order, paired sorting, conditional matrices, template restoration");return 0;
}
