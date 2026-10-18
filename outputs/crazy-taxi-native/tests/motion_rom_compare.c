/* Native half: isolated motion_prepare with explicit no-drift orientation fixture. */
#include "../src/motion.h"
#include <stdio.h>
#include <stdint.h>
static TaxiTrig trig;
int main(void){uint8_t b[16388];FILE*f=fopen("outputs/crazy-taxi-native/assets/driving.ctr","rb");if(!f||fread(b,1,sizeof b,f)!=sizeof b)return 1;fclose(f);taxi_trig_decode(&trig,b+4,b+8196);
int speed,steer,pitch,heading,roll,delta;
while(scanf("%d%d%d%d%d%d",&speed,&steer,&pitch,&heading,&roll,&delta)==6){
TaxiMotionState m={.orientation={.speed_544=speed,.angle_18=(uint16_t)pitch,.angle_1a=(uint16_t)roll,.heading_1c=heading*4096,.heading_20=heading*4096,.force_540=12345},.steering={.angle_578=(uint16_t)steer,.limit_7b0=100000},.scale_7c8=4096,.scale_e84=5324,.dimension_594=2000};
TaxiCommand cmd={0};cmd.bytes[4]=4;TaxiOrientationContext c={.delta=delta,.settings=128,.submode=5};TaxiOrientationEffects e;
TaxiSteeringState st=m.steering;st.speed_544=speed;int32_t pre; if(taxi_steering_response(&st,&cmd,128,&trig,&pre))return 2;
int rc=taxi_motion_prepare(&m,&c,&cmd,&trig,&e);printf("%d %d %d %d %d %d %d %d %d\n",rc,pre,m.response_57c,m.orientation.heading_1c,m.orientation.heading_20,m.displacement.x,m.displacement.y,m.displacement.z,m.orientation.force_540);
}return ferror(stdin)||ferror(stdout);}
