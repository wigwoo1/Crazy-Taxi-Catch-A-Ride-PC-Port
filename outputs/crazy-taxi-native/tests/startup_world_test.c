#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "../src/drive_world.h"
static TaxiTrig trig;
static TaxiNormalAngles angles;
static int obstacle(void *ctx,TaxiPoint p,TaxiObstacleHit *hit){(void)ctx;(void)p;(void)hit;return 0;}
static int ground(void *ctx,TaxiPoint p,TaxiGroundHit *hit){(void)p;(void)hit;return *(int*)ctx;}
int main(void){
 for(unsigned i=0;i<4096;i++)trig.cosine[i]=256;
 TaxiDriveWorld s={0};TaxiDriveWorldContext c={0};TaxiDriveWorldEffects e={0};
 s.world.height.position=(TaxiPoint){10,100,20};s.world.height.scale_e84=4096;
 s.world.displacement_x=12;s.world.displacement_z=-7;
 s.world.impulse_74c=40960;s.world.impulse_34=60000;
 s.driver.power.gears.speed_544=4096;s.driver.power.gears.flags_28=8;
 s.driver.power.gears.steering_578=123;s.driver.steering_rate_57a=17;
 TaxiGearInput in={.delta=1,.frame_delta=1,.freeze_177b=1,.action2=1};int result=0;
 assert(!taxi_drive_world_start_step(&s,&c,&in,0,&trig,&angles,obstacle,ground,&result,&e));
 assert(s.world.height.position.x==32&&s.world.height.position.z==13);
 assert(s.world.displacement_x==12&&s.world.displacement_z==-7);
 assert(s.world.heading_20==1000&&s.world.impulse_74c==35840);
 assert(s.driver.power.gears.steering_578==123&&s.driver.steering_rate_57a==17);
 assert(s.driver.power.gears.speed_544==s.world.speed_544&&s.world.speed_544>0&&s.world.speed_544<4096);
 assert(s.driver.motion.motion.orientation.position.x==32);
 TaxiDriveWorld old=s;TaxiDriveWorldContext oldc=c;TaxiDriveWorldEffects olde=e;
 result=-1;
 assert(taxi_drive_world_start_step(&s,&c,&in,0,&trig,&angles,obstacle,ground,&result,&e)==-1);
 assert(!memcmp(&s,&old,sizeof s)&&!memcmp(&c,&oldc,sizeof c)&&!memcmp(&e,&olde,sizeof e));
 puts("PASS startup residual world movement, gear gate, no steering, shared state and rollback.");
 return 0;
}

