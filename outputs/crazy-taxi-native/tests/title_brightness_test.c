#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/palette_fade.h"
#include <assert.h>
#include <stdio.h>
int main(void){
 for(int mode=0;mode<2;mode++)for(int p=-32768;p<=32767;p++){
  /* Independent floor-half arithmetic, avoiding implementation's signed
     division/remainder path. This proves register math, not GPU output. */
  int half=p>=0?p/2:-((-p+1)/2);
  int expected=16-half;
  if(mode==1&&expected<0)expected=0;
  if(mode==0&&expected>16)expected=16;
  uint16_t actual=0;
  assert(taxi_palette_fade_bldy((uint8_t)mode,(int16_t)p,&actual));
  assert(actual==(uint16_t)expected);
 }
 uint16_t value=123;
 assert(!taxi_palette_fade_bldy(2,0,&value)&&value==123);
 assert(!taxi_palette_fade_bldy(0,0,0));
 assert(taxi_palette_fade_bldy(1,0,&value)&&value==16);
 assert(taxi_palette_fade_bldy(1,31,&value)&&value==1);
 assert(taxi_palette_fade_bldy(1,32,&value)&&value==0);
 assert(taxi_palette_fade_bldy(0,-1,&value)&&value==16);
 for(unsigned bldy=0;bldy<64;bldy++)for(unsigned color=0;color<32768;color++){
  unsigned coefficient=(bldy&16)?16:bldy&15,expected=0;
  for(unsigned shift=0;shift<15;shift+=5){
   unsigned channel=(color>>shift)&31;
   expected|=((channel*(16-coefficient)+15)/16)<<shift;
  }
  assert(taxi_palette_fade_darken_rgb555((uint16_t)color,(uint16_t)bldy)==expected);
 }
 for(unsigned bldy=0;bldy<32;bldy++)for(unsigned channel=0;channel<32;channel++){
  for(unsigned encoding=0;encoding<2;encoding++){
   uint8_t source[4]={(uint8_t)(encoding?((channel<<3)|(channel>>2)):channel*255/31),0,0,73},out[4];
   source[1]=source[2]=source[0];
   taxi_palette_fade_darken_rgba(source,out,1,(uint16_t)bldy);
   unsigned coefficient=bldy>16?16:bldy,dark=(channel*(16-coefficient)+15)/16;
   assert(out[0]==(dark==channel?source[0]:dark*255/31));
   assert(out[1]==out[0]&&out[2]==out[0]&&out[3]==73);
   taxi_palette_fade_darken_rgba(source,source,1,(uint16_t)bldy);
   for(unsigned c=0;c<4;c++)assert(source[c]==out[c]);
  }
 }
 puts("PASS title brightness: all signed register progress/modes, all RGB555 colors/64 coefficients, RGBA encodings and in-place alpha preservation; GPU output checked separately.");
}
