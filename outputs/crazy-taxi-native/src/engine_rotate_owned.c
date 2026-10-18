#include "engine_rotate_owned.h"
#include "engine_matrix_call.h"
#include <string.h>
static int32_t bits(uint32_t v){int32_t s;memcpy(&s,&v,4);return s;}
static int32_t shift(uint32_t v){return bits((v>>8)|((v&0x80000000u)?0xff000000u:0));}
static void rotation(int32_t m[16],unsigned axis,int32_t angle,const TaxiTrig *t){
 memset(m,0,64);m[0]=m[5]=m[10]=m[15]=256;int32_t s=t->sine[(uint32_t)angle&4095],c=t->cosine[(uint32_t)angle&4095];
 if(axis==0){m[5]=m[10]=c;m[9]=s;m[6]=bits(0u-(uint32_t)s);}
 if(axis==1){m[0]=m[10]=c;m[2]=s;m[8]=bits(0u-(uint32_t)s);}
 if(axis==2){m[0]=m[5]=c;m[1]=s;m[4]=bits(0u-(uint32_t)s);}
}
int taxi_engine_rotate_owned(TaxiPoint p,int32_t x,int32_t y,int32_t z,const TaxiTrig *t,unsigned camera,TaxiPoint *out){
 if(!t||!out||camera>1)return -1;int32_t rx[16],ry[16],rz[16],temp[16],m[16],v[3];rotation(rx,0,x,t);rotation(ry,1,y,t);rotation(rz,2,z,t);
 if(taxi_engine_matrix_call(0x03006a98,camera?rz:ry,rx,temp)||taxi_engine_matrix_call(0x03006a98,temp,camera?ry:rz,m))return -1;
 for(unsigned i=0;i<3;i++){uint32_t sum=(uint32_t)p.x*(uint32_t)m[i]+(uint32_t)p.y*(uint32_t)m[4+i]+(uint32_t)p.z*(uint32_t)m[8+i];v[i]=bits((uint32_t)shift(sum)+(uint32_t)m[12+i]);}
 *out=(TaxiPoint){v[0],v[1],v[2]};return 0;
}
