#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_ball_hud.h"
int main(void){
 uint8_t positions[60];FILE *f=fopen("outputs/crazy-taxi-native/assets/challenge-flag/flag-hud-positions.bin","rb");assert(f);assert(fread(positions,1,60,f)==60&&fgetc(f)==EOF);fclose(f);
 TaxiBallHudPlan p;
 for(int count=0;count<=20;count++)for(unsigned paused=0;paused<2;paused++){
  assert(!taxi_challenge_ball_hud_plan(&p,2700,(int16_t)count,240,160,paused?32:0,paused,positions,60));
  assert(!strcmp(p.clock.time,"00'45\"00")&&p.clock.indicator_y==137&&p.clock.indicator_variant==paused+1);
  assert(p.counter_count==(count>=10?5u:4u));unsigned slot=0;
  if(count>=10){assert(p.counter[slot].glyph==16+count/10&&p.counter[slot].x==160);slot++;}
  assert(p.counter[slot].glyph==16+count%10&&p.counter[slot].x==176&&p.counter[slot].y==2);slot++;
  assert(p.counter[slot].glyph==15&&p.counter[slot].x==192);slot++;
  assert(p.counter[slot].glyph==18&&p.counter[slot].x==208);slot++;
  assert(p.counter[slot].glyph==16&&p.counter[slot].x==224);
  assert(p.label_id==615&&p.label_x==238&&p.label_y==20&&p.label_alignment==2&&p.label_style==(paused?1:2)&&p.label_scale==256);
 }
 for(int count=-32768;count<=32767;count++){
  assert(!taxi_challenge_ball_hud_plan(&p,0,(int16_t)count,240,160,0,0,positions,60));unsigned start=count/10?1:0;assert(p.counter_count==start+4);
  if(start)assert(p.counter[0].glyph==(uint8_t)(count/10+16));assert(p.counter[start].glyph==(uint8_t)(count%10+16));
 }
 assert(!taxi_challenge_ball_hud_plan(&p,1,20,320,180,32,1,positions,60));assert(p.counter[0].x==240&&p.label_x==318&&p.clock.indicator_y==157);
 TaxiBallHudPlan before=p;assert(taxi_challenge_ball_hud_plan(&p,0,0,240,160,0,0,positions,59)==-1&&!memcmp(&p,&before,sizeof p));
 TaxiFontAsset font={0};assert(!taxi_font_asset_load(&font,"outputs/crazy-taxi-native/assets/challenge-flag/flag-clock-metrics.ctf","outputs/crazy-taxi-native/assets/hud/clock.rgba",2,16,0));
 uint8_t clock_pixels[153600],ball_pixels[153600];
 for(int count=0;count<=20;count++){
  assert(!taxi_challenge_ball_hud_plan(&p,2700,(int16_t)count,240,160,0,0,positions,60));
  assert(!taxi_challenge_flag_hud_pixels(clock_pixels,&p.clock,&font)&&!taxi_challenge_ball_hud_pixels(ball_pixels,&p,&font));
  unsigned added=0;for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){
   size_t offset=((size_t)y*240+x)*4;
   if(memcmp(clock_pixels+offset,ball_pixels+offset,4)){assert(x>=160&&y<20&&ball_pixels[offset+3]==255);added++;}
   else if(clock_pixels[offset+3])assert(!memcmp(clock_pixels+offset,ball_pixels+offset,4));
  }assert(added);
 }
 TaxiFontAsset label_font={0};assert(!taxi_font_asset_load(&label_font,"outputs/crazy-taxi-native/assets/loading/font0-metrics.ctf","outputs/crazy-taxi-native/assets/loading/font0.rgba",4,32,1));
 for(unsigned paused=0;paused<2;paused++){
  assert(!taxi_challenge_ball_hud_plan(&p,2700,20,240,160,0,paused,positions,60));assert(!taxi_challenge_ball_hud_pixels(ball_pixels,&p,&font));memcpy(clock_pixels,ball_pixels,sizeof ball_pixels);
  assert(!taxi_challenge_ball_label_pixels(ball_pixels,&p,&label_font,"Balloons"));unsigned added=0;
  for(unsigned y=0;y<160;y++)for(unsigned x=0;x<240;x++){size_t off=((size_t)y*240+x)*4;if(clock_pixels[off+3])assert(!memcmp(clock_pixels+off,ball_pixels+off,4));else if(ball_pixels[off+3]){assert(x<238&&x>100&&y>=15&&y<40);added++;}}
  assert(added);printf("Source Balloons label style%u added%u original font pixels; preceding OBJ priority retained\n",p.label_style,added);
 }
 taxi_font_asset_free(&label_font);
 taxi_font_asset_free(&font);puts("PASS21 original Ball counter/clock native pixel compositions: exact sourcefont/group3, timer pixels retained, counter bounded in original right HUD area. Label/indicator/GPU pending.");
 puts("PASS source8430 Ball HUD:42 normal count/pause layouts,65536 signed counter values, original shared clock/gear/positions/right-edge label request, caller viewport and invalid atomicity. Pixel composition/live HUD pending.");return 0;
}
