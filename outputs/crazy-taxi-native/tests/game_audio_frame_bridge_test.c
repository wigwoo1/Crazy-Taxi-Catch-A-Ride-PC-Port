#ifdef NDEBUG
#undef NDEBUG
#endif
#include "game_audio_frame_bridge.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
int main(void){
 TaxiDriveWorld car={0};TaxiDriveWorldContext context={0};TaxiPassengerScene passengers={0};TaxiTrafficGlobals traffic={0};
 TaxiGameAudioController s={0};TaxiGameAudioControllerInput i={0};uint8_t timer=0;
 passengers.session.audio_muted_1534=1;passengers.count=2;passengers.session.trip.active_customer=1;passengers.pickup.dialogue_index=0;
 passengers.customers[0].bytes[1]=0;passengers.customers[1].bytes[1]=1;
 car.world.speed_544=-1138;car.speed_divisor_548=184572;car.world.height.scale_e84=4096;
 car.driver.power.gears.rpm_538=2136;car.driver.power.gears.steering_578=-1536;car.driver.power.gears.flags_28=8;
 car.driver.motion.motion.orientation.state_564=17;context.driver.freeze_2620=1;
 context.driver.power.state_25dc=8128;context.driver.power.state_25d2=1;context.driver.gear.state_25d4=1;
 traffic.cooldown_e4=60;traffic.near_miss_25d3=1;passengers.session.trip.voice_25d6=255;passengers.pickup.animation_25d7=17;
 s.handles[9]=37;s.factor=256;s.flag0=1;i.delta=3;i.muted=1;i.settings=128;i.driver=2;
 assert(!taxi_game_audio_frame_import(&s,&i,&car,&context,&passengers,&traffic,19));
 assert(i.speed==-1138&&i.max_speed==184572&&i.scale==4096&&i.rpm==2136&&i.angle578==-1536&&i.flags28==8&&i.vehicle564==17&&i.override2620==1&&i.gate00e4==60);
 assert(i.current_class==0&&i.alternate_class==1&&i.delta==3&&i.muted==1&&i.settings==128&&i.driver==2);
 assert(s.handles[9]==37&&s.factor==256&&s.flag0==1&&s.amplitude==8128&&s.pending2==1&&s.pending3==1&&s.enabled4==1&&s.reward_cooldown==19&&s.context_selection==-1&&s.driver_selection==17);
 for(unsigned v=0;v<256;v++){passengers.session.audio_muted_1534=(uint8_t)v;assert(!taxi_game_audio_frame_import(&s,&i,&car,&context,&passengers,&traffic,19)&&i.muted==v);}
 TaxiGameAudioController before=s;TaxiGameAudioControllerInput prior=i;passengers.pickup.dialogue_index=128;
 assert(taxi_game_audio_frame_import(&s,&i,&car,&context,&passengers,&traffic,0)==-1&&!memcmp(&s,&before,sizeof s)&&!memcmp(&i,&prior,sizeof i));
 s.amplitude=16384;s.pending2=0;s.pending3=0;s.driver_selection=-1;s.context_selection=21;s.reward_cooldown=20;
 taxi_game_audio_frame_commit(&s,&context,&passengers,&traffic,&timer);
 assert(context.driver.power.state_25dc==16384&&!context.driver.power.state_25d2&&!traffic.near_miss_25d3&&timer==20);
 assert(passengers.session.trip.voice_25d6==21&&passengers.pickup.voice_25d6==21&&traffic.voice_25d6==21&&passengers.pickup.animation_25d7==255);
 puts("PASS shared frame import/commit, independent passenger owners, retained external gates and atomic invalid-reference rejection");return 0;
}
