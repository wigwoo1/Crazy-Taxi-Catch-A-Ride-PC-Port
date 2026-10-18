/* Full actual actor input/output for recovered 08015bb8 response routine. */
#include "../src/vehicle_math.h"
#include <stdio.h>
#include <string.h>
static unsigned char actor[0xe88];
static int32_t read32(const unsigned char *p){uint32_t u=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
static void write32(unsigned char *p,int32_t v){uint32_t u=(uint32_t)v;for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(u>>(8*i));}
int main(int argc,char **argv){
 if(argc!=3)return 1;
 FILE *f=fopen(argv[1],"rb");if(!f)return 2;
 size_t n=fread(actor,1,sizeof actor,f);int extra=fgetc(f),bad=ferror(f);fclose(f);
 if(n!=sizeof actor||extra!=EOF||bad)return 3;
 TaxiVehicleMath v={0};v.state_540=read32(actor+0x540);v.state_744=read32(actor+0x744);v.flags_4ac=actor[0x4ac];
 taxi_vehicle_response_step(&v);write32(actor+0x540,v.state_540);write32(actor+0x744,v.state_744);
 f=fopen(argv[2],"wb");if(!f)return 4;
 n=fwrite(actor,1,sizeof actor,f);bad=ferror(f);int closed=fclose(f);
 return n==sizeof actor&&!bad&&!closed?0:5;
}
