/* Native half: isolated motion_prepare with explicit no-drift orientation fixture. */
#include "../src/motion.h"
#include <stdio.h>
#include <stdint.h>
static TaxiTrig trig;
int main(void){uint8_t b[16388];FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fread(b,1,sizeof b,f)!=sizeof b)return 1;fclose(f);taxi_trig_decode(&trig,b+4,b+8196);
int speed,steer,pitch,heading,roll,delta,drift,timer,state,flags;
while(scanf("%d%d%d%d%d%d%d%d%d%d",&speed,&steer,&pitch,&heading,&roll,&delta,&drift,&timer,&state,&flags)==10){
TaxiMotionState m={.orientation={.speed_544=speed,.angle_18=(uint16_t)pitch,.angle_1a=(uint16_t)roll,.heading_1c=heading*4096,.heading_20=heading*4096,.force_540=12345,.flags_358=4,.phase_e6a=2,.flags_28=(uint8_t)flags,.drift_69c=drift,.timer_2a=(int16_t)timer,.state_564=state,.position={100,200,300}},.steering={.angle_578=(uint16_t)steer,.limit_7b0=100000},.scale_7c8=4096,.scale_e84=5324,.dimension_594=2000};
TaxiCommand cmd={0};cmd.bytes[4]=4;TaxiOrientationContext c={.delta=delta,.settings=128,.submode=5};TaxiOrientationEffects e;
TaxiSteeringState st=m.steering;st.speed_544=speed;st.state_564=state;st.flags_358=4;st.phase_e6a=2;st.flags_28=(uint8_t)flags;int32_t pre; if(taxi_steering_response(&st,&cmd,128,&trig,&pre))return 2;
int rc=taxi_motion_prepare(&m,&c,&cmd,&trig,&e);printf("%d %d %u %d %d %d %d %d %d %d %d %d %d %d %d\n",rc,pre,st.flags_28,m.response_57c,m.orientation.heading_1c,m.orientation.heading_20,m.displacement.x,m.displacement.y,m.displacement.z,m.orientation.force_540,m.orientation.drift_69c,m.orientation.timer_2a,m.orientation.speed_544,m.orientation.position.x,m.orientation.position.z);
}return ferror(stdin)||ferror(stdout);}
