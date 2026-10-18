#include "../src/drive_world.h"
#include "../src/camera_angle.h"
#include "../src/body_points.h"
#include "../src/vehicle_start_driver.h"
#include "../src/vehicle_start_world.h"
#include "../src/driving_shared_snapshot.h"
#include "../src/input_snapshot.h"
#include "../src/actions.h"
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include "../src/traffic_frame.h"
#include "../src/traffic_presentation.h"
#include "../src/traffic_actor.h"
#include "../src/traffic_grid.h"
#include "../src/traffic_init.h"
#include "../src/game_audio_context_voice.h"
#include "../src/game_audio_reward_event.h"
#include "../src/reward.h"
#include "../src/random.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    TaxiDriveWorld *car;
    TaxiRuntimeAsset *runtime;
    TaxiWorldAsset *world;
    TaxiAiPathAsset *roads;
    int32_t frame, delta, camera_x, camera_z;
    int paused, sound, active;
    TaxiTrafficGlobals globals;
    TaxiGameAudioContextVoice voice;
    TaxiGameAudioRewardEvent reward_event;
    TaxiRewardState reward;
    uint32_t rng,reward_rng_before;
    int reward_processed;
} FrameData;
typedef struct {FrameData *frame; int busy; uint32_t start_handle;} VoiceContext;

static int32_t word(const uint8_t *p) {
    uint32_t v=(uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);
    int32_t s; memcpy(&s,&v,4); return s;
}
static void store_word(uint8_t *p,int32_t value) {
    uint32_t v; memcpy(&v,&value,4);
    p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);p[2]=(uint8_t)(v>>16);p[3]=(uint8_t)(v>>24);
}
static void read_file(const char *path,uint8_t *dst,size_t size) {
    FILE *f=fopen(path,"rb"); assert(f); assert(fread(dst,1,size,f)==size); assert(fclose(f)==0);
}
static void disk_actor(const uint8_t *b,TaxiTrafficActor *a) {
    int32_t v[18]; for(unsigned i=0;i<18;i++)memcpy(v+i,b+4*i,4);
    memset(a,0,sizeof(*a)); a->flags=(uint32_t)v[0]; a->type=(uint32_t)v[1];
    a->countdown=v[2]; a->x=v[3]; a->y=v[4]; a->z=v[5]; a->heading=v[6];
    a->route=(uint32_t)v[7]; a->path.distance=v[8];
    a->path.segment=(size_t)(uint32_t)v[9]; a->path.x=v[10]; a->path.z=v[11];
    a->path.heading=(uint16_t)v[12]; a->impulse_x=v[13]; a->impulse_z=v[14];
    a->offset_x=v[15]; a->offset_z=v[16]; a->collision_timer=v[17];
}
static unsigned heading(const TaxiDriveWorld *d) {
    return ((uint32_t)d->world.heading_1c>>12)&4095u;
}
static int follow(void *opaque,TaxiTrafficActor *a,size_t index) {
    (void)index; FrameData *f=opaque;
    if(a->route>=256||!f->roads[a->route].points)return -1;
    TaxiTrafficActorEffects e={0}; unsigned h=heading(f->car);
    return taxi_traffic_actor_follow(a,f->roads+a->route,&f->car->driver.power,
        f->car->world.height.position.x,f->car->world.height.position.z,
        f->runtime->trig.sine[h],f->runtime->trig.cosine[h],f->frame,f->delta,
        f->sound,f->active,&f->globals,&f->rng,&e);
}
static int postimpact(void *opaque,TaxiTrafficActor *a,size_t index) {
    (void)index; FrameData *f=opaque;
    if(a->route>=256||!f->roads[a->route].points)return -1;
    TaxiTrafficActorEffects e={0}; unsigned h=heading(f->car);
    return taxi_traffic_actor_postimpact(a,f->roads+a->route,&f->car->driver.power,
        f->car->world.height.position.x,f->car->world.height.position.z,
        f->runtime->trig.sine[h],f->runtime->trig.cosine[h],f->frame,f->delta,
        f->sound,f->active,&f->globals,&f->rng,0,taxi_world_asset_obstacle,
        f->world,&f->runtime->traction,&e);
}
static int common(void *opaque,TaxiTrafficActor *a,size_t index) {
    (void)index; FrameData *f=opaque;
    return taxi_traffic_presentation_prepare(a,f->camera_x,f->camera_z,f->frame,
        f->paused,taxi_world_asset_height,f->world)<0?-1:0;
}
static uint32_t voice_request(void *opaque,TaxiGameAudioEngineRequest request) {
    VoiceContext *vc=opaque;
    if(request.op==1)return vc->start_handle;
    if(request.op==6)return (uint32_t)vc->busy;
    if(request.op==8)return (uint32_t)taxi_random_range(&vc->frame->rng,0,65535);
    return 0;
}

int main(int argc,char **argv) {
    assert(argc==5);
    uint8_t ram[0x40000],iw[0x8000],disk[50*72];
    read_file(argv[1],ram,sizeof(ram)); read_file(argv[2],iw,sizeof(iw));
    read_file(argv[3],disk,sizeof(disk));
    TaxiTrafficActor actors[50]={0};
    for(unsigned i=0;i<50;i++)disk_actor(disk+i*72,actors+i);

    TaxiRuntimeAsset runtime; assert(!taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"));
    TaxiWorldAsset world={0};
    assert(!taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/city1.ctw"));
    assert(!taxi_world_asset_load_special(&world,"outputs/crazy-taxi-native/assets/city1-special.cts"));
    TaxiTrafficGrid grid={0};
    assert(!taxi_traffic_grid_load(&grid,"outputs/crazy-taxi-native/assets/traffic/city1-grid.ctg"));
    TaxiAiPathAsset roads[256]={0}; char path[256];
    for(unsigned i=1;i<256;i++) {
        snprintf(path,sizeof(path),"outputs/crazy-taxi-native/assets/traffic/city1-road-%03u.ctrp",i);
        FILE *test=fopen(path,"rb"); if(test){fclose(test);assert(!taxi_traffic_path_load(roads+i,path));}
    }

    TaxiDrivingSharedSnapshot shared;
    assert(!taxi_driving_shared_snapshot_read(&shared,ram,sizeof(ram),iw+0x53c0,48));
    TaxiDriveWorld car={0}; TaxiDriveWorldContext ctx={0}; TaxiVehicleStartPacked packed={0};
    memcpy(packed.actor,ram+0x2634,sizeof(packed.actor)); packed.state_2630=shared.context.gear.state_2630;
    assert(!taxi_vehicle_start_driver_export(&car.driver,&ctx.driver,&packed,shared.settings));
    assert(!taxi_vehicle_start_world_export(&car.world,&packed)); ctx.driver=shared.context;
    ctx.height=(TaxiHeightGlobals){.distance_1540=word(ram+0x1540),
        .takeoff_1544={word(ram+0x1544),word(ram+0x1548),word(ram+0x154c)},
        .flag_153c=ram[0x153c],.flag_153d=ram[0x153d],.flag_153e=ram[0x153e],
        .flag_1697=ram[0x1697]};
    int32_t camera_pitch=word(ram+0x2634+0x650),camera_heading=word(ram+0x2634+0x654);
    int32_t camera_offset=word(ram+0x2618),camera_target=word(ram+0x261c);
    int32_t camera_base=word(ram+0x2634+0x680),camera_parent_distance=word(ram+0x1678);
    int32_t camera_parent_height=word(ram+0x167c);
    TaxiCameraOffsets camera_offsets={word(ram+0x2628),word(ram+0x2634+0x76c),word(ram+0x2634+0x770)};
    uint16_t camera_flags=(uint16_t)(ram[0x2634+0x64c]|((uint16_t)ram[0x2634+0x64d]<<8));
    uint8_t camera_actor_flags=ram[0x2634+0x29],camera_intro=ram[0x1696],camera_paused=ram[0x5f20];
    TaxiVehicleStartPacked player_pack={0};
    memcpy(player_pack.actor,ram+0x2634,sizeof(player_pack.actor));
    TaxiInput buttons; assert(!taxi_input_snapshot_read(&buttons,ram+0x34dc,68));
    TaxiCommand cmd=shared.command;

    unsigned setting,count,timer,counter,near,voice,combo_flag,sound,active;
    int cooldown,combo_whole,combo_fraction,camera_x,camera_z; uint32_t rng;
    FILE *cf=fopen(argv[4],"r"); assert(cf);
    assert(fscanf(cf,"%u %u %u %u %d %u %u %u %d %d %u %u %d %d",
        &setting,&count,&timer,&counter,&cooldown,&near,&voice,&combo_flag,
        &combo_whole,&combo_fraction,&rng,&sound,&active,&camera_x)==14);
    assert(fscanf(cf,"%d",&camera_z)==1); assert(fclose(cf)==0);
    assert(count==taxi_traffic_density((uint8_t)setting));
    uint8_t timer_byte=(uint8_t)timer;
    FrameData fd={.car=&car,.runtime=&runtime,.world=&world,.roads=roads,
        .camera_x=camera_x,.camera_z=camera_z,.sound=(int)sound,.active=(int)active,
        .voice={.handle=(uint32_t)word(ram+0x2604),.driver_handle=(uint32_t)word(ram+0x2608),
            .selection=(int8_t)ram[0x25d6],.classification=(int8_t)ram[0x2610],
            .followup=(int8_t)ram[0x25d7],.last=ram[0x25d8]},
        .globals={.cooldown_e4=(int16_t)cooldown,.near_miss_25d3=(uint8_t)near,
            .voice_25d6=(uint8_t)voice,.combo_flag_9aa6=(uint8_t)combo_flag,
            .combo_9aa8=combo_whole,.combo_9aac=combo_fraction},
        .reward_event={.handle=0,.pending=(uint8_t)near,.cooldown=timer_byte},
        .reward={.gate_9aa4=ram[0x9aa4],.combo_9aa6=(uint8_t)combo_flag,
            .voice_25d6=(uint8_t)voice,.whole_9aa8=combo_whole,.fraction_9aac=combo_fraction,
            .random_state=rng},.rng=rng};
    TaxiTrafficFrameHooks hooks={follow,postimpact,common,&fd};

    unsigned raw,delta,frame,boundary,tick,contact; int audio_action;
    uint32_t audio_handle,audio_driver; int audio_selection,audio_class,audio_followup;
    unsigned audio_last,audio_intro,audio_ended,audio_frozen,audio_seed; uint32_t audio_start_handle;
    while(scanf("%u %u %u %u %u %u %d %d %d %u %u %d %d %d %u %u %u %u %u %u",
                &raw,&delta,&frame,&boundary,&tick,&contact,&camera_x,&camera_z,
                &audio_action,&audio_handle,&audio_driver,&audio_selection,&audio_class,
                &audio_followup,&audio_last,&audio_intro,&audio_ended,&audio_frozen,
                &audio_seed,&audio_start_handle)==20) {
        taxi_input_update(&buttons,(TaxiInputSample){.active_low_keys=(uint16_t)raw,
            .elapsed_ticks=1,.suppress_physical=ram[0x1eaa]});
        if(!boundary)continue;
        TaxiActionContext actions={.profile=(shared.settings>>1)&3,
            .vehicle_flags=car.driver.power.gears.flags_4ac};
        taxi_command_pack(&cmd,&buttons);
        TaxiGearInput in={.delta=(int32_t)delta,.frame_delta=(int32_t)frame,
            .settings=shared.settings,.freeze_177b=shared.freeze_177b,
            .sound_enabled=shared.sound_enabled};
        memcpy(in.held,buttons.held,10);
        in.action2=(uint8_t)taxi_action_query(&buttons,actions,2);
        in.action3=(uint8_t)taxi_action_query(&buttons,actions,3);
        in.action4=(uint8_t)taxi_action_query(&buttons,actions,4);
        in.action5=(uint8_t)taxi_action_query(&buttons,actions,5);
        TaxiPoint prior_position=car.world.height.position;
        store_word(player_pack.actor+0x348,prior_position.x);
        store_word(player_pack.actor+0x34c,prior_position.y);
        store_word(player_pack.actor+0x350,prior_position.z);        TaxiDriveWorldEffects effects;
        assert(!taxi_drive_world_step(&car,&ctx,&cmd,&in,0,&runtime.trig,
            &runtime.traction,&runtime.normals,taxi_world_asset_obstacle,
            taxi_world_asset_ground,&world,&effects));
        camera_heading=taxi_camera_yaw_step(camera_heading,car.world.heading_20,(int32_t)delta,camera_intro,&runtime.trig);
        camera_pitch=taxi_camera_pitch_step(camera_pitch,(int16_t)car.world.angle_18,(int32_t)delta,&runtime.trig);
        camera_offset=taxi_camera_yaw_offset_step(camera_offset,camera_target,(int32_t)frame);
        taxi_camera_mode2_offsets(&camera_offsets,car.world.height.scale_e84,car.driver.power.gears.flags_4ac,
            (int32_t)frame,camera_paused,camera_parent_distance,camera_parent_height);
        TaxiCameraView camera_view=taxi_camera_mode2_view(car.world.height.position,camera_pitch,camera_heading,
            camera_offset,&camera_offsets,camera_base,car.camera_offset_690,camera_flags,&camera_actor_flags,&runtime.trig);
        fd.camera_x=camera_view.position.x; fd.camera_z=camera_view.position.z;
        if(!contact)continue;

        fd.frame=(int32_t)frame; fd.delta=(int32_t)delta;
        fd.reward_processed=0; fd.reward_rng_before=fd.rng;
        unsigned reward_request_count=0;
                unsigned reward_pending_before=fd.globals.near_miss_25d3;
        unsigned reward_cooldown_before=timer_byte;
        if((audio_action==-4||audio_action>=0)&&reward_pending_before){
            fd.reward_event.pending=(uint8_t)reward_pending_before;
            fd.reward_event.cooldown=timer_byte;
            TaxiGameAudioRewardEventInput reward_event_input={
                .speed=car.driver.power.gears.speed_544,
                .gate00e4=fd.globals.cooldown_e4,.start=audio_start_handle};
            TaxiGameAudioEngineRequest requests[2];
            reward_request_count=taxi_game_audio_reward_event(&fd.reward_event,
                &reward_event_input,requests);
            fd.globals.near_miss_25d3=fd.reward_event.pending;
            timer_byte=fd.reward_event.cooldown;
            for(unsigned ri=0;ri<reward_request_count;ri++) if(requests[ri].op==7){
                fd.reward.combo_9aa6=fd.globals.combo_flag_9aa6;
                fd.reward.voice_25d6=fd.globals.voice_25d6;
                fd.reward.whole_9aa8=fd.globals.combo_9aa8;
                fd.reward.fraction_9aac=fd.globals.combo_9aac;
                fd.reward.random_state=fd.rng;
                TaxiRewardInput reward_input={.kind=1,.active_1694=(uint8_t)fd.active,
                    .settings_0=(uint8_t)shared.settings,.sound_enabled=(uint8_t)fd.sound,
                    .whole=(int32_t)requests[ri].handle,.fraction=requests[ri].value};
                TaxiRewardEffects reward_effects;
                assert(!taxi_reward(&fd.reward,&reward_input,&reward_effects));
                fd.rng=fd.reward.random_state;
                fd.globals.voice_25d6=fd.reward.voice_25d6;
                fd.globals.combo_flag_9aa6=fd.reward.combo_9aa6;
                fd.globals.combo_9aa8=fd.reward.whole_9aa8;
                fd.globals.combo_9aac=fd.reward.fraction_9aac;
                fd.voice.selection=(int8_t)fd.reward.voice_25d6;
                fd.reward_processed=1;
            }
            printf("W %u %u %u %u %u %u %u %d %u %d %d %u %u\n",tick,
                reward_pending_before,fd.reward_event.pending,reward_cooldown_before,
                fd.reward_event.cooldown,reward_request_count,(unsigned)fd.reward_processed,
                (int)fd.globals.voice_25d6,(unsigned)fd.globals.combo_flag_9aa6,
                (int)fd.globals.combo_9aa8,(int)fd.globals.combo_9aac,
                fd.reward_rng_before,fd.rng);
        }
        int spawned=taxi_traffic_frame(actors,count,(int32_t)delta,0,camera_x,
            camera_z,&grid,roads,taxi_traffic_spacing((uint8_t)setting),&fd.rng,
            &counter,&fd.globals.cooldown_e4,&timer_byte,&hooks);
        assert(spawned>=0);
        if(audio_action==-2) {
            int audio_selection_before=fd.voice.selection;
            fd.voice.selection=(int8_t)audio_selection;
            fd.globals.voice_25d6=(uint8_t)audio_selection;
            printf("U %u %d %d\n",tick,audio_selection_before,(int)fd.voice.selection);
        } else if(audio_action>=0||audio_action==-3||audio_action==-4) {
            if(audio_seed) fd.voice=(TaxiGameAudioContextVoice){.handle=audio_handle,.driver_handle=audio_driver,
                .selection=(int8_t)audio_selection,.classification=(int8_t)audio_class,
                .followup=(int8_t)audio_followup,.last=(uint8_t)audio_last};
            else fd.voice.selection=(int8_t)fd.globals.voice_25d6;
            uint32_t selection_rng_before=fd.reward_processed?fd.reward_rng_before:fd.rng;
            if(audio_action==-4) {
                if(!fd.reward_processed) {
                    fd.voice.selection=taxi_game_audio_context_voice_select(&fd.rng,1);
                    fd.globals.voice_25d6=(uint8_t)fd.voice.selection;
                } else fd.voice.selection=(int8_t)fd.globals.voice_25d6;
            }
            TaxiGameAudioContextVoiceInput vi={.intro=(uint8_t)audio_intro,.ended=(uint8_t)audio_ended,
                .frozen=(uint8_t)audio_frozen,.current_class=(int8_t)audio_class,
                .alternate_class=(int8_t)audio_class};
            VoiceContext vc={&fd,(audio_action==-3||audio_action==-4)?0:audio_action,audio_start_handle};
            uint32_t rng_before=fd.rng,handle_before=fd.voice.handle;
            int selection_before=fd.voice.selection,followup_before=fd.voice.followup;
            int last_before=fd.voice.last,class_before=fd.voice.classification;
            unsigned audio_count=taxi_game_audio_context_voice_dispatch(&fd.voice,&vi,
                voice_request,&vc);
            fd.globals.voice_25d6=(uint8_t)fd.voice.selection;
            printf("Q %u %d %u %d %u %d %d %d %u %u %d %d %u %u\n",tick,audio_action,
                handle_before,selection_before,fd.voice.handle,fd.voice.selection,
                followup_before,fd.voice.followup,last_before,fd.voice.last,
                class_before,fd.voice.classification,rng_before,fd.rng);
            printf("N %u %u\n",tick,audio_count);
            if(audio_action==-4)printf("R %u %u %d\n",tick,selection_rng_before,fd.voice.selection);
        }
        assert(!taxi_vehicle_start_world_import(&player_pack,&car.world));
        assert(!taxi_vehicle_start_driver_import(&player_pack,&car.driver));        uint16_t response_57c=(uint16_t)car.driver.motion.motion.response_57c;
        player_pack.actor[0x57c]=(uint8_t)response_57c;
        player_pack.actor[0x57d]=(uint8_t)(response_57c>>8);
        store_word(player_pack.actor+0x744,car.driver.motion.response_744);
        TaxiBodyPoints body_points;
        assert(taxi_body_points(&body_points,car.world.height.position,
            (uint16_t)car.world.angle_18,(uint16_t)car.world.angle_1a,
            car.world.heading_1c,car.world.drift_69c,&runtime.trig));
        for(unsigned pi=0;pi<6;pi++){
            unsigned po=0x3d8+pi*16,vo=0x438+pi*16;
            store_word(player_pack.actor+po,body_points.positions[pi].x);
            store_word(player_pack.actor+po+4,body_points.positions[pi].y);
            store_word(player_pack.actor+po+8,body_points.positions[pi].z);
            store_word(player_pack.actor+vo,body_points.vectors[pi].x);
            store_word(player_pack.actor+vo+4,body_points.vectors[pi].y);
            store_word(player_pack.actor+vo+8,body_points.vectors[pi].z);
        }
        store_word(player_pack.actor+0x650,camera_pitch);
        store_word(player_pack.actor+0x654,camera_heading);
        store_word(player_pack.actor+0x76c,camera_offsets.height_76c);
        store_word(player_pack.actor+0x770,camera_offsets.distance_770);
        player_pack.actor[0x29]=camera_actor_flags;        store_word(player_pack.actor+0x498,camera_view.position.x);
        store_word(player_pack.actor+0x49c,camera_view.position.y);
        store_word(player_pack.actor+0x4a0,camera_view.position.z);
        int32_t transform_yaw=taxi_camera_transform_yaw(camera_heading,camera_offset);
        store_word(player_pack.actor+0x4a6,transform_yaw);
        store_word(player_pack.actor+0xe18,camera_view.position.x);
        store_word(player_pack.actor+0xe1c,camera_view.position.y);
        store_word(player_pack.actor+0xe20,camera_view.position.z);
        store_word(player_pack.actor+0xe28,transform_yaw&0xfff);        int32_t shadow_ground=INT32_MIN;
        TaxiPoint shadow_query={car.world.height.position.x,word(player_pack.actor+0xe8c),car.world.height.position.z};
        assert(taxi_world_asset_height(&world,shadow_query,&shadow_ground)>=0);
        store_word(player_pack.actor+0xe88,car.world.height.position.x);
        store_word(player_pack.actor+0xe8c,(int32_t)((uint32_t)shadow_ground-192u));
        store_word(player_pack.actor+0xe90,car.world.height.position.z);
        printf("S %u %d %u %u %u %d %u %u %u %d %d %u\n",tick,spawned,count,
            timer_byte,counter,fd.globals.cooldown_e4,fd.globals.near_miss_25d3,
            fd.globals.voice_25d6,fd.globals.combo_flag_9aa6,fd.globals.combo_9aa8,
            fd.globals.combo_9aac,fd.rng);
        for(unsigned i=0;i<count;i++) {
            TaxiTrafficActor *a=actors+i;
            printf("A %u %u %u %u %d %d %d %d %d %u %d %d %d %d %d %d %llu %d %d %u\n",
                tick,i,a->flags,a->type,a->countdown,a->x,a->y,a->z,a->heading,
                a->route,a->collision_timer,a->impulse_x,a->impulse_z,a->offset_x,
                a->offset_z,a->path.distance,(unsigned long long)a->path.segment,
                a->path.x,a->path.z,(unsigned)a->path.heading);
        }
        printf("V %u %d %d %d %d %d\n",tick,car.world.height.position.x,
            car.world.height.position.y,car.world.height.position.z,
            car.world.heading_1c,car.driver.power.gears.speed_544);
        printf("C %u %d %d %d %d %d %d %d %d %d\n",tick,camera_view.position.x,
            camera_view.position.y,camera_view.position.z,camera_view.pitch,camera_view.yaw,
            camera_view.roll,camera_heading,camera_pitch,camera_offset);
        printf("P %u ",tick);
        for(unsigned bi=0;bi<sizeof(player_pack.actor);bi++)printf("%02x",player_pack.actor[bi]);
        putchar('\n');
    }
    taxi_world_asset_free(&world); taxi_traffic_grid_free(&grid); return 0;
}
