#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "challenge_drift_hud.h"
int main(void){
 uint8_t positions[60];FILE *f=fopen("outputs/crazy-taxi-native/assets/challenge-flag/flag-hud-positions.bin","rb");assert(f);assert(fread(positions,1,60,f)==60&&fgetc(f)==EOF);fclose(f);
 TaxiBallHudPlan p;
 for(int count=-32768;count<=32767;count++){
  assert(!taxi_challenge_drift_hud_plan(&p,1800,(int16_t)count,240,160,32,1,positions,60));
  unsigned slot=count/10?1:0;assert(p.counter_count==slot+4);
  if(slot)assert(p.counter[0].glyph==(uint8_t)(count/10+16)&&p.counter[0].x==160);
  assert(p.counter[slot].glyph==(uint8_t)(count%10+16)&&p.counter[slot].x==176);
  assert(p.counter[slot+1].glyph==15&&p.counter[slot+2].glyph==17&&p.counter[slot+3].glyph==21);
  assert(p.counter[slot+2].x==208&&p.counter[slot+3].x==224&&p.label_id==616&&p.label_x==238&&p.label_y==20);
  assert(!strcmp(p.clock.time,"00'30\"00")&&p.clock.indicator_variant==2&&p.clock.indicator_y==137&&p.label_style==1);
 }
 TaxiFontAsset font={0},label={0};assert(!taxi_font_asset_load(&font,"outputs/crazy-taxi-native/assets/challenge-flag/flag-clock-metrics.ctf","outputs/crazy-taxi-native/assets/hud/clock.rgba",2,16,0));
 assert(!taxi_font_asset_load(&label,"outputs/crazy-taxi-native/assets/loading/font0-metrics.ctf","outputs/crazy-taxi-native/assets/loading/font0.rgba",4,32,1));
 uint8_t pixels[153600],before[153600];
 for(int count=0;count<=15;count++)for(int paused=0;paused<2;paused++){
  assert(!taxi_challenge_drift_hud_plan(&p,599,(int16_t)count,240,160,0,paused,positions,60));
  assert(!taxi_challenge_ball_hud_pixels(pixels,&p,&font));memcpy(before,pixels,sizeof pixels);
  assert(!taxi_challenge_ball_label_pixels(pixels,&p,&label,"Combos"));unsigned added=0;
  for(unsigned i=0;i<38400;i++){if(before[i*4+3])assert(!memcmp(before+i*4,pixels+i*4,4));else if(pixels[i*4+3])added++;}assert(added);
 }
 TaxiBallHudPlan retained=p;assert(taxi_challenge_drift_hud_plan(&p,0,0,240,160,0,0,positions,59)==-1&&!memcmp(&p,&retained,sizeof p));
 assert(!taxi_challenge_drift_hud_plan(&p,0,15,320,180,0,0,positions,60)&&p.label_x==318&&p.counter[0].x==240&&p.clock.indicator_y==157&&p.label_style==2);
 taxi_font_asset_free(&font);taxi_font_asset_free(&label);
 puts("PASS Drift HUD:65536 signed count layouts;32 original font counter/Combos compositions preserve preceding pixels; shared clock/gear/viewport and invalid atomicity. No independent GBA screen oracle.");return 0;
}
