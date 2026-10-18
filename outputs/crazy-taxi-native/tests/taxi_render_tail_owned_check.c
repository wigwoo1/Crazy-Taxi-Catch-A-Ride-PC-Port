#ifdef NDEBUG
#undef NDEBUG
#endif
#include "taxi_render_tail_owned.h"
#include "taxi_graphics_package.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static void guards(const uint8_t *rom,size_t size){
 TaxiPaletteRegistration reg,prior_reg;TaxiPaletteBanks banks,prior_banks;TaxiPaletteTransfer tr={1,2,3},prior_tr=tr;
 memset(&reg,165,sizeof reg);memset(&banks,165,sizeof banks);prior_reg=reg;prior_banks=banks;
 uint8_t state[3]={1,0,7},before[3];memcpy(before,state,3);uint32_t first[3]={7,8,9},anchor[3]={11,12,13},held[3];memcpy(held,anchor,12);
 assert(taxi_render_tail_owned(&reg,&banks,&tr,rom,0x20fcff,state,0,1,first,anchor)==-1);
 assert(!memcmp(&reg,&prior_reg,sizeof reg)&&!memcmp(&banks,&prior_banks,sizeof banks)&&!memcmp(&tr,&prior_tr,sizeof tr)&&!memcmp(state,before,3)&&!memcmp(anchor,held,12));
 assert(taxi_render_tail_owned(NULL,&banks,&tr,rom,size,state,0,1,first,anchor)==-1);
 assert(!memcmp(state,before,3)&&!memcmp(anchor,held,12));
 state[0]=0;assert(!taxi_render_tail_owned(NULL,NULL,NULL,NULL,0,state,0,UINT32_MAX,first,anchor));
 assert(!memcmp(first,anchor,12)&&state[1]==0&&state[2]==7);
 assert(taxi_render_tail_owned(NULL,NULL,NULL,NULL,0,NULL,0,1,first,anchor)==-1);
}
int main(int argc,char **argv){
 if(argc!=4)return 1;uint8_t *rom=NULL;size_t size=8388608;TaxiGraphicsPackage package={0};
 if(!strcmp(argv[1],"package")){if(taxi_graphics_package_load(&package,argv[2]))return 2;rom=package.bytes;size=package.size;}
 else{FILE *f=fopen(argv[2],"rb");rom=malloc(size);if(!f||!rom||fread(rom,1,size,f)!=size||fgetc(f)!=EOF)return 3;fclose(f);}
 guards(rom,size);FILE *out=fopen(argv[3],"wb");if(!out)return 4;
 unsigned failure,phase,timer,fade,delta,x,y,z,count=0;
 while(scanf("%u%u%u%u%u%u%u%u",&failure,&phase,&timer,&fade,&delta,&x,&y,&z)==8){
  TaxiPaletteRegistration reg;TaxiPaletteBanks banks;TaxiPaletteTransfer tr={0x11111111,0x22222222,0x33333333};
  memset(&reg,165,sizeof reg);memset(&banks,165,sizeof banks);uint8_t state[3]={(uint8_t)failure,(uint8_t)phase,(uint8_t)timer};uint32_t first[3]={x,y,z},anchor[3]={11,12,13};
  int result=taxi_render_tail_owned(&reg,&banks,&tr,rom,size,state,(uint8_t)fade,delta,first,anchor);if(result<0)return 5;
  assert(reg.count==0xa5a5&&reg.bg_bytes==0xa5a5&&reg.obj_bytes==0xa5a5&&reg.flags==0xa5);
  for(unsigned i=0;i<128;i++)for(unsigned j=0;j<20;j++)if(!(result&&i==4&&j>=16))assert(reg.records[i][j]==165);
  for(unsigned i=0;i<256;i++){assert(banks.background[i]==0xa5a5);if(!(result&&i>=64&&i<80))assert(banks.objects[i]==0xa5a5);}
  uint8_t h[4]={(uint8_t)result,state[0],state[1],state[2]};
  if(fwrite(h,1,4,out)!=4||fwrite(anchor,1,12,out)!=12||fwrite(&tr,1,12,out)!=12||fwrite(reg.records[4]+16,1,4,out)!=4||fwrite(banks.objects+64,1,32,out)!=32)return 6;count++;
 }
 if(fclose(out))return 7;if(package.bytes)taxi_graphics_package_release(&package);else free(rom);
 printf("PASS %u native renderer-tail vectors and transactional missing-source/owner guards\n",count);return 0;
}