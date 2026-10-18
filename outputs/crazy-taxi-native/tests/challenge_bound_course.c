#include "../src/challenge_bound_course.h"
#include "../src/runtime_asset.h"
#include <stdio.h>
int main(void){static TaxiChallengeBoundCourse course;static TaxiRuntimeAsset runtime;TaxiDriveWorld car={0};TaxiDriveWorldContext ctx={0};TaxiChallengeBound timer={0};uint8_t option=0;TaxiPassengerState actor={0};
 if(taxi_challenge_bound_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))return 1;
 if(taxi_challenge_bound_course_start(&course,&car,&ctx,&timer,&option,runtime.templates[0],584,128)!=1||taxi_challenge_bound_course_actor(&course,&actor))return 2;
 if(timer.remaining_1660!=2400||option!=4||ctx.driver.motion.orientation.submode!=5||actor.bytes[0]!=1)return 3;
 TaxiChallengeBoundStatus status={0};TaxiChallengeBoundEffects effects;TaxiChallengeBoundInput in={.delta=1,.intro1696=1,.vehicle_y2638=car.world.height.position.y};
 if(taxi_challenge_bound_course_control(&course,&ctx,&timer,in,&status,&effects)||status.ended1697||timer.remaining_1660!=2400)return 4;
 in.intro1696=0;if(taxi_challenge_bound_course_control(&course,&ctx,&timer,in,&status,&effects)||timer.remaining_1660!=2399||status.ended1697)return 5;
 printf("PASS Bound807/737 original course/template0 startup position%d,%d,%d, source actor flag1, submode5,2400timer and intro gate. Prior actorY0 explicit fixture; not playable/GBA oracle.\n",car.world.height.position.x,car.world.height.position.y,car.world.height.position.z);taxi_challenge_bound_course_free(&course);return 0;}
