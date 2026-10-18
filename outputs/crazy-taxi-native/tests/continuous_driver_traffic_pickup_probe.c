#include "../src/reference_driving.h"
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
static int traffic_after_driver(void *opaque,TaxiReferenceDriving *f,
    const TaxiReferenceDrivingStep *step,const TaxiTrig *trig) {
    Replay *r=opaque;TaxiTrafficScene *s=&r->scene;
    /* Diagnostic uses the already verified captured update clock. Consume
       actual352-sample banks since the previous service, then service the
       prior context selection before passenger search can select a new one.
       No source expected post-state or busy result is loaded here. */
    assert(step->tick>=r->audio_tick);
    while(r->audio_tick<step->tick){
        int16_t pcm[352]={0};
        if(taxi_audio_channel_mix(r->audio_channels.channels+2,pcm,352,
            r->audio_assets.codebook,608,64)<0)return -1;
        r->audio_tick++;
    }
    /* Original combined controller services driver selection before context
       selection; pickup animation25D7 and driver selection are one byte. */
    TaxiGameAudioVoice driver_voice={r->context_voice.driver_handle,
        r->context_voice.handle,(int8_t)r->pickup.animation_25d7};
    r->driver_voice_input.intro=f->intro;
    r->driver_voice_input.ended=step->flag_1697;
    r->driver_voice_input.frozen=r->freeze;
    taxi_game_audio_voice_dispatch(&driver_voice,&r->driver_voice_input,
        taxi_game_audio_channel_dispatch,&r->audio_channels);
    if(r->audio_channels.error)return -1;
    r->context_voice.driver_handle=driver_voice.handle;
    r->context_voice.followup=driver_voice.selection;
    r->context_voice.selection=(int8_t)r->globals.voice_25d6;
    TaxiGameAudioContextVoiceInput audio_input={0};
    audio_input.intro=f->intro;audio_input.ended=step->flag_1697;
    audio_input.frozen=r->freeze;audio_input.current_class=-1;
    audio_input.alternate_class=-1;
    if(r->voice_owner_index>=0&&taxi_game_audio_customer_class(
        r->passengers+r->voice_owner_index,&audio_input.alternate_class))return -1;
    taxi_game_audio_context_voice_dispatch(&r->context_voice,&audio_input,
        taxi_game_audio_channel_dispatch,&r->audio_channels);
    if(r->audio_channels.error)return -1;
    r->globals.voice_25d6=(uint8_t)r->context_voice.selection;
    r->pickup.animation_25d7=(uint8_t)r->context_voice.followup;
    r->voice_gate_2604=r->context_voice.handle;
    r->voice_gate_2608=r->context_voice.driver_handle;
    /* Original10B54 controller cooldown subtraction, saturating at zero. */
    r->voice_timer_2614-=(int32_t)step->delta;
    if(r->voice_timer_2614<0)r->voice_timer_2614=0;
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
        /* Completion's destination/fare/clock owners are intentionally not
           replaced with source expected state; audit identifies that gap. */
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
    assert(argc==4);
    int32_t disk[50][18];read_exact(argv[1],disk,sizeof(disk));
    Config c=read_config(argv[2]);Replay r={0};
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
    r.audio_channels.assets=&r.audio_assets;r.audio_channels.output_rate=21024;
    /* Isolated context-owner diagnostic: preserve the source voice slot by
       reserving both engine channels with real protected engine assets.
       Their pitch/mixing and the rest of the controller are outside this test. */
    assert(taxi_audio_channel_start(r.audio_channels.channels,r.audio_assets.sounds+9,21024,1,1)==0);
    assert(taxi_audio_channel_start(r.audio_channels.channels,r.audio_assets.sounds+10,21024,1,1)==1);
    r.context_voice.followup=(int8_t)initial_ram[0x25d7];
    r.context_voice.last=initial_ram[0x25d8];
    r.context_voice.classification=(int8_t)initial_ram[0x2610];
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
    taxi_reference_driving_set_post_driver_hook(traffic_after_driver,&r);
    while(driving.next<driving.count){
        if(taxi_reference_driving_step(&driving,&r.runtime.trig,&r.runtime.traction,
            &r.runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&r.world))return 3;
    }
    taxi_reference_driving_set_post_driver_hook(NULL,NULL);
    taxi_reference_driving_free(&driving);taxi_world_asset_free(&r.world);
    taxi_traffic_grid_free(&r.grid);
    for(unsigned i=0;i<256;i++)free(r.roads[i].points);
    return 0;
}
