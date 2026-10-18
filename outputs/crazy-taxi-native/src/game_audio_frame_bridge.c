#include "game_audio_frame_bridge.h"
static int8_t byte_signed(uint8_t v){return (int8_t)(v<128?v:(int)v-256);}
int taxi_game_audio_frame_import(TaxiGameAudioController *state,TaxiGameAudioControllerInput *input,const TaxiDriveWorld *car,const TaxiDriveWorldContext *context,const TaxiPassengerScene *passengers,const TaxiTrafficGlobals *traffic,uint8_t timer){
 if(!state||!input||!car||!context||!passengers||!traffic)return -1;
 TaxiGameAudioController s=*state;TaxiGameAudioControllerInput i=*input;
 if(taxi_game_audio_passenger_classes(passengers,&i.current_class,&i.alternate_class))return -1;
 i.speed=car->world.speed_544;i.max_speed=car->speed_divisor_548;i.scale=car->world.height.scale_e84;
 i.rpm=car->driver.power.gears.rpm_538;i.angle578=car->driver.power.gears.steering_578;
 i.vehicle564=car->driver.motion.motion.orientation.state_564;i.flags28=car->driver.power.gears.flags_28;
 i.override2620=context->driver.freeze_2620;i.gate00e4=traffic->cooldown_e4;
 i.muted=passengers->session.audio_muted_1534;
 i.ended=passengers->session.ended_1697;i.outer=passengers->session.outer_state_1ea8;
 s.amplitude=context->driver.power.state_25dc;s.pending2=context->driver.power.state_25d2;
 s.pending3=traffic->near_miss_25d3;s.enabled4=context->driver.gear.state_25d4;s.reward_cooldown=timer;
 s.context_selection=byte_signed(passengers->session.trip.voice_25d6);
 s.driver_selection=byte_signed(passengers->pickup.animation_25d7);
 *state=s;*input=i;return 0;
}
void taxi_game_audio_frame_commit(const TaxiGameAudioController *s,TaxiDriveWorldContext *context,TaxiPassengerScene *passengers,TaxiTrafficGlobals *traffic,uint8_t *timer){
 context->driver.power.state_25dc=s->amplitude;context->driver.power.state_25d2=s->pending2;
 context->driver.gear.state_25d4=s->enabled4;traffic->near_miss_25d3=s->pending3;*timer=s->reward_cooldown;
 passengers->session.trip.voice_25d6=passengers->pickup.voice_25d6=traffic->voice_25d6=(uint8_t)s->context_selection;
 passengers->pickup.animation_25d7=(uint8_t)s->driver_selection;
}
