#include "challenge_drift_hud.h"
int taxi_challenge_drift_hud_plan(TaxiBallHudPlan *out,int16_t remaining,int16_t count,uint16_t width,uint16_t height,uint8_t flags,int paused,const uint8_t positions[60],size_t size){
 if(!out)return -1;
 TaxiBallHudPlan p;
 if(taxi_challenge_ball_hud_plan(&p,remaining,count,width,height,flags,paused,positions,size))return -1;
 p.counter[p.counter_count-2].glyph=17;
 p.counter[p.counter_count-1].glyph=21;
 p.label_id=616;*out=p;return 0;
}
