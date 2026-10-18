#include "game_audio_controller.h"
#include <string.h>
int taxi_game_audio_controller_step(TaxiGameAudioController *state,const TaxiGameAudioControllerInput *i,TaxiGameAudioEngineRequest out[32],unsigned *count){
 if(!state||!i||!out||!count)return -1;
 TaxiGameAudioController s=*state;TaxiGameAudioEngineRequest q[32];unsigned n=0;
 TaxiGameAudioLevelsInput li={i->speed,i->max_speed,i->scale,s.cooldown,i->delta,i->rpm,s.factor,i->muted,i->ended,i->outer};TaxiGameAudioLevels l;
 if(taxi_game_audio_levels(&li,&l))return -1;
 s.cooldown=l.cooldown;s.factor=l.factor;
 TaxiGameAudioEngine engine={{s.handles[0],s.handles[1]}};
 n+=taxi_game_audio_engine_step(&engine,l.low,l.high,s.amplitude,l.pitch,i->starts,q+n);
 s.handles[0]=engine.handle[0];s.handles[1]=engine.handle[1];
 TaxiGameAudioEvents ev={s.flag0,s.flag1,s.hold_timer};TaxiGameAudioEventsInput ei={l.speed_class,i->delta,i->effects,i->action2,i->intro,i->ended,i->frozen};TaxiGameAudioEventRequests er;
 taxi_game_audio_events_step(&ev,&ei,&er);s.flag0=ev.flag0;s.flag1=ev.flag1;s.hold_timer=ev.hold_timer;
 for(unsigned j=0;j<er.count;j++)q[n++]=(TaxiGameAudioEngineRequest){9,er.sounds[j],0};
 if(er.query_action2)q[n++]=(TaxiGameAudioEngineRequest){5,0,2};
 TaxiGameAudioAux aux={s.handles[2],s.handles[3],s.pending2};TaxiGameAudioAuxInput ai={l.speed_class,i->action3,i->override2620,i->intro,i->ended,i->frozen,i->busy[3],i->starts[2],i->starts[3]};
 n+=taxi_game_audio_aux_step(&aux,&ai,q+n);s.handles[2]=aux.loop;s.handles[3]=aux.one_shot;s.pending2=aux.pending;
 TaxiGameAudioVehicleLoopInput vi={i->vehicle564,i->flags28,i->frozen,i->ended,i->fade,i->busy[4],i->starts[4]};
 n+=taxi_game_audio_vehicle_loop(&s.handles[4],&vi,q+n);
 TaxiGameAudioRewardEvent reward={s.handles[5],s.pending3,s.reward_cooldown};TaxiGameAudioRewardEventInput ri={i->speed,i->gate00e4,i->starts[5]};
 n+=taxi_game_audio_reward_event(&reward,&ri,q+n);s.handles[5]=reward.handle;s.pending3=reward.pending;s.reward_cooldown=reward.cooldown;
 TaxiGameAudioTurnLoop turn={s.handles[5],s.handles[6]};TaxiGameAudioTurnLoopInput ti={i->angle578,s.enabled4,i->flags28,i->frozen,i->ended,i->fade,i->busy[5],i->busy[6],i->starts[6]};
 n+=taxi_game_audio_turn_loop(&turn,&ti,q+n);s.handles[5]=turn.event_handle;s.handles[6]=turn.loop_handle;
 TaxiGameAudioVoice voice={s.handles[8],s.handles[7],s.driver_selection};TaxiGameAudioVoiceInput v={i->settings,i->challenge_driver,i->intro,i->ended,i->frozen,i->busy[8],i->driver,i->starts[8]};
 n+=taxi_game_audio_voice_step(&voice,&v,q+n);s.handles[8]=voice.handle;s.driver_selection=voice.selection;
 TaxiGameAudioContextVoice context={s.handles[7],s.handles[8],s.context_selection,s.classification,s.driver_selection,s.last};TaxiGameAudioContextVoiceInput ci={i->intro,i->ended,i->frozen,i->busy[7],i->current_class,i->alternate_class,i->starts[7],i->random};
 n+=taxi_game_audio_context_voice_step(&context,&ci,q+n);s.handles[7]=context.handle;s.context_selection=context.selection;s.classification=context.classification;s.driver_selection=context.followup;s.last=context.last;
 s.amplitude=taxi_game_audio_amplitude_step(s.amplitude,i->delta);
 *state=s;memcpy(out,q,n*sizeof(*q));*count=n;return 0;
}

int taxi_game_audio_controller_dispatch_shared(TaxiGameAudioController *state,const TaxiGameAudioControllerInput *i,TaxiGameAudioDispatch call,void *dispatch_context,TaxiGameAudioSelectionRead selection_read,unsigned *count){
 if(!state||!i||!call||!count)return -1;
 TaxiGameAudioController s=*state;unsigned n=0;
 TaxiGameAudioLevelsInput li={i->speed,i->max_speed,i->scale,s.cooldown,i->delta,i->rpm,s.factor,i->muted,i->ended,i->outer};TaxiGameAudioLevels l;
 if(taxi_game_audio_levels(&li,&l))return -1;
 s.cooldown=l.cooldown;s.factor=l.factor;
 TaxiGameAudioEngine engine={{s.handles[0],s.handles[1]}};
 n+=taxi_game_audio_engine_dispatch(&engine,l.low,l.high,s.amplitude,l.pitch,call,dispatch_context);
 s.handles[0]=engine.handle[0];s.handles[1]=engine.handle[1];
 TaxiGameAudioEvents ev={s.flag0,s.flag1,s.hold_timer};TaxiGameAudioEventsInput ei={l.speed_class,i->delta,i->effects,i->action2,i->intro,i->ended,i->frozen};unsigned events_count=taxi_game_audio_events_dispatch(&ev,&ei,call,dispatch_context);
 n+=events_count;s.flag0=ev.flag0;s.flag1=ev.flag1;s.hold_timer=ev.hold_timer;
 TaxiGameAudioAux aux={s.handles[2],s.handles[3],s.pending2};TaxiGameAudioAuxInput ai={l.speed_class,i->action3,i->override2620,i->intro,i->ended,i->frozen,i->busy[3],i->starts[2],i->starts[3]};
 n+=taxi_game_audio_aux_dispatch(&aux,&ai,call,dispatch_context);s.handles[2]=aux.loop;s.handles[3]=aux.one_shot;s.pending2=aux.pending;
 TaxiGameAudioVehicleLoopInput vi={i->vehicle564,i->flags28,i->frozen,i->ended,i->fade,i->busy[4],i->starts[4]};
 n+=taxi_game_audio_vehicle_loop_dispatch(&s.handles[4],&vi,call,dispatch_context);
 TaxiGameAudioRewardEvent reward={s.handles[5],s.pending3,s.reward_cooldown};TaxiGameAudioRewardEventInput ri={i->speed,i->gate00e4,i->starts[5]};
 int reward_changes_selection=s.pending3&&!i->gate00e4&&!s.reward_cooldown;
 n+=taxi_game_audio_reward_event_dispatch(&reward,&ri,call,dispatch_context);s.handles[5]=reward.handle;s.pending3=reward.pending;s.reward_cooldown=reward.cooldown;
 if(reward_changes_selection&&selection_read)s.context_selection=selection_read(dispatch_context);
 TaxiGameAudioTurnLoop turn={s.handles[5],s.handles[6]};TaxiGameAudioTurnLoopInput ti={i->angle578,s.enabled4,i->flags28,i->frozen,i->ended,i->fade,i->busy[5],i->busy[6],i->starts[6]};
 n+=taxi_game_audio_turn_loop_dispatch(&turn,&ti,call,dispatch_context);s.handles[5]=turn.event_handle;s.handles[6]=turn.loop_handle;
 TaxiGameAudioVoice voice={s.handles[8],s.handles[7],s.driver_selection};TaxiGameAudioVoiceInput v={i->settings,i->challenge_driver,i->intro,i->ended,i->frozen,i->busy[8],i->driver,i->starts[8]};
 n+=taxi_game_audio_voice_dispatch(&voice,&v,call,dispatch_context);s.handles[8]=voice.handle;s.driver_selection=voice.selection;
 TaxiGameAudioContextVoice context={s.handles[7],s.handles[8],s.context_selection,s.classification,s.driver_selection,s.last};TaxiGameAudioContextVoiceInput ci={i->intro,i->ended,i->frozen,i->busy[7],i->current_class,i->alternate_class,i->starts[7],i->random};
 n+=taxi_game_audio_context_voice_dispatch(&context,&ci,call,dispatch_context);s.handles[7]=context.handle;s.context_selection=context.selection;s.classification=context.classification;s.driver_selection=context.followup;s.last=context.last;
 s.amplitude=taxi_game_audio_amplitude_step(s.amplitude,i->delta);
 *state=s;*count=n;return 0;
}

int taxi_game_audio_controller_dispatch(TaxiGameAudioController *state,const TaxiGameAudioControllerInput *i,TaxiGameAudioDispatch call,void *context,unsigned *count){
 return taxi_game_audio_controller_dispatch_shared(state,i,call,context,0,count);
}
