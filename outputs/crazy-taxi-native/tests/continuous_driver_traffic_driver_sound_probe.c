#include "../src/reference_driving.h"
#include "../src/game_audio_frame_bridge.h"
#include "audio_owner_snapshot.h"
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include "../src/traffic_scene.h"
#include "../src/traffic_grid.h"
#include "../src/traffic_init.h"
#include "../src/passenger_idle.h"
#include "../src/passenger_search.h"
#include "../src/random.h"
#include "../src/game_audio_context_voice.h"
#include "../src/game_audio_channels.h"
#include "../src/passenger_asset.h"
#include "../src/game_audio_voice.h"
#include "../src/trip_clock.h"
#include "../src/fare.h"
#include "../src/impact_voice.h"
#include "../src/actions.h"
#include "../src/game_audio_reward_event.h"
#include "../src/reward_update.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    unsigned setting,count,timer,counter,near,voice,combo_flag,sound,active;
    int cooldown,combo_whole,combo_fraction;
    uint32_t rng;
} Config;
typedef struct {
    uint64_t source_audio_bank_clocks[236];
    TaxiGameAudioController full_audio;
    TaxiReferenceDriving *current_driving;
    uint8_t audio_muted,audio_outer,audio_effects;
    unsigned audio_requests;
    TaxiGameAudioRewardEvent reward_event;
    TaxiRewardState rewards;
    TaxiRewardClock reward_clock;
    TaxiCashGlyphs cash_glyphs;
    unsigned diagnostic_step;
    TaxiDistanceDisplay distance_display;
    TaxiMainClock main_clock;
    TaxiTripStartState trip;
    TaxiTripEndState trip_end;
    TaxiFareState fare;
    uint8_t settings_0,settings_3;
    TaxiGameAudioVoiceInput driver_voice_input;
    TaxiPassengerAsset passenger_assets;
    uint8_t destinations[128];
    int32_t passenger_gate_1700;
    TaxiAudioAsset audio_assets;
    TaxiGameAudioChannels audio_channels;
    TaxiGameAudioContextVoice context_voice;
    uint64_t audio_tick;
    TaxiTrafficActor actors[50];
    TaxiTrafficScene scene;
    TaxiTrafficGlobals globals;
    TaxiPassengerState passengers[128];
    TaxiRoutePoint passenger_spawns[128];
    int32_t passenger_candidate,passenger_distance;
    TaxiPickupState pickup;
    uint8_t freeze;
    uint32_t voice_gate_2604,voice_gate_2608;
    int32_t voice_timer_2614,voice_owner_index;
    uint8_t timer;
    uint32_t rng,counter;
    unsigned count;
    int sound,active;
    TaxiRuntimeAsset runtime;
    TaxiWorldAsset world;
    TaxiTrafficGrid grid;
    TaxiAiPathAsset roads[256];
    TaxiTrafficSubmission submissions[50];
    uint32_t scales[4];
} Replay;

static void read_exact(const char *path,void *dst,size_t bytes) {
    FILE *f=fopen(path,"rb");assert(f);
    assert(fread(dst,1,bytes,f)==bytes);assert(fgetc(f)==EOF);assert(fclose(f)==0);
}
static Config read_config(const char *path) {
    Config c={0};FILE *f=fopen(path,"r");assert(f);
    assert(fscanf(f,"%u %u %u %u %d %u %u %d %d %d %u %u %u",
        &c.setting,&c.count,&c.timer,&c.counter,&c.cooldown,&c.near,&c.voice,
        &c.combo_flag,&c.combo_whole,&c.combo_fraction,&c.rng,&c.sound,&c.active)==13);
    assert(fclose(f)==0);return c;
}
static void load_actor(const int32_t *v,TaxiTrafficActor *a) {
    memset(a,0,sizeof(*a));a->flags=(uint32_t)v[0];a->type=(uint32_t)v[1];
    a->countdown=v[2];a->x=v[3];a->y=v[4];a->z=v[5];a->heading=v[6];
    a->route=(uint32_t)v[7];a->path.distance=v[8];a->path.segment=(size_t)(uint32_t)v[9];
    a->path.x=v[10];a->path.z=v[11];a->path.heading=(uint16_t)v[12];
    a->impulse_x=v[13];a->impulse_z=v[14];a->offset_x=v[15];a->offset_z=v[16];
    a->collision_timer=v[17];
}
/* Source context followups request the same shared1E72C RNG owner. */
static uint32_t replay_audio_dispatch(void *opaque,TaxiGameAudioEngineRequest request){
    Replay *r=opaque;
    if(request.op==5){
        assert(r->current_driving&&request.handle==0);
        TaxiActionContext action={.profile=(r->settings_0>>1)&3,
            .vehicle_flags=r->current_driving->car.driver.power.gears.flags_4ac};
        return (uint32_t)taxi_action_query(&r->current_driving->input,action,request.value);
    }
    if(request.op==7){
        r->rewards.combo_9aa6=r->globals.combo_flag_9aa6;
        r->rewards.whole_9aa8=r->globals.combo_9aa8;
        r->rewards.fraction_9aac=r->globals.combo_9aac;
        r->rewards.voice_25d6=r->globals.voice_25d6;
        r->rewards.random_state=r->rng;
        TaxiRewardInput input={.kind=1,.active_1694=r->trip.active_1694,
            .settings_0=r->settings_0,.sound_enabled=(uint8_t)r->sound,
            .whole=(int32_t)request.handle,.fraction=request.value};
        TaxiRewardEffects effect;
        if(taxi_reward(&r->rewards,&input,&effect)){
            r->audio_channels.error=-20;return 0;
        }
        r->rng=r->rewards.random_state;
        r->globals.combo_flag_9aa6=r->rewards.combo_9aa6;
        r->globals.combo_9aa8=r->rewards.whole_9aa8;
        r->globals.combo_9aac=r->rewards.fraction_9aac;
        r->globals.voice_25d6=r->rewards.voice_25d6;
        if(effect.sound_table_index>=0)
            taxi_game_audio_channel_dispatch(&r->audio_channels,
                (TaxiGameAudioEngineRequest){9,(uint32_t)effect.sound_table_index,0});
        printf("R %u %d %d %u %u %d %d %d\n",r->diagnostic_step,
            input.whole,input.fraction,r->rng,r->globals.voice_25d6,
            r->globals.combo_9aa8,r->globals.combo_9aac,effect.slot);
        return 0;
    }
    if(request.op==8){
        assert(request.handle==0&&request.value==65535);
        return (uint32_t)taxi_random_range_tagged(&r->rng,0,65535,"context-followup");
    }
    return taxi_game_audio_channel_dispatch(&r->audio_channels,request);
}
/* This is an offline full-controller diagnostic. Physical bank timing is
   uses validated source observation ticks plus one completed core frame; the
   logical driver clock/deltas are separate. This is not an autonomous scheduler. */
/* Direct source267C8 gear25/43 and brake24, never deferred25D0/1. */
static int replay_driver_sound(void *opaque,unsigned phase,unsigned index){
    Replay *r=opaque;
    assert(r->current_driving&&r->diagnostic_step==r->current_driving->next);
    assert(phase<=2&&index<203);
    if(phase==0)assert(index==25||index==43);
    if(phase==2)assert(index==24);
    uint32_t handle=taxi_game_audio_channel_dispatch(&r->audio_channels,
        (TaxiGameAudioEngineRequest){9,index,0});
    printf("L %u %llu %u %u %u\n",r->diagnostic_step,
        (unsigned long long)r->audio_tick,phase,index,handle);
    return r->audio_channels.error?-1:0;
}
static int8_t replay_shared_selection(void *opaque){
    Replay *r=opaque;return (int8_t)r->globals.voice_25d6;
}
static int full_audio_before_driver(void *opaque,TaxiReferenceDriving *f,
    const TaxiReferenceDrivingStep *step,const TaxiTrig *trig){
    Replay *r=opaque;(void)trig;r->current_driving=f;r->diagnostic_step=f->next;
    assert(f->next<236);
    uint64_t bank_clock=r->source_audio_bank_clocks[f->next];
    assert(bank_clock>=r->audio_tick);
    while(r->audio_tick<bank_clock){
        int16_t pcm[352]={0};
        for(unsigned channel=0;channel<4;channel++){
            TaxiAudioChannel *c=r->audio_channels.channels+channel;
            if(taxi_audio_channel_mix(c,pcm,352,r->audio_assets.codebook,608,
                taxi_audio_effective_volume(64,c->volume))<0)return -1;
        }
        r->audio_tick++;
    }
    TaxiPassengerScene passengers={0};passengers.count=128;
    memcpy(passengers.customers,r->passengers,sizeof r->passengers);
    passengers.session.trip=r->trip;passengers.session.trip.voice_25d6=r->globals.voice_25d6;
    passengers.session.audio_muted_1534=r->audio_muted;
    passengers.session.outer_state_1ea8=r->audio_outer;
    passengers.session.ended_1697=step->flag_1697;
    passengers.pickup=r->pickup;passengers.pickup.dialogue_index=r->voice_owner_index;
    TaxiGameAudioControllerInput input={.delta=(int32_t)step->delta,
        .intro=f->intro,.frozen=r->freeze,.fade=step->freeze_177b,
        .effects=r->audio_effects,.settings=r->driver_voice_input.settings,
        .challenge_driver=r->driver_voice_input.challenge_driver,
        .driver=r->driver_voice_input.driver};
    if(taxi_game_audio_frame_import(&r->full_audio,&input,&f->car,&f->context,
        &passengers,&r->globals,r->timer))return -1;
    /* Passenger search and audio share original2614; import the carried
       native writer, never a source expected post-state. */
    r->full_audio.cooldown=r->voice_timer_2614;
    unsigned requests=0;
    if(taxi_game_audio_controller_dispatch_shared(&r->full_audio,&input,
        replay_audio_dispatch,r,replay_shared_selection,&requests)||r->audio_channels.error){
        fprintf(stderr,"FULL_AUDIO_FAILURE step=%u channel_error=%d\n",f->next,r->audio_channels.error);
        return -1;
    }
    r->audio_requests+=requests;
    taxi_game_audio_frame_commit(&r->full_audio,&f->context,&passengers,&r->globals,&r->timer);
    r->trip.voice_25d6=passengers.session.trip.voice_25d6;
    r->pickup.animation_25d7=passengers.pickup.animation_25d7;
    r->pickup.voice_25d6=passengers.pickup.voice_25d6;
    r->voice_gate_2604=r->full_audio.handles[7];
    r->voice_gate_2608=r->full_audio.handles[8];r->voice_timer_2614=r->full_audio.cooldown;
    r->context_voice=(TaxiGameAudioContextVoice){r->full_audio.handles[7],
        r->full_audio.handles[8],r->full_audio.context_selection,
        r->full_audio.classification,r->full_audio.driver_selection,r->full_audio.last};
    printf("A %u %llu %d %d %d %u %u %u %u %u\n",f->next,
        (unsigned long long)r->audio_tick,input.speed,input.rpm,input.angle578,
        requests,r->full_audio.handles[0],r->full_audio.handles[1],
        r->full_audio.handles[7],r->full_audio.handles[8]);
    return 0;
}
static int traffic_after_driver(void *opaque,TaxiReferenceDriving *f,
    const TaxiReferenceDrivingStep *step,const TaxiTrig *trig) {
    Replay *r=opaque;TaxiTrafficScene *s=&r->scene;
    r->diagnostic_step=f->next;
    /* Original29264 tip clocks/money/glyph projection, before movement effects. */
    if(taxi_reward_update_allowed(f->paused,f->intro,step->flag_1697,
        (uint8_t)((r->settings_0&224)==128))){
        TaxiRewardUpdateEffects effect;
        if(taxi_reward_update(&r->rewards,&r->reward_clock,&r->fare,
            &r->cash_glyphs,(int32_t)step->delta,(uint8_t)r->sound,&effect))return -1;
        for(unsigned i=0;i<effect.sound_requests;i++)
            taxi_game_audio_channel_dispatch(&r->audio_channels,(TaxiGameAudioEngineRequest){9,48,0});
    }
    /* Source0802A90A..A928 world-impact voice request follows audio service;
       use actual native world collision and the continuously shared RNG. */
    if(taxi_impact_voice(f->last_effects.world.movement.collided,
        r->trip.active_1694,&r->rng,&r->globals.voice_25d6))
        printf("I %u %u %u\n",f->next,r->globals.voice_25d6,r->rng);
    /* Source frame's clocks precede passenger pickup initialization. The
       captured driver delta is not equated with the physical VBlank count. */
    TaxiClockInput clock_input={.delta_3b00=(int32_t)step->delta,
        .gate_1700=r->passenger_gate_1700,.settings_0=r->settings_0,
        .freeze_5f20=r->freeze,.ended_1697=step->flag_1697};
    TaxiActionContext action_context={.profile=(r->settings_0>>1)&3,
        .vehicle_flags=f->car.driver.power.gears.flags_4ac};
    taxi_distance_display_step(&r->distance_display,&r->trip,(int32_t)step->delta,
        (uint8_t)taxi_action_query(&f->input,action_context,6));
    if(taxi_main_clock_step(&r->main_clock,&clock_input)||
        taxi_trip_clock_step(&r->trip,&clock_input))return -1;
    taxi_clock_transfer(&r->main_clock,&r->trip,&r->trip_end,&clock_input);
    TaxiCameraView view=taxi_reference_driving_view(f,trig);
    TaxiRoutePoint taxi={f->car.world.height.position.x,f->car.world.height.position.z};
    if(taxi_passenger_search(r->passengers,r->passenger_spawns,128,taxi,
        &r->passenger_candidate,&r->passenger_distance))return -1;
    /* Source B098..B20A: recover pickup ownership through the existing
       native routine and real shared RNG, before customer actor service. */
    if(!r->freeze&&!step->flag_1697){
        TaxiPickupInput input={.candidate_index=r->passenger_candidate,
            .taxi_position=taxi,.gate_2b78=f->car.world.speed_544,
            .gate_1700=r->passenger_gate_1700};
        if(!r->pickup.pending_1704&&r->pickup.destination_index==-1&&
            r->passenger_candidate>=0&&r->passenger_candidate<128&&
            (r->passengers[r->passenger_candidate].bytes[0]&4u)&&
            !input.gate_2b78&&!input.gate_1700){
            input.random_first=taxi_random_range_tagged(&r->rng,0,65535,"pickup-first");
            if(r->passengers[r->passenger_candidate].bytes[0]&8u)
                input.random_second=taxi_random_range_tagged(&r->rng,0,65535,"pickup-second");
        }
        r->pickup.voice_25d6=r->globals.voice_25d6;
        TaxiPickupEffects effect;
        if(taxi_passenger_pickup(&r->pickup,r->passengers,r->destinations,128,
            &input,&effect))return -1;
        if(effect.began){
            r->voice_owner_index=r->pickup.dialogue_index;
            r->globals.voice_25d6=r->pickup.voice_25d6;
        }
        printf("K %u %u %u %d %d %d %d %d %d %d %u %u %u\n",f->next,
            effect.began,effect.completed,r->pickup.pending_1704,
            r->pickup.selected_index,r->pickup.dialogue_index,r->pickup.destination_index,
            r->pickup.approach_x_7534,r->pickup.approach_z_7538,
            effect.customer_index,r->pickup.animation_25d7,r->pickup.voice_25d6,r->rng);
        if(effect.completed){
            const TaxiPassengerState *customer=r->passengers+effect.customer_index;
            int32_t route_length;memcpy(&route_length,customer->bytes+4,4);
            int32_t draw=taxi_random_range_tagged(&r->rng,0,65535,"fare-pickup");
            TaxiTripStartInput trip_input={.customer_index=effect.customer_index,
                .route_length=route_length,.remaining_16a8=r->main_clock.remaining_16a8,
                .random_result=draw,.category=customer->bytes[2],
                .settings_0=r->settings_0,.settings_3=r->settings_3};
            uint8_t palette_refresh;
            if(taxi_trip_start(&r->trip,&trip_input,&palette_refresh))return -1;
            taxi_fare_start(&r->fare,route_length);
            r->globals.voice_25d6=r->pickup.voice_25d6=r->trip.voice_25d6;
            r->active=r->trip.active_1694;
            printf("J %u %d %d %d %d %u %u %u\n",f->next,
                effect.customer_index,effect.destination_index,route_length,draw,
                palette_refresh,r->globals.voice_25d6,r->rng);
        }
    }
    if(r->passenger_candidate>=0&&r->passenger_candidate<128&&
       (r->passengers[r->passenger_candidate].bytes[0]&0x10u)&&
       !r->voice_gate_2604&&!r->voice_gate_2608&&!r->voice_timer_2614){
        r->voice_owner_index=r->passenger_candidate;
        r->globals.voice_25d6=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+18);
        r->voice_timer_2614=300;
    }
    for(unsigned i=0;i<128;i++){
        uint8_t *p=r->passengers[i].bytes;
        int32_t dx=(int32_t)((uint32_t)taxi.x-(uint32_t)r->passenger_spawns[i].x);
        int32_t dz=(int32_t)((uint32_t)taxi.z-(uint32_t)r->passenger_spawns[i].z);
        if(dx<0)dx=(int32_t)(0u-(uint32_t)dx);
        if(dz<0)dz=(int32_t)(0u-(uint32_t)dz);
        int visible=dx<=12799&&dz<=12799&&p[1]<8&&(p[0]&3u)==1;
        if(taxi_passenger_idle_rng(r->passengers+i,(int32_t)i==r->pickup.selected_index,&r->pickup,taxi,
            (int32_t)step->delta,r->freeze,&r->rng)||
           taxi_passenger_boarding(r->passengers+i,(int32_t)i==r->pickup.selected_index,&r->pickup,taxi,
            (int32_t)step->delta,r->freeze)||
           (visible&&taxi_passenger_frame_select(r->passengers+i)))return -1;
    }
    /* Both projections own original shared byte020025D6. */
    r->trip.voice_25d6=r->globals.voice_25d6;
    printf("T %u %d %d %d %d %d %d %u %u %u %d %d %d %d\n",f->next,
        r->main_clock.remaining_16a8,r->trip.active_customer,r->trip.direction_753c,
        r->trip.time_16ac,r->trip.initial_time_16b0,r->trip.allowance_16e0,
        r->trip.active_1694,r->trip.active_1695,r->trip.voice_25d6,
        r->fare.pending_whole_16a0,r->fare.pending_fraction_16a4,
        r->fare.total_whole_1698,r->fare.total_fraction_169c);
    printf("M %u %d %d %u %u %d %d\n",f->next,r->passenger_candidate,
        r->passenger_distance,r->rng,r->globals.voice_25d6,r->voice_timer_2614,
        r->voice_owner_index);
    printf("V %u %u %u %u %u %u %d %u %d\n",f->next,
        r->audio_channels.channels[2].phase,r->audio_channels.channels[2].last,
        r->context_voice.handle,r->context_voice.driver_handle,
        r->context_voice.last,r->context_voice.classification,
        r->globals.voice_25d6,r->voice_timer_2614);
    printf("P %u %u ",f->next,r->rng);
    for(unsigned i=0;i<sizeof r->passengers[98].bytes;i++)printf("%02x",r->passengers[98].bytes[i]);
    putchar('\n');
    printf("Q %u %u ",f->next,r->rng);
    for(unsigned i=0;i<sizeof r->passengers[91].bytes;i++)printf("%02x",r->passengers[91].bytes[i]);
    putchar('\n');
    unsigned heading=((uint32_t)f->car.world.heading_1c>>12)&4095u;
    s->player=&f->car.driver.power;s->player_position=f->car.world.height.position;
    s->player_sine=trig->sine[heading];s->player_cosine=trig->cosine[heading];
    s->camera=view.position;s->camera_yaw=view.yaw;s->camera_pitch=view.pitch;
    s->frame_delta=(int32_t)step->frame;s->global_delta=(int32_t)step->delta;
    s->sound_enabled=r->sound;s->trip_active=r->active;
    TaxiTrafficSpawn diagnostic_spawn={0};
    int diagnostic_grid=taxi_traffic_grid_select(&r->grid,s->camera.x,s->camera.z,
        r->counter,&diagnostic_spawn);
    printf("C %u %d %d %d %d %d %d %llu %u %d\n",f->next,
        s->camera.x,s->camera.y,s->camera.z,s->camera_yaw,s->camera_pitch,
        diagnostic_grid,(unsigned long long)diagnostic_spawn.cell,
        diagnostic_spawn.route,diagnostic_spawn.phase);
    int spawned=taxi_traffic_scene_update(r->actors,r->count,s);
    if(spawned<0)return -1;
    /* Match traffic_drive.c's duplicate-field synchronization after dispatch. */
    f->car.world.speed_544=f->car.driver.power.gears.speed_544;
    f->car.driver.motion.motion.orientation.speed_544=f->car.world.speed_544;
    f->car.world.height.scale_e84=f->car.driver.power.scale_e84;
    f->car.driver.motion.motion.scale_e84=f->car.driver.power.scale_e84;
    printf("F %u %llu %d %d %d %u %u %u %d %u %u %u %d %d %u %d %d %d %d %d\n",
        f->next,(unsigned long long)step->tick,step->delta,step->frame,spawned,r->count,
        r->timer,r->counter,r->globals.cooldown_e4,r->globals.near_miss_25d3,
        r->globals.voice_25d6,r->globals.combo_flag_9aa6,r->globals.combo_9aa8,
        r->globals.combo_9aac,r->rng,f->car.world.height.position.x,
        f->car.world.height.position.y,f->car.world.height.position.z,
        f->car.world.heading_1c,f->car.world.speed_544);
    for(unsigned i=0;i<50;i++) {
        const TaxiTrafficActor *a=r->actors+i;
        printf("A %u %u %u %u %d %d %d %d %d %u %d %d %d %d %d %d %llu %d %d %u\n",
            f->next,i,a->flags,a->type,a->countdown,a->x,a->y,a->z,a->heading,
            a->route,a->collision_timer,a->impulse_x,a->impulse_z,a->offset_x,
            a->offset_z,a->path.distance,(unsigned long long)a->path.segment,
            a->path.x,a->path.z,(unsigned)a->path.heading);
    }
    return 0;
}

int main(int argc,char **argv) {
    assert(argc==6);unsigned observe=(unsigned)strtoul(argv[5],NULL,10);assert(observe<=1);unsigned tail=(unsigned)strtoul(argv[4],NULL,10);assert(tail<=60);
    int32_t disk[50][18];read_exact(argv[1],disk,sizeof(disk));
    Config c=read_config(argv[2]);Replay r={0};
    read_exact("outputs/crazy-taxi-native/reports/original-core-reference/dispatchclean-source-audio-bank-clocks.bin",r.source_audio_bank_clocks,sizeof r.source_audio_bank_clocks);
    for(unsigned i=0;i<50;i++)load_actor(disk[i],r.actors+i);
    uint8_t initial_ram[0x40000];
    read_exact("outputs/crazy-taxi-native/reports/original-core-reference/boot4200.ewram",
        initial_ram,sizeof initial_ram);
    for(unsigned i=0;i<128;i++){
        const uint8_t *p=initial_ram+0x5f2c+i*44;
        memcpy(r.passengers[i].bytes,p,sizeof r.passengers[i].bytes);
        memcpy(&r.passenger_spawns[i].x,p+16,4);
        memcpy(&r.passenger_spawns[i].z,p+24,4);
    }
    memcpy(&r.pickup.pending_1704,initial_ram+0x1704,sizeof r.pickup.pending_1704);
    r.pickup.selected_index=-1;r.pickup.dialogue_index=-1;r.pickup.destination_index=-1;
    r.freeze=initial_ram[0x5f20];
    memcpy(&r.voice_gate_2604,initial_ram+0x2604,4);
    memcpy(&r.voice_gate_2608,initial_ram+0x2608,4);
    memcpy(&r.voice_timer_2614,initial_ram+0x2614,4);
    r.voice_owner_index=-1;
    r.rewards.gate_9aa4=initial_ram[0x9aa4];
    r.reward_clock.clock_9aa5=initial_ram[0x9aa5];
    r.reward_clock.active_count_9aa7=initial_ram[0x9aa7];
    memcpy(r.rewards.tips,initial_ram+0x9abc,sizeof r.rewards.tips);
    taxi_cash_glyphs_init(&r.cash_glyphs);
    uint32_t reward_pointer;memcpy(&reward_pointer,initial_ram+0x25fc,4);
    if(reward_pointer){assert(reward_pointer>=0x02007574&&reward_pointer<0x020075f4&&
        (reward_pointer-0x02007574)%32==0);r.reward_event.handle=(reward_pointer-0x02007574)/32+1;}
    r.distance_display.phase_16c2=initial_ram[0x16c2];
    memcpy(&r.distance_display.offset_16c4,initial_ram+0x16c4,4);
    memcpy(&r.distance_display.hold_16c8,initial_ram+0x16c8,4);
    r.settings_0=initial_ram[0x1eb0];r.settings_3=initial_ram[0x1eb3];
    memcpy(&r.main_clock.remaining_16a8,initial_ram+0x16a8,4);
    r.trip.active_customer=-1;
    memcpy(&r.trip.direction_753c,initial_ram+0x753c,4);
    memcpy(&r.trip.time_16ac,initial_ram+0x16ac,4);
    memcpy(&r.trip.initial_time_16b0,initial_ram+0x16b0,4);
    memcpy(&r.trip.allowance_16e0,initial_ram+0x16e0,4);
    memcpy(&r.trip.scale_5310,initial_ram+0x5310,2);
    memcpy(&r.trip.scale_5312,initial_ram+0x5312,2);
    memcpy(&r.trip.phase_5314,initial_ram+0x5314,2);
    r.trip.active_1694=initial_ram[0x1694];r.trip.active_1695=initial_ram[0x1695];
    r.trip.voice_25d6=initial_ram[0x25d6];
    memcpy(&r.trip_end.bonus_16e8,initial_ram+0x16e8,4);
    memcpy(&r.trip_end.bonus_display_timer_16e4,initial_ram+0x16e4,4);
    memcpy(&r.fare.pending_whole_16a0,initial_ram+0x16a0,4);
    memcpy(&r.fare.pending_fraction_16a4,initial_ram+0x16a4,4);
    memcpy(&r.fare.total_whole_1698,initial_ram+0x1698,4);
    memcpy(&r.fare.total_fraction_169c,initial_ram+0x169c,4);
    r.fare.rating_1693=initial_ram[0x1693];r.fare.qualified_1691=initial_ram[0x1691];
    r.pickup.animation_25d7=initial_ram[0x25d7];
    r.driver_voice_input.settings=initial_ram[0x1eb0];
    r.driver_voice_input.challenge_driver=initial_ram[0x1539];
    r.driver_voice_input.driver=(int8_t)initial_ram[0x1690];
    memcpy(&r.passenger_gate_1700,initial_ram+0x1700,4);
    assert(!taxi_passenger_asset_load(&r.passenger_assets,"outputs/crazy-taxi-native/assets/city1.ctp"));
    assert(r.passenger_assets.count==128);
    for(unsigned i=0;i<128;i++)r.destinations[i]=r.passenger_assets.source[i][1];
    assert(!r.voice_gate_2604&&!r.voice_gate_2608);
    assert(!taxi_audio_asset_load(&r.audio_assets,"outputs/crazy-taxi-native/assets/audio"));
    uint32_t audio_payloads[203];
    read_exact("outputs/crazy-taxi-native/work/llvm-build-20261014/audio-source-payloads.bin",
        audio_payloads,sizeof audio_payloads);
    audio_owner_snapshot_import(initial_ram,audio_payloads,&r.audio_assets,&r.full_audio,&r.audio_channels);
    r.audio_muted=initial_ram[0x1534];r.audio_outer=initial_ram[0x1ea8];
    r.audio_effects=initial_ram[0x7602];
    assert(!taxi_runtime_asset_load(&r.runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
    assert(!taxi_world_asset_load(&r.world,"outputs/crazy-taxi-native/assets/city1.ctw"));
    assert(!taxi_world_asset_load_special(&r.world,"outputs/crazy-taxi-native/assets/city1-special.cts"));
    assert(!taxi_traffic_grid_load(&r.grid,"outputs/crazy-taxi-native/assets/traffic/city1-grid.ctg"));
    char path[256];
    for(unsigned i=1;i<256;i++){
        snprintf(path,sizeof(path),"outputs/crazy-taxi-native/assets/traffic/city1-road-%03u.ctrp",i);
        FILE *f=fopen(path,"rb");if(f){assert(!fclose(f));assert(!taxi_traffic_path_load(r.roads+i,path));}
    }
    r.count=taxi_traffic_density((uint8_t)c.setting);assert(r.count==c.count&&r.count<=50);
    r.timer=(uint8_t)c.timer;r.counter=c.counter;r.rng=c.rng;r.sound=(int)c.sound;r.active=(int)c.active;
    r.globals=(TaxiTrafficGlobals){.cooldown_e4=(int16_t)c.cooldown,
        .near_miss_25d3=(uint8_t)c.near,.voice_25d6=(uint8_t)c.voice,
        .combo_flag_9aa6=(uint8_t)c.combo_flag,.combo_9aa8=c.combo_whole,
        .combo_9aac=c.combo_fraction};
    for(unsigned i=0;i<4;i++)r.scales[i]=4096;
    r.scene=(TaxiTrafficScene){.grid=&r.grid,.roads=r.roads,.trig=&r.runtime.trig,
        .angles=&r.runtime.traction,.globals=&r.globals,.timer_25d5=&r.timer,
        .rng=&r.rng,.selection_counter=&r.counter,.spacing=taxi_traffic_spacing((uint8_t)c.setting),
        .scales=r.scales,.obstacle=taxi_world_asset_obstacle,.height=taxi_world_asset_height,
        .terrain=&r.world,.submissions=r.submissions,.submission_count=50};
    TaxiReferenceDriving driving={0};
    if(taxi_reference_driving_load(&driving,argv[3]))return 2;
    assert(driving.count<=236);
    taxi_reference_driving_set_sound_observer(observe?replay_driver_sound:NULL,observe?&r:NULL);
    taxi_reference_driving_set_pre_driver_hook(full_audio_before_driver,&r);
    taxi_reference_driving_set_post_driver_hook(traffic_after_driver,&r);
    while(driving.next<driving.count){
        if(taxi_reference_driving_step(&driving,&r.runtime.trig,&r.runtime.traction,
            &r.runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&r.world))return 3;
    }
    taxi_reference_driving_set_sound_observer(NULL,NULL);
    taxi_reference_driving_set_pre_driver_hook(NULL,NULL);
    taxi_reference_driving_set_post_driver_hook(NULL,NULL);
    /* Capture endpoint can be a later VBlank than the last logical update.
       Explicit diagnostic tail consumes only real audio banks, no RNG or
       owner dispatch and no source expected phase/state injection. */
    for(unsigned bank=0;bank<tail;bank++){
        int16_t pcm[352]={0};
        for(unsigned channel=0;channel<4;channel++)
            assert(taxi_audio_channel_mix(r.audio_channels.channels+channel,pcm,352,
                r.audio_assets.codebook,608,taxi_audio_effective_volume(64,r.audio_channels.channels[channel].volume))>=0);
    }
    printf("B %u %u %u %u %u %d %d %u ",driving.next,
        r.rewards.gate_9aa4,r.reward_clock.clock_9aa5,r.reward_clock.active_count_9aa7,
        r.globals.combo_flag_9aa6,r.globals.combo_9aa8,r.globals.combo_9aac,r.rng);
    for(unsigned i=0;i<sizeof r.rewards.tips;i++)printf("%02x",((uint8_t *)r.rewards.tips)[i]);
    putchar('\n');
    printf("U %u ",driving.next);
    for(unsigned i=0;i<sizeof r.passengers;i++)printf("%02x",((uint8_t *)r.passengers)[i]);
    putchar('\n');
    printf("W %u %d %u %d %d %d %d %u %u %d %d\n",driving.next,
        r.main_clock.remaining_16a8,r.distance_display.phase_16c2,
        r.distance_display.offset_16c4,r.distance_display.hold_16c8,
        r.fare.pending_whole_16a0,r.fare.pending_fraction_16a4,
        r.fare.rating_1693,r.fare.qualified_1691,r.trip.active_customer,r.pickup.destination_index);
    for(unsigned channel=0;channel<4;channel++){
        const TaxiAudioChannel *c=r.audio_channels.channels+channel;
        printf("H %u %u %u %u %u %u %u %u %u %u %d\n",driving.next,channel,
            c->stamp,c->flags4,c->flags5,c->volume,c->last,c->rate,c->step,c->phase,
            c->sound?(int)(c->sound-r.audio_assets.sounds):-1);
    }
    taxi_reference_driving_free(&driving);taxi_world_asset_free(&r.world);
    taxi_traffic_grid_free(&r.grid);
    for(unsigned i=0;i<256;i++)free(r.roads[i].points);
    return 0;
}
