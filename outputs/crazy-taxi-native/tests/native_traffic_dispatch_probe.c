/* Offline native dispatcher differential. The source fixture is captured from
   the supplied ROM; this executable is never used by the playable viewer. */
#include "../src/reference_driving.h"
#include "../src/runtime_asset.h"
#include "../src/world_asset.h"
#include "../src/traffic_frame.h"
#include "../src/traffic_actor.h"
#include "../src/traffic_grid.h"
#include "../src/traffic_init.h"
#include "../src/vehicle_start_driver.h"
#include "../src/vehicle_start_world.h"
#include "../src/traffic_presentation.h"
#include "../src/passenger_idle.h"
#include "../src/traffic_effects.h"
#include "../src/random.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ACTORS=50, ACTOR_DISK_BYTES=72, ROWS=1755, PLAYER_BYTES=0xEA0,
       INPUT_BYTES=68, COMMAND_BYTES=48, SHARED_BYTES=408, CAMERA_BYTES=64 };
typedef struct { int16_t cooldown; uint8_t near_miss,voice,combo_flag,timer;
    int32_t combo_a,combo_b; uint32_t rng,counter; } Globals;
typedef struct { int32_t values[18]; } DiskActor;
typedef struct {
    int32_t schedule[14]; uint8_t gates[4]; int32_t camera[3];
    Globals globals; TaxiTrafficActor expected[ACTORS]; uint8_t player_actor[PLAYER_BYTES];
    uint8_t shared_owners[SHARED_BYTES];
} SourceRow;
typedef struct {
    TaxiReferenceDriving *driver; TaxiRuntimeAsset *runtime; TaxiWorldAsset *world;
    TaxiTrafficGrid *grid; TaxiAiPathAsset *roads; TaxiTrafficActor *actors;
    TaxiTrafficGlobals globals; uint8_t timer,setting; uint32_t rng,counter;
    SourceRow *row; uint8_t *passenger_before[4],*passenger_after[4],*voice_before,*voice_after,*voice_reset_before,*voice_reset_after;uint32_t *dispatch_counters;unsigned dispatch_count,dispatch_next,dispatch_calls,dispatch_skipped,follow_calls,postimpact_calls,common_calls,rng_boundary_rebases,voice_boundary_rebases,actor_checkpoint_rebases,actor_mismatches,deferred_phase_rows,phase_checkpoint_rows; int preserve_query_y,dispatch_entry_mode,source_checkpoints,source_max_speed_probe,source_exact_entry_probe,source_exact_rng_probe,source_max_speed_probe_all,source_passenger_events,source_voice_event,source_voice_reset_event,dispatcher_phase_probe,dispatcher_phase_checkpoint,unsupported_postimpact,diverged; uint32_t phase_checkpoint_counter;Globals phase_checkpoint_globals;TaxiTrafficActor phase_checkpoint_actors[ACTORS];char divergence[256],hook_failure[256];
} Replay;

static uint32_t u32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t s32(const uint8_t *p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static uint16_t u16(const uint8_t *p){return (uint16_t)(p[0]|((uint16_t)p[1]<<8));}
static int read_exact(FILE *f,void *p,size_t n){return fread(p,1,n,f)==n?0:-1;}
static uint8_t *read_ewram_checkpoint(const char *path){FILE *f=fopen(path,"rb");if(!f)return NULL;uint8_t *data=malloc(0x40000);if(!data||read_exact(f,data,0x40000)||fgetc(f)!=EOF){free(data);data=NULL;}fclose(f);return data;}
static int read_actors(FILE *f,TaxiTrafficActor actors[ACTORS]);
static int read_globals(FILE *f,Globals *g){uint8_t b[22];if(read_exact(f,b,sizeof b))return -1;g->cooldown=(int16_t)u16(b);g->near_miss=b[2];g->voice=b[3];g->combo_flag=b[4];g->timer=b[5];g->combo_a=s32(b+6);g->combo_b=s32(b+10);g->rng=u32(b+14);g->counter=u32(b+18);return 0;}
static int read_phase_checkpoint(const char *path,uint32_t *counter,Globals *globals,TaxiTrafficActor actors[ACTORS]){FILE *f=fopen(path,"rb");if(!f)return -1;uint8_t header[12];int rc=read_exact(f,header,sizeof header)||memcmp(header,"CTTPH001",8);if(!rc){*counter=u32(header+8);rc=read_globals(f,globals)||read_actors(f,actors)||fgetc(f)!=EOF;}fclose(f);return rc?-1:0;}
static int read_dispatch_counters(const char *path,Replay *r){FILE *f=fopen(path,"r");if(!f)return -1;char magic[40]={0};unsigned count=0;int bad=fscanf(f,"%39s %u",magic,&count)!=2||strcmp(magic,"SOURCE_DISPATCH_COUNTERS_V1")||!count||count>4096;if(!bad){r->dispatch_counters=calloc(count,sizeof *r->dispatch_counters);if(!r->dispatch_counters)bad=1;}for(unsigned i=0;!bad&&i<count;i++){unsigned value;if(fscanf(f,"%u",&value)!=1||(i&&value<=r->dispatch_counters[i-1]))bad=1;else r->dispatch_counters[i]=value;}char extra;if(!bad&&fscanf(f," %c",&extra)==1)bad=1;if(ferror(f))bad=1;if(fclose(f))bad=1;if(bad){free(r->dispatch_counters);r->dispatch_counters=NULL;return -1;}r->dispatch_count=count;return 0;}
static void disk_actor(const uint8_t *b,TaxiTrafficActor *a){
    int32_t v[18];for(unsigned i=0;i<18;i++)v[i]=s32(b+4*i);
    memset(a,0,sizeof *a);a->flags=(uint32_t)v[0];a->type=(uint32_t)v[1];a->countdown=v[2];
    a->x=v[3];a->y=v[4];a->z=v[5];a->heading=v[6];a->route=(uint32_t)v[7];
    a->path.distance=v[8];a->path.segment=(size_t)(uint32_t)v[9];a->path.x=v[10];a->path.z=v[11];a->path.heading=(uint16_t)v[12];
    a->impulse_x=v[13];a->impulse_z=v[14];a->offset_x=v[15];a->offset_z=v[16];a->collision_timer=v[17];
}
static int read_actors(FILE *f,TaxiTrafficActor actors[ACTORS]){uint8_t raw[ACTORS*ACTOR_DISK_BYTES];if(read_exact(f,raw,sizeof raw))return -1;for(unsigned i=0;i<ACTORS;i++)disk_actor(raw+i*ACTOR_DISK_BYTES,actors+i);return 0;}
static int actor_equal(const TaxiTrafficActor *a,const TaxiTrafficActor *b){
    return a->flags==b->flags&&a->type==b->type&&a->countdown==b->countdown&&a->x==b->x&&a->y==b->y&&a->z==b->z&&a->heading==b->heading&&a->route==b->route&&a->collision_timer==b->collision_timer&&a->impulse_x==b->impulse_x&&a->impulse_z==b->impulse_z&&a->offset_x==b->offset_x&&a->offset_z==b->offset_z&&a->path.distance==b->path.distance&&a->path.segment==b->path.segment&&a->path.x==b->path.x&&a->path.z==b->path.z&&a->path.heading==b->path.heading;
}
static int actor_equal_except_spawn_phase(const TaxiTrafficActor *a,const TaxiTrafficActor *b){
    return a->countdown==b->countdown&&a->x==b->x&&a->z==b->z&&a->heading==b->heading&&a->route==b->route&&a->collision_timer==b->collision_timer&&a->impulse_x==b->impulse_x&&a->impulse_z==b->impulse_z&&a->offset_x==b->offset_x&&a->offset_z==b->offset_z&&a->path.distance==b->path.distance&&a->path.segment==b->path.segment&&a->path.x==b->path.x&&a->path.z==b->path.z&&a->path.heading==b->path.heading;
}
static int read_row(FILE *f,SourceRow *r,int event_mode){
    if(read_exact(f,r->schedule,sizeof r->schedule)||read_exact(f,r->gates,sizeof r->gates)||read_exact(f,r->camera,sizeof r->camera)||read_globals(f,&r->globals)||read_actors(f,r->expected))return -1;
    if(event_mode){if(read_exact(f,r->player_actor,sizeof r->player_actor)||fseek(f,INPUT_BYTES+COMMAND_BYTES,SEEK_CUR)||read_exact(f,r->shared_owners,sizeof r->shared_owners)||fseek(f,CAMERA_BYTES,SEEK_CUR))return -1;}
    else if(fseek(f,PLAYER_BYTES+INPUT_BYTES+COMMAND_BYTES,SEEK_CUR)||read_exact(f,r->shared_owners,sizeof r->shared_owners)||fseek(f,CAMERA_BYTES,SEEK_CUR))return -1;
    return 0;
}
static unsigned heading_index(const TaxiReferenceDriving *f){return ((uint32_t)f->car.world.heading_1c>>12)&4095u;}
static int hook_follow(void *opaque,TaxiTrafficActor *a,size_t index){
    Replay *r=opaque;r->follow_calls++;TaxiReferenceDriving *f=r->driver;const TaxiReferenceDrivingStep *s=f->steps+f->next;
    unsigned h=heading_index(f);TaxiTrafficActorEffects effects={0};
    if(a->route>=256||!r->roads[a->route].points){snprintf(r->hook_failure,sizeof r->hook_failure,"missing route asset slot%u route%u",(unsigned)index,a->route);return -1;}
    int trace=getenv("TAXI_TRAFFIC_REPLAY_TRACE")&&f->next==0&&index==1;
    TaxiTrafficActor before=*a;int32_t power_speed=f->car.driver.power.gears.speed_544;
    int32_t taxi_x=f->car.world.height.position.x,taxi_z=f->car.world.height.position.z;
    int rc=taxi_traffic_actor_follow(a,r->roads+a->route,&f->car.driver.power,
        taxi_x,taxi_z,r->runtime->trig.sine[h],r->runtime->trig.cosine[h],
        (int32_t)s->frame,(int32_t)s->delta,r->row->gates[1],r->row->gates[2],
        &r->globals,&r->rng,&effects);
    if(trace)fprintf(stderr,"COUNTER5294_FOLLOW_INPUT speed=%d world_speed=%d taxi=%d,%d actor=%d,%d path=%d,%d delta=%d/%d collision=%u after=%d,%d flags=%u timer=%d\n",power_speed,f->car.world.speed_544,taxi_x,taxi_z,before.x,before.z,before.path.x,before.path.z,s->frame,s->delta,effects.collision,a->x,a->z,a->flags,a->collision_timer);
    if(rc)snprintf(r->hook_failure,sizeof r->hook_failure,"follow failed slot%u route%u pos%d,%d,%d",(unsigned)index,a->route,a->x,a->y,a->z);
    return rc;
}
static int hook_postimpact(void *opaque,TaxiTrafficActor *a,size_t index){
    Replay *r=opaque;r->postimpact_calls++;TaxiReferenceDriving *f=r->driver;const TaxiReferenceDrivingStep *s=f->steps+f->next;
    unsigned h=heading_index(f);TaxiTrafficActorEffects effects={0};
    if(a->route>=256||!r->roads[a->route].points){snprintf(r->hook_failure,sizeof r->hook_failure,"missing postimpact route slot%u route%u",(unsigned)index,a->route);return -1;}
    int rc=taxi_traffic_actor_postimpact(a,r->roads+a->route,&f->car.driver.power,
        f->car.world.height.position.x,f->car.world.height.position.z,
        r->runtime->trig.sine[h],r->runtime->trig.cosine[h],(int32_t)s->frame,(int32_t)s->delta,
        r->row->gates[1],r->row->gates[2],&r->globals,&r->rng,r->preserve_query_y,
        taxi_world_asset_obstacle,r->world,&r->runtime->traction,&effects);
    if(rc)snprintf(r->hook_failure,sizeof r->hook_failure,"postimpact failed slot%u route%u preserveY%d",(unsigned)index,a->route,r->preserve_query_y);
    return rc;
}
static int hook_common(void *opaque,TaxiTrafficActor *a,size_t index){
    Replay *r=opaque;r->common_calls++;const TaxiReferenceDrivingStep *s=r->driver->steps+r->driver->next;
    int rc=taxi_traffic_presentation_prepare(a,r->row->camera[0],r->row->camera[2],
        (int32_t)s->frame,r->row->gates[0],taxi_world_asset_height,r->world);
    if(rc<0){snprintf(r->hook_failure,sizeof r->hook_failure,"presentation/height failed slot%u route%u pos%d,%d,%d",(unsigned)index,a->route,a->x,a->y,a->z);return -1;}
    return 0;
}
static int traffic_globals_equal(const Replay *r,const Globals *s){
    return r->globals.cooldown_e4==s->cooldown&&r->globals.near_miss_25d3==s->near_miss&&r->globals.combo_flag_9aa6==s->combo_flag&&r->globals.combo_9aa8==s->combo_a&&r->globals.combo_9aac==s->combo_b&&r->timer==s->timer&&r->counter==s->counter;
}
static int source_passenger_boundary(Replay *r,unsigned row_index,const TaxiReferenceDrivingStep *step){
    if(!r->source_passenger_events)return 0;
    if(r->dispatch_entry_mode&&row_index!=30&&row_index!=36&&row_index!=56&&row_index!=64)return 0;
    if(!r->dispatch_entry_mode&&(row_index!=30&&row_index!=36&&row_index!=56&&row_index!=64))return 0;
    unsigned event=row_index==30?0:(row_index==36?1:(row_index==56?2:3));size_t offset=(size_t)(((event==1||event==3)?0x02006ed0ul:0x02007004ul)-0x02000000ul);
    const uint8_t *before=r->passenger_before[event],*after=r->passenger_after[event];
    if(!before||!after){snprintf(r->divergence,sizeof r->divergence,"missing passenger boundary snapshots");return -1;}
    int32_t expected_counter=event==0?151:(event==1?181:(event==2?281:317));
    if(r->dispatch_entry_mode?step->tick!=(uint64_t)expected_counter:r->row->schedule[9]!=expected_counter){snprintf(r->divergence,sizeof r->divergence,"passenger event row/counter mismatch at row%u counter%d tick%llu",row_index,r->row->schedule[9],(unsigned long long)step->tick);return -1;}
    if(r->rng!=u32(before+0x3b04)){snprintf(r->divergence,sizeof r->divergence,"passenger event RNG input mismatch row%u replay=%u source=%u",row_index,r->rng,u32(before+0x3b04));return -1;}
    TaxiPassengerState passenger;memcpy(passenger.bytes,before+offset,sizeof passenger.bytes);
    TaxiPickupState pickup={0};pickup.pending_1704=s32(before+0x1704);
    TaxiRoutePoint taxi={step->expected.x,step->expected.z};
    int idle_rc=taxi_passenger_idle_rng(&passenger,before[0x1694]!=0,&pickup,taxi,(int32_t)step->delta,before[0x5f20],&r->rng);
    int frame_rc=idle_rc?0:taxi_passenger_frame_select(&passenger);
    int passenger_match=!memcmp(passenger.bytes,after+offset,sizeof passenger.bytes);
    if(idle_rc||frame_rc||!passenger_match){
        fprintf(stderr,"PASSENGER_EVENT_DEBUG row=%u step_delta=%d taxi=%d,%d source_taxi=%d,%d selected=%u freeze=%u pending=%d rng=%u expected_rng=%u idle_rc=%d frame_rc=%d passenger_equal=%d\\n",row_index,step->delta,taxi.x,taxi.z,s32(after+0x2634),s32(after+0x263c),before[0x1694],before[0x5f20],pickup.pending_1704,r->rng,u32(after+0x3b04),idle_rc,frame_rc,passenger_match);
        for(unsigned i=0;i<sizeof passenger.bytes;i++)if(passenger.bytes[i]!=after[offset+i])fprintf(stderr," passenger byte%u native=%u source=%u\\n",i,passenger.bytes[i],after[offset+i]);
        snprintf(r->divergence,sizeof r->divergence,"native passenger idle mismatch at source row%u",row_index);return -1;
    }
    return 0;
}
static int source_voice_boundary(Replay *r,unsigned row_index,const TaxiReferenceDrivingStep *step){
    if(!r->source_voice_event)return 0;
    if(r->dispatch_entry_mode?step->tick!=378:row_index!=77)return 0;
    const uint8_t *before=r->voice_before,*after=r->voice_after;
    if(!before||!after||(r->dispatch_entry_mode?step->tick!=378:r->row->schedule[9]!=378)||r->rng!=u32(before+0x3b04)){
        snprintf(r->divergence,sizeof r->divergence,"voice-prompt input mismatch row%u counter%d rng=%u source=%u",row_index,r->row->schedule[9],r->rng,before?u32(before+0x3b04):0);return -1;
    }
    if(u32(before+0x2604)||u32(before+0x2608)||u32(before+0x2614)){
        snprintf(r->divergence,sizeof r->divergence,"voice-prompt source guards were not clear at row%u",row_index);return -1;
    }
    uint8_t voice=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+18);r->globals.voice_25d6=voice;
    if(r->rng!=u32(after+0x3b04)||voice!=after[0x25d6]||u32(after+0x168c)!=0x02007004u||u32(after+0x2614)!=300u){
        snprintf(r->divergence,sizeof r->divergence,"voice-prompt RNG/voice/timer mismatch at row%u",row_index);return -1;
    }
    return 0;
}
static int source_voice_reset_boundary(Replay *r,unsigned row_index,const TaxiReferenceDrivingStep *step){
    if(!r->source_voice_reset_event)return 0;
    if(r->dispatch_entry_mode?step->tick!=415:row_index!=85)return 0;
    const uint8_t *before=r->voice_reset_before,*after=r->voice_reset_after;
    if(!before||!after||(r->dispatch_entry_mode?step->tick!=415:r->row->schedule[9]!=415)||before[0x25d6]!=18||after[0x25d6]!=255){
        snprintf(r->divergence,sizeof r->divergence,"source voice-reset boundary is not the captured counter4610 event at row85/tick415");return -1;
    }
    if(r->globals.voice_25d6!=before[0x25d6]||r->rng!=u32(before+0x3b04)||u32(before+0x3b04)!=u32(after+0x3b04)){
        snprintf(r->divergence,sizeof r->divergence,"voice-reset boundary input mismatch row%u voice=%u/%u rng=%u/%u/%u",row_index,r->globals.voice_25d6,before[0x25d6],r->rng,u32(before+0x3b04),u32(after+0x3b04));return -1;
    }
    fprintf(stderr,"SOURCE_VOICE_RESET row=85 tick=415: source store at ROM 0x08011446 ran at counter4610 after tick414; apply observed 18->255 before next source traffic row. RNG unchanged; no actor/global checkpoint reseed.\n");
    r->globals.voice_25d6=after[0x25d6];
    return 0;
}
static int source_voice_selection_boundary(Replay *r,unsigned row_index,const TaxiReferenceDrivingStep *step){
    if(!r->source_voice_reset_event||row_index!=102)return 0;
    if((r->dispatch_entry_mode?step->tick!=500:r->row->schedule[9]!=500)||r->globals.voice_25d6!=255||r->rng==0){
        snprintf(r->divergence,sizeof r->divergence,"source voice-selection boundary input mismatch at row%u tick%u voice=%u rng=%u",row_index,(unsigned)r->row->schedule[9],r->globals.voice_25d6,r->rng);return -1;
    }
    /* ROM 0800B19C: random(0,65535)%2+2 -> 020025D7.
       ROM 0800B1B4: random(0,65535)&1+20 -> 020025D6. Both calls
       were observed at VBlank499 with r5=02007004 in the focused trace. */
    uint8_t mode=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+2);
    uint8_t voice=(uint8_t)((taxi_random_range(&r->rng,0,65535)&1)+20);
    size_t owner_offset=304; /* packed 020025D0 after the preceding six owners */
    const uint8_t *owner=r->row->shared_owners+owner_offset;
    if(owner[6]!=voice||owner[7]!=mode||voice!=r->row->globals.voice||r->rng!=r->row->globals.rng){
        snprintf(r->divergence,sizeof r->divergence,"source voice-selection result mismatch row%u mode=%u/%u voice=%u/%u rng=%u/%u",row_index,mode,owner[7],voice,r->row->globals.voice,r->rng,r->row->globals.rng);return -1;
    }
    fprintf(stderr,"SOURCE_VOICE_SELECTION row=102 tick=500: original calls 0x0800B19C/0x0800B1B4 at captured VBlank499 reproduce mode=%u voice=%u and both RNG transitions; no checkpoint reseed.\n",mode,voice);
    r->globals.voice_25d6=voice;
    return 0;
}
static int source_trip_start_voice_boundary(Replay *r,const TaxiReferenceDrivingStep *step){
    if(!r->dispatch_entry_mode||!r->source_voice_event||step->tick!=516)return 0;
    /* Original 0x0800CCA0 was observed at VBlank515 with r5=0x020016AC.
       Its return is reduced mod2, +12, and stored to voice 0x020025D6. */
    if(r->globals.voice_25d6!=21||!r->rng){snprintf(r->divergence,sizeof r->divergence,"trip-start voice input mismatch tick%llu voice=%u rng=%u",(unsigned long long)step->tick,r->globals.voice_25d6,r->rng);return -1;}
    uint8_t voice=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+12);
    if(voice!=r->row->globals.voice||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"trip-start voice RNG result mismatch voice=%u/%u rng=%u/%u",voice,r->row->globals.voice,r->rng,r->row->globals.rng);return -1;}
    fprintf(stderr,"SOURCE_TRIP_START_VOICE tick=516: original call 0x0800CCA0 at VBlank515 reproduces voice=%u and RNG; audio playback remains outside this comparison.\n",voice);
    r->globals.voice_25d6=voice;
    return 0;
}
static int source_trip_running_voice_boundary(Replay *r,const TaxiReferenceDrivingStep *step){
    if(!r->dispatch_entry_mode||!r->source_voice_event||(step->tick!=628&&step->tick!=698))return 0;
    /* Original 0x0802A91C runs at VBlank627 with trip-active set. It stores
       random(0,65535)%3+6 to the shared voice byte. A second call of the
       same ROM site is logged at VBlank697 with the trip still active, after
       the preceding voice request has returned to sentinel255. */
    uint8_t expected_input_voice=step->tick==628?13:255;
    if(r->globals.voice_25d6!=expected_input_voice||!r->rng){snprintf(r->divergence,sizeof r->divergence,"running-voice input mismatch tick%llu voice=%u expected=%u rng=%u",(unsigned long long)step->tick,r->globals.voice_25d6,expected_input_voice,r->rng);return -1;}
    uint8_t voice=(uint8_t)(taxi_random_range(&r->rng,0,65535)%3+6);
    if(voice!=r->row->globals.voice||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"running-voice RNG result mismatch voice=%u/%u rng=%u/%u",voice,r->row->globals.voice,r->rng,r->row->globals.rng);return -1;}
    fprintf(stderr,"SOURCE_TRIP_RUNNING_VOICE tick=%llu: original call 0x0802A91C at VBlank%llu reproduces voice=%u and RNG; audio playback remains outside this comparison.\n",(unsigned long long)step->tick,(unsigned long long)(step->tick-1),voice);
    r->globals.voice_25d6=voice;
    return 0;
}
static int source_trip_voice_mode_end_boundary(Replay *r,const TaxiReferenceDrivingStep *step){
    if(!r->dispatch_entry_mode||!r->source_voice_event||step->tick!=633)return 0;
    /* Original 0x080113EA chooses mode 11/12, then the surrounding routine
       writes the voice sentinel 255. The source call was observed at VBlank632. */
    if(r->globals.voice_25d6!=8||!r->rng){snprintf(r->divergence,sizeof r->divergence,"trip voice-mode-end input mismatch tick%llu voice=%u rng=%u",(unsigned long long)step->tick,r->globals.voice_25d6,r->rng);return -1;}
    uint8_t mode=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+11);
    const uint8_t *owner=r->row->shared_owners+304;
    if(owner[6]!=255||owner[7]!=mode||r->row->globals.voice!=255||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"trip voice-mode-end result mismatch mode=%u/%u voice=%u rng=%u/%u",mode,owner[7],r->row->globals.voice,r->rng,r->row->globals.rng);return -1;}
    fprintf(stderr,"SOURCE_TRIP_VOICE_MODE_END tick=633: original call 0x080113EA at VBlank632 reproduces mode=%u and voice sentinel255; audio playback remains outside this comparison.\n",mode);
    r->globals.voice_25d6=255;
    return 0;
}
static int source_trip_voice_mode_refresh_boundary(Replay *r,const TaxiReferenceDrivingStep *step){
    if(!r->dispatch_entry_mode||!r->source_voice_event||step->tick!=773)return 0;
    /* At VBlank772, 0x0801142C consumes one draw, stores mode 13/14 at
       0x020025D7, and writes the 255 voice sentinel. */
    if(r->globals.voice_25d6!=8||r->rng!=2014618744u){snprintf(r->divergence,sizeof r->divergence,"late trip voice-mode input mismatch tick%llu voice=%u rng=%u",(unsigned long long)step->tick,r->globals.voice_25d6,r->rng);return -1;}
    uint8_t mode=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+13);
    const uint8_t *owner=r->row->shared_owners+304;
    if(owner[6]!=255||owner[7]!=mode||r->row->globals.voice!=255||r->rng!=1806590897u){snprintf(r->divergence,sizeof r->divergence,"late trip voice-mode result mismatch mode=%u/%u voice=%u rng=%u",mode,owner[7],r->row->globals.voice,r->rng);return -1;}
    fprintf(stderr,"SOURCE_TRIP_VOICE_MODE_REFRESH tick=773: call 0x0801142C at VBlank772 reproduces mode=%u, voice sentinel255, and first RNG draw.\n",mode);
    r->globals.voice_25d6=255;
    return 0;
}
static int source_external_selector_rng_boundary(Replay *r,const TaxiReferenceDrivingStep *step){
    if(!r->dispatch_entry_mode||!r->source_voice_event||step->tick!=887)return 0;
    /* Original call 0x0802302E runs at VBlank886 with r5=0x02006ED0.
       The isolated source capture shows its selector byte at 0x02006EDA
       changes 0->1 while the shared RNG advances one step. */
    if(r->rng!=36762375u){snprintf(r->divergence,sizeof r->divergence,"external selector RNG input mismatch tick%llu rng=%u",(unsigned long long)step->tick,r->rng);return -1;}
    (void)taxi_random_range(&r->rng,0,65535);
    if(r->rng!=39970318u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"external selector RNG result mismatch native=%u source=%u",r->rng,r->row->globals.rng);return -1;}
    fprintf(stderr,"SOURCE_EXTERNAL_SELECTOR_RNG tick=887: original call 0x0802302E at VBlank886 advances shared RNG once; companion EWRAM snapshots show 0x02006EDA changing 0->1. Selector semantics outside traffic remain unmodeled.\n");
    return 0;
}
static int dispatch_after_driver(void *opaque,TaxiReferenceDriving *f,const TaxiReferenceDrivingStep *step,const TaxiTrig *trig){
    (void)f;(void)trig;Replay *r=opaque;SourceRow *row=r->row;
    if(row->schedule[0]!=(int32_t)step->raw||row->schedule[2]!=(int32_t)step->delta||row->schedule[3]!=(int32_t)step->frame||row->schedule[4]!=step->expected.x||row->schedule[5]!=step->expected.y||row->schedule[6]!=step->expected.z){snprintf(r->divergence,sizeof r->divergence,"fixture schedule row mismatch");r->diverged=1;return -1;}
    int dispatch_due=1;
    if(r->dispatch_count){
        uint32_t source_counter=(uint32_t)row->schedule[9];
        if(r->dispatch_next<r->dispatch_count){
            uint32_t expected_counter=r->dispatch_counters[r->dispatch_next];
            if(source_counter>expected_counter){snprintf(r->divergence,sizeof r->divergence,"captured dispatcher counter %u is missing before candidate row counter %u",expected_counter,source_counter);r->diverged=1;return -1;}
            if(source_counter==expected_counter){dispatch_due=1;r->dispatch_next++;r->dispatch_calls++;}
            else{dispatch_due=0;r->dispatch_skipped++;}
        }else{dispatch_due=0;r->dispatch_skipped++;}
    }
    /* Forensic one-row probe: VBlank90 is post-impact, but maximum_550 still
       records 22,140 from the preceding driver calculation. Test whether that
       retained value reproduces this collision; it is not assumed to be a
       general traffic-dispatch input. */
    int source_collision_timer_advanced=0;
    if(r->source_max_speed_probe_all&&step->speed!=0)
        for(unsigned i=0;i<ACTORS;i++)if(row->expected[i].collision_timer>r->actors[i].collision_timer&&row->expected[i].collision_timer>0){source_collision_timer_advanced=1;break;}
    if((r->source_exact_entry_probe||r->source_exact_rng_probe)&&f->next==18){
        if(step->tick!=91||step->expected.x!=23086||step->expected.y!=19200||step->expected.z!=-33528||step->speed!=5535){snprintf(r->divergence,sizeof r->divergence,"counter4286 exact-entry probe is not aligned to source collision checkpoint");r->diverged=1;return -1;}
        f->car.driver.power.gears.speed_544=22140;
        fprintf(stderr,"SOURCE_EXACT_COLLISION_ENTRY tick=91: use captured 0x08006908 speed544=22140 at counter4286; source return clears actor0 and leaves player speed5535.\n");
    }
    if((r->source_max_speed_probe&&f->next==18)||(r->source_max_speed_probe_all&&source_collision_timer_advanced))
        f->car.driver.power.gears.speed_544=f->car.driver.power.maximum_550;
    if(r->source_exact_rng_probe&&f->next==30){
        if(step->tick!=151||r->rng!=1136748202u){snprintf(r->divergence,sizeof r->divergence,"source selector RNG event is not aligned before candidate row30");r->diverged=1;return -1;}
        /* Original call 0x0802302E at observed VBlank150, r5=0x02007004,
           is the only RNG caller logged there. Untouched source snapshots
           change 0x02003B04 from1136748202 to470148629 on that VBlank. */
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=470148629u||r->row->globals.rng!=r->rng){snprintf(r->divergence,sizeof r->divergence,"source selector RNG result mismatch native=%u source=%u",r->rng,r->row->globals.rng);r->diverged=1;return -1;}
        fprintf(stderr,"SOURCE_EXTERNAL_SELECTOR_RNG tick=150: call 0x0802302E with r5=0x02007004 advances RNG 1136748202->470148629.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==36){
        if(step->tick!=181||step->expected.x!=24527||step->expected.y!=19200||step->expected.z!=-32902||step->speed!=10438||r->rng!=470148629u){snprintf(r->divergence,sizeof r->divergence,"counter4376 post-impact input is not aligned to source checkpoint");r->diverged=1;return -1;}
        /* The independent callsite trace records original 0x0802302E with
           r5=0x02006ED0 at VBlank180; its single range draw precedes 0x08006CE8. */
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=1040960554u){snprintf(r->divergence,sizeof r->divergence,"counter4376 pre-impact RNG mismatch %u",r->rng);r->diverged=1;return -1;}
        f->car.driver.power.gears.speed_544=27835;
        fprintf(stderr,"SOURCE_EXACT_POSTIMPACT_ENTRY counter=4376: 0x08006CE8 receives speed544=27835 and RNG=1040960554 after the 0x0802302E/r5=0x02006ED0 draw at VBlank180.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==52){
        if(step->tick!=261||step->expected.x!=26084||step->expected.y!=19200||step->expected.z!=-32114||step->speed!=11682||r->rng!=304681575u){snprintf(r->divergence,sizeof r->divergence,"counter4456 post-impact input is not aligned to source checkpoint: tick=%llu speed=%d rng=%u",(unsigned long long)step->tick,step->speed,r->rng);r->diverged=1;return -1;}
        /* Exact 0x08006CE8 capture at counter4456: source enters before the
           player update with speed544=31153; the translated collision response
           should produce the 11,682 speed recorded after the source frame. */
        f->car.driver.power.gears.speed_544=31153;
        fprintf(stderr,"SOURCE_EXACT_POSTIMPACT_ENTRY counter=4456: apply captured pre-update speed544=31153 and RNG=304681575; verify translated output against the tick261 actor/player checkpoint.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==56){
        if(step->tick!=281||r->rng!=1118063004u){snprintf(r->divergence,sizeof r->divergence,"counter4476 selector RNG input mismatch tick=%llu rng=%u",(unsigned long long)step->tick,r->rng);r->diverged=1;return -1;}
        /* Captured RNG caller trace: 0x080230E0 with r5=0x02007004 at VBlank280. */
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=516929657u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4476 selector RNG output mismatch native=%u source=%u",r->rng,r->row->globals.rng);r->diverged=1;return -1;}
        fprintf(stderr,"SOURCE_EXTERNAL_SELECTOR_RNG tick=280: call 0x080230E0 with r5=0x02007004 advances RNG 1118063004->516929657.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==64){
        if(step->tick!=317||r->rng!=516929657u){snprintf(r->divergence,sizeof r->divergence,"counter4512 selector RNG input mismatch tick=%llu rng=%u",(unsigned long long)step->tick,r->rng);r->diverged=1;return -1;}
        /* Captured caller trace: same ROM BL, but r5=0x02006ED0 at VBlank316. */
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=1000304882u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4512 selector RNG output mismatch native=%u source=%u",r->rng,r->row->globals.rng);r->diverged=1;return -1;}
        fprintf(stderr,"SOURCE_EXTERNAL_SELECTOR_RNG tick=316: call 0x080230E0 with r5=0x02006ED0 advances RNG 516929657->1000304882.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==77){
        if(step->tick!=378||r->rng!=1000304882u){snprintf(r->divergence,sizeof r->divergence,"counter4573 external RNG input mismatch tick=%llu rng=%u",(unsigned long long)step->tick,r->rng);r->diverged=1;return -1;}
        /* Independent callsite trace records one source draw at 0x0800B24E,
           r5=0x0200168C, VBlank377. Owner semantics remain unassigned. */
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=1899946468u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4573 external RNG output mismatch native=%u source=%u",r->rng,r->row->globals.rng);r->diverged=1;return -1;}
        if(r->globals.voice_25d6!=255||r->row->globals.voice!=18){snprintf(r->divergence,sizeof r->divergence,"counter4573 external voice boundary mismatch native=%u source=%u",r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        r->globals.voice_25d6=18;
        fprintf(stderr,"SOURCE_EXTERNAL_VOICE_AND_RNG_BOUNDARY tick=377: call 0x0800B24E with r5=0x0200168C advances RNG 1000304882->1899946468; source snapshot changes voice 255->18. Higher-level write owner remains unidentified.\\n");
    }
    if(r->source_exact_rng_probe&&f->next==155){
        if(step->tick!=773||r->row->schedule[9]!=4968||r->rng!=2014618744u||r->globals.voice_25d6!=8||r->row->globals.voice!=255){snprintf(r->divergence,sizeof r->divergence,"counter4968 voice-mode refresh input mismatch tick=%llu counter=%d rng=%u voice=%u/%u",(unsigned long long)step->tick,r->row->schedule[9],r->rng,r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        uint8_t mode=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+13);
        const uint8_t *owner=r->row->shared_owners+304;
        if(mode!=14||r->rng!=1806590897u||owner[6]!=255||owner[7]!=mode){snprintf(r->divergence,sizeof r->divergence,"counter4968 voice-mode refresh result mismatch mode=%u rng=%u owner=%u/%u",mode,r->rng,owner[6],owner[7]);r->diverged=1;return -1;}
        r->globals.voice_25d6=255;
        fprintf(stderr,"SOURCE_TRIP_VOICE_MODE_REFRESH counter4968: call 0x0801142C selects mode14 and advances RNG 2014618744->1806590897 before two source actor-type draws.\n");
    }
    if(r->source_exact_rng_probe&&f->next==127){
        if(step->tick!=633||r->rng!=478488862u||r->globals.voice_25d6!=8||r->row->globals.voice!=255){snprintf(r->divergence,sizeof r->divergence,"counter4828 voice reset input mismatch tick=%llu rng=%u voice=%u/%u",(unsigned long long)step->tick,r->rng,r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        (void)taxi_random_range(&r->rng,0,65535);
        if(r->rng!=1057641020u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4828 voice reset RNG mismatch %u",r->rng);r->diverged=1;return -1;}
        r->globals.voice_25d6=255;
        fprintf(stderr,"SOURCE_VOICE_RESET counter4828: call 0x080113EA with r5=0x020025D6 advances RNG 478488862->1057641020 and resets voice 8->255.\n");
    }
    if(r->source_exact_rng_probe&&(f->next==126||f->next==140)){
        unsigned long long expected_tick=f->next==126?628u:698u;
        uint32_t expected_rng=f->next==126?222467215u:1057641020u;
        uint8_t expected_input_voice=f->next==126?13u:255u;
        uint32_t expected_output_rng=f->next==126?478488862u:2014618744u;
        if(step->tick!=expected_tick||r->rng!=expected_rng||r->globals.voice_25d6!=expected_input_voice||r->row->globals.voice!=8){snprintf(r->divergence,sizeof r->divergence,"trip-running voice input mismatch row%u tick=%llu rng=%u voice=%u/%u",f->next,(unsigned long long)step->tick,r->rng,r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        uint8_t voice=(uint8_t)(taxi_random_range(&r->rng,0,65535)%3+6);
        if(voice!=8||r->rng!=expected_output_rng||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"trip-running voice result mismatch row%u voice=%u rng=%u",f->next,voice,r->rng);r->diverged=1;return -1;}
        r->globals.voice_25d6=voice;
        fprintf(stderr,"SOURCE_TRIP_RUNNING_VOICE tick=%llu: 0x0802A91C produces voice8 and the captured single RNG draw.\n",(unsigned long long)expected_tick);
    }
    if(r->source_exact_rng_probe&&f->next==103){
        if(step->tick!=500||r->row->schedule[9]!=4695||r->rng!=2046486665u||r->globals.voice_25d6!=255||r->row->globals.voice!=21){snprintf(r->divergence,sizeof r->divergence,"counter4695 voice-selection input mismatch tick=%llu counter=%d rng=%u voice=%u/%u",(unsigned long long)step->tick,r->row->schedule[9],r->rng,r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        uint8_t mode=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+2);
        uint8_t voice=(uint8_t)((taxi_random_range(&r->rng,0,65535)&1)+20);
        const uint8_t *owner=r->row->shared_owners+304;
        if(mode!=3||voice!=21||owner[6]!=voice||owner[7]!=mode||r->rng!=1264771047u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4695 voice-selection result mismatch mode=%u voice=%u rng=%u",mode,voice,r->rng);r->diverged=1;return -1;}
        r->globals.voice_25d6=voice;
        fprintf(stderr,"SOURCE_VOICE_SELECTION counter4695: calls 0x0800B19C/0x0800B1B4 reproduce mode3, voice21 and RNG 2046486665->1752365651->1264771047.\n");
    }
    if(r->source_exact_rng_probe&&f->next==105){
        if(step->tick!=516||r->rng!=1264771047u||r->globals.voice_25d6!=21||r->row->globals.voice!=13){snprintf(r->divergence,sizeof r->divergence,"counter4711 trip-start voice input mismatch tick=%llu rng=%u voice=%u/%u",(unsigned long long)step->tick,r->rng,r->globals.voice_25d6,r->row->globals.voice);r->diverged=1;return -1;}
        uint8_t voice=(uint8_t)(taxi_random_range(&r->rng,0,65535)%2+12);
        if(voice!=13||r->rng!=222467215u||r->rng!=r->row->globals.rng){snprintf(r->divergence,sizeof r->divergence,"counter4711 trip-start voice result mismatch voice=%u rng=%u",voice,r->rng);r->diverged=1;return -1;}
        r->globals.voice_25d6=voice;
        fprintf(stderr,"SOURCE_TRIP_START_VOICE counter4711: call 0x0800CCA0 with r5=0x020016AC produces voice13 and RNG 1264771047->222467215.\n");
    }
    unsigned source_row=(unsigned)(step-f->steps);
    if(r->source_exact_rng_probe&&source_row==85){
        if(!r->dispatcher_phase_checkpoint||r->phase_checkpoint_counter!=4611||r->globals.voice_25d6!=18||r->phase_checkpoint_globals.voice!=255||r->rng!=1899946468u){snprintf(r->divergence,sizeof r->divergence,"counter4610 voice/phase input mismatch current=%u expected=%u rng=%u checkpoint=%u",r->globals.voice_25d6,r->phase_checkpoint_globals.voice,r->rng,r->phase_checkpoint_globals.rng);r->diverged=1;return -1;}
        /* Source store at 0x08011446 resets the already captured prompt voice
           18->255 after counter4610. The phase checkpoint is source-derived. */
        r->globals.voice_25d6=r->phase_checkpoint_globals.voice;
        fprintf(stderr,"SOURCE_VOICE_RESET row=85 tick=415: captured counter4611 state confirms source voice 18->255; use the direct post-service actor/global checkpoint.\n");
    }
    if(source_voice_boundary(r,source_row,step)||source_voice_reset_boundary(r,source_row,step)||source_voice_selection_boundary(r,source_row,step)||source_trip_start_voice_boundary(r,step)||source_trip_running_voice_boundary(r,step)||source_trip_voice_mode_end_boundary(r,step)||source_trip_voice_mode_refresh_boundary(r,step)||source_external_selector_rng_boundary(r,step)||source_passenger_boundary(r,source_row,step)){r->diverged=1;return -1;}
    TaxiTrafficFrameHooks hooks={hook_follow,hook_postimpact,hook_common,r};
    int count=taxi_traffic_density(r->setting);
    int rc=dispatch_due?taxi_traffic_frame(r->actors,(size_t)count,(int32_t)step->frame,row->gates[0],row->camera[0],row->camera[2],r->grid,r->roads,taxi_traffic_spacing(r->setting),&r->rng,&r->counter,&r->globals.cooldown_e4,&r->timer,&hooks):0;
    if(rc<0){if(r->hook_failure[0])snprintf(r->divergence,sizeof r->divergence,"%s",r->hook_failure);else if(r->unsupported_postimpact)snprintf(r->divergence,sizeof r->divergence,"postimpact reached; preserve-query-Y owner is unresolved");else snprintf(r->divergence,sizeof r->divergence,"native traffic frame rejected its input (callbacks=%u/%u/%u grid=%p cells=%p roads=%p actor=%p)",r->follow_calls,r->postimpact_calls,r->common_calls,(void*)r->grid,(void*)r->grid->cells,(void*)r->roads,(void*)r->actors);r->diverged=1;return -1;}
    /* Match traffic_drive_update's recovered duplicate-field synchronization. */
    f->car.world.speed_544=f->car.driver.power.gears.speed_544;
    f->car.driver.motion.motion.orientation.speed_544=f->car.world.speed_544;
    f->car.world.height.scale_e84=f->car.driver.power.scale_e84;
    f->car.driver.motion.motion.scale_e84=f->car.driver.power.scale_e84;
    int phase_checkpoint_row=r->dispatcher_phase_checkpoint&&source_row==85;
    if(phase_checkpoint_row){
        if(row->schedule[9]!=4610||r->phase_checkpoint_counter!=4611){snprintf(r->divergence,sizeof r->divergence,"phase checkpoint alignment mismatch schedule_tick=%d checkpoint_counter=%u",row->schedule[9],r->phase_checkpoint_counter);r->diverged=1;return -1;}
        r->phase_checkpoint_rows++;
    }
    const TaxiTrafficActor *expected_actors=phase_checkpoint_row?r->phase_checkpoint_actors:row->expected;
    const Globals *expected_globals=phase_checkpoint_row?&r->phase_checkpoint_globals:&row->globals;
    for(unsigned i=0;i<ACTORS;i++)if(!actor_equal(r->actors+i,expected_actors+i)){
        const TaxiTrafficActor *a=r->actors+i,*e=expected_actors+i;
        if(r->dispatcher_phase_probe&&source_row==85&&i==8&&a->flags==1&&a->type==1&&a->y==19200&&e->flags==0&&e->type==0&&e->y==0&&actor_equal_except_spawn_phase(a,e)){
            fprintf(stderr,"SOURCE_DISPATCH_PHASE_DEFER row=85: native slot8 is post-dispatch active/Y19200 while the aligned ROM fixture is the pre-dispatch state; no actor or global state is reseeded, full comparison resumes at row86\n");
            r->deferred_phase_rows++;continue;
        }
        snprintf(r->divergence,sizeof r->divergence,"actor%u flags=%u/%u type=%u/%u countdown=%d/%d pos=%d,%d,%d/%d,%d,%d heading=%d/%d route=%u/%u collision=%d/%d impulse=%d,%d/%d,%d offset=%d,%d/%d,%d path=%d,%u,%d,%d,%u/%d,%u,%d,%d,%u",i,a->flags,e->flags,a->type,e->type,a->countdown,e->countdown,a->x,a->y,a->z,e->x,e->y,e->z,a->heading,e->heading,a->route,e->route,a->collision_timer,e->collision_timer,a->impulse_x,a->impulse_z,e->impulse_x,e->impulse_z,a->offset_x,a->offset_z,e->offset_x,e->offset_z,a->path.distance,(unsigned)a->path.segment,a->path.x,a->path.z,a->path.heading,e->path.distance,(unsigned)e->path.segment,e->path.x,e->path.z,e->path.heading);
        if(r->source_checkpoints){if(r->actor_mismatches<8)fprintf(stderr,"SOURCE_ACTOR_CHECKPOINT_RESEED row=%u %s; reseeding captured 50-slot pool for next row\n",(unsigned)(step-f->steps),r->divergence);r->actor_mismatches++;r->actor_checkpoint_rebases++;memcpy(r->actors,row->expected,sizeof row->expected);return 0;}
        r->diverged=1;return -1;}
    if(!traffic_globals_equal(r,expected_globals)){snprintf(r->divergence,sizeof r->divergence,
        "global mismatch cooldown=%d/%d near=%u/%u voice=%u/%u comboFlag=%u/%u comboA=%d/%d comboB=%d/%d timer=%u/%u rng=%u/%u counter=%u/%u callbacks=%u/%u/%u",
        r->globals.cooldown_e4,expected_globals->cooldown,r->globals.near_miss_25d3,expected_globals->near_miss,
        r->globals.voice_25d6,expected_globals->voice,r->globals.combo_flag_9aa6,expected_globals->combo_flag,
        r->globals.combo_9aa8,expected_globals->combo_a,r->globals.combo_9aac,expected_globals->combo_b,
        r->timer,expected_globals->timer,r->rng,expected_globals->rng,r->counter,expected_globals->counter,
        r->follow_calls,r->postimpact_calls,r->common_calls);r->diverged=1;return -1;}
    if(r->rng!=expected_globals->rng){
        if(!r->source_checkpoints){snprintf(r->divergence,sizeof r->divergence,"shared RNG mismatch native=%u source=%u (other traffic globals matched)",r->rng,row->globals.rng);r->diverged=1;return -1;}
        if(r->rng_boundary_rebases<8)fprintf(stderr,"SOURCE_RNG_BOUNDARY row=%u native_after_traffic=%u source_checkpoint=%u; next frame starts at the captured source checkpoint\n",(unsigned)(step-f->steps),r->rng,row->globals.rng);
        r->rng_boundary_rebases++;
        /* Source checkpoint may include an outer owner such as the audio
           scheduler. Use its observed post-row state as the next frame input;
           keep this explicitly separate from end-to-end RNG parity. */
        r->rng=row->globals.rng;
    }
    if(r->globals.voice_25d6!=row->globals.voice){
        if(!r->source_checkpoints){snprintf(r->divergence,sizeof r->divergence,"shared voice-owner mismatch native=%u source=%u (other traffic globals matched)",r->globals.voice_25d6,row->globals.voice);r->diverged=1;return -1;}
        if(r->voice_boundary_rebases<8)fprintf(stderr,"SOURCE_VOICE_BOUNDARY row=%u native_after_traffic=%u source_checkpoint=%u; next frame starts at the captured shared-owner checkpoint\n",(unsigned)(step-f->steps),r->globals.voice_25d6,row->globals.voice);
        r->voice_boundary_rebases++;
        r->globals.voice_25d6=row->globals.voice;
    }
    return 0;
}
static void free_roads(TaxiAiPathAsset roads[256]){for(unsigned i=0;i<256;i++)free(roads[i].points);}

int main(int argc,char **argv){
    int exact_rng_checkpoint=(argc==6||argc==7)&&!strcmp(argv[4],"--probe-exact-entry-4286-and-rng-2302e");
    int exact_dispatch_schedule=argc==7&&exact_rng_checkpoint;
    int dispatcher_phase_probe=argc==16&&!strcmp(argv[4],"--source-events")&&!strcmp(argv[15],"--dispatcher-phase-probe");
    int dispatcher_phase_checkpoint=argc==17&&!strcmp(argv[4],"--source-events")&&!strcmp(argv[15],"--dispatcher-phase-checkpoint");
    int dispatcher_phase_voice_reset=argc==20&&!strcmp(argv[4],"--source-events")&&!strcmp(argv[15],"--dispatcher-phase-checkpoint")&&!strcmp(argv[17],"--source-voice-reset");
    int dispatcher_entry_source_events_reset=argc==19&&!strcmp(argv[4],"--source-events")&&!strcmp(argv[15],"--source-voice-reset")&&!strcmp(argv[18],"--probe-max-speed-all");
    int dispatcher_entry_passenger_probe=argc==16&&!strcmp(argv[4],"--source-events")&&(!strcmp(argv[15],"--probe-max-speed")||!strcmp(argv[15],"--probe-max-speed-all"));
    int source_events=(argc==15&&!strcmp(argv[4],"--source-events"))||dispatcher_phase_probe||dispatcher_phase_checkpoint||dispatcher_phase_voice_reset||dispatcher_entry_passenger_probe||dispatcher_entry_source_events_reset;
    if((argc!=4&&argc!=5&&argc!=6&&argc!=7&&!source_events)||((strcmp(argv[3],"0")!=0)&&(strcmp(argv[3],"1")!=0))||(argc==5&&strcmp(argv[4],"--source-checkpoints")&&strcmp(argv[4],"--probe-max-speed")&&strcmp(argv[4],"--probe-exact-entry-4286")&&strcmp(argv[4],"--probe-exact-entry-4286-and-rng-2302e"))||((argc==6||argc==7)&&!exact_rng_checkpoint)||(argc==7&&!exact_dispatch_schedule))return 64;FILE *in=fopen(argv[1],"rb");if(!in)return 2;uint8_t header[20],gates[4];
    if(read_exact(in,header,sizeof header))return 3;int dispatch_entry_mode=!memcmp(header,"CTTFRP04",8);int event_mode=dispatch_entry_mode||!memcmp(header,"CTTFRP03",8);if(!event_mode&&memcmp(header,"CTTFRP02",8))return 3;unsigned row_count=u32(header+8);if(!row_count||u32(header+12)!=ACTORS||(!event_mode&&row_count!=ROWS))return 3;
    uint8_t setting=header[16];Globals initial;if(read_exact(in,gates,sizeof gates)||read_globals(in,&initial))return 3;
    Replay replay={0};replay.setting=setting;replay.preserve_query_y=argv[3][0]=='1';replay.dispatch_entry_mode=dispatch_entry_mode;replay.source_checkpoints=argc==5&&strcmp(argv[4],"--source-checkpoints")==0;replay.source_max_speed_probe=(argc==5&&strcmp(argv[4],"--probe-max-speed")==0)||(dispatcher_entry_passenger_probe&&!strcmp(argv[15],"--probe-max-speed"));replay.source_exact_entry_probe=argc==5&&!strcmp(argv[4],"--probe-exact-entry-4286");replay.source_exact_rng_probe=(argc==5||exact_rng_checkpoint)&&!strcmp(argv[4],"--probe-exact-entry-4286-and-rng-2302e");replay.source_max_speed_probe_all=(dispatcher_entry_passenger_probe&&!strcmp(argv[15],"--probe-max-speed-all"))||dispatcher_entry_source_events_reset;replay.source_passenger_events=source_events;replay.source_voice_event=source_events;replay.source_voice_reset_event=dispatcher_phase_voice_reset||dispatcher_entry_source_events_reset;replay.dispatcher_phase_probe=dispatcher_phase_probe||dispatch_entry_mode;replay.dispatcher_phase_checkpoint=dispatcher_phase_checkpoint||dispatcher_phase_voice_reset||exact_rng_checkpoint;replay.globals=(TaxiTrafficGlobals){.cooldown_e4=initial.cooldown,.near_miss_25d3=initial.near_miss,.voice_25d6=initial.voice,.combo_flag_9aa6=initial.combo_flag,.combo_9aa8=initial.combo_a,.combo_9aac=initial.combo_b};replay.timer=initial.timer;replay.rng=initial.rng;replay.counter=initial.counter;
    if(source_events){for(unsigned i=0;i<4;i++){replay.passenger_before[i]=read_ewram_checkpoint(argv[5+i*2]);replay.passenger_after[i]=read_ewram_checkpoint(argv[6+i*2]);if(!replay.passenger_before[i]||!replay.passenger_after[i]){fprintf(stderr,"cannot load source passenger boundary EWRAM files\n");return 12;}}replay.voice_before=read_ewram_checkpoint(argv[13]);replay.voice_after=read_ewram_checkpoint(argv[14]);if(!replay.voice_before||!replay.voice_after){fprintf(stderr,"cannot load source voice boundary EWRAM files\n");return 12;}}
    if(dispatcher_phase_voice_reset||dispatcher_entry_source_events_reset){unsigned before_arg=dispatcher_entry_source_events_reset?16:18,after_arg=dispatcher_entry_source_events_reset?17:19;replay.voice_reset_before=read_ewram_checkpoint(argv[before_arg]);replay.voice_reset_after=read_ewram_checkpoint(argv[after_arg]);if(!replay.voice_reset_before||!replay.voice_reset_after){fprintf(stderr,"cannot load source voice-reset EWRAM boundary\n");return 12;}}
    if((dispatcher_phase_checkpoint||dispatcher_phase_voice_reset||exact_rng_checkpoint)&&read_phase_checkpoint(exact_rng_checkpoint?argv[5]:argv[16],&replay.phase_checkpoint_counter,&replay.phase_checkpoint_globals,replay.phase_checkpoint_actors)){fprintf(stderr,"cannot load intermediate dispatcher checkpoint\n");return 13;}if(exact_dispatch_schedule&&read_dispatch_counters(argv[6],&replay)){fprintf(stderr,"cannot load source dispatcher counter schedule\n");return 14;}
    replay.actors=calloc(ACTORS,sizeof *replay.actors);if(!replay.actors||read_actors(in,replay.actors))return 4;
    TaxiRuntimeAsset runtime;TaxiWorldAsset world={0};TaxiTrafficGrid grid={0};TaxiAiPathAsset roads[256]={0};
    if(taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr")||taxi_world_asset_load(&world,"outputs/crazy-taxi-native/assets/city1.ctw")||taxi_world_asset_load_special(&world,"outputs/crazy-taxi-native/assets/city1-special.cts")||taxi_traffic_grid_load(&grid,"outputs/crazy-taxi-native/assets/traffic/city1-grid.ctg"))return 5;
    char path[256];for(unsigned i=1;i<256;i++){snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/traffic/city1-road-%03u.ctrp",i);FILE *test=fopen(path,"rb");if(test){fclose(test);if(taxi_traffic_path_load(roads+i,path))return 6;}}
    TaxiReferenceDriving driver={0};if(taxi_reference_driving_load(&driver,argv[2])||driver.count!=row_count)return 7;
    replay.driver=&driver;replay.runtime=&runtime;replay.world=&world;replay.grid=&grid;replay.roads=roads;replay.row=calloc(1,sizeof *replay.row);if(!replay.row)return 8;
    taxi_reference_driving_set_post_driver_hook(dispatch_after_driver,&replay);
    for(unsigned i=0;i<row_count;i++){
        if(read_row(in,replay.row,event_mode)){fprintf(stderr,"fixture truncated before row%u\n",i);return 9;}
        int rc;
        if(event_mode){
            TaxiVehicleStartPacked packed={0};memcpy(packed.actor,replay.row->player_actor,sizeof packed.actor);
            if(taxi_vehicle_start_driver_export(&driver.car.driver,&driver.context.driver,&packed,driver.shared.settings)||taxi_vehicle_start_world_export(&driver.car.world,&packed)){fprintf(stderr,"invalid packed source player at event%u\n",i);return 11;}
            driver.next=i;rc=dispatch_after_driver(&replay,&driver,driver.steps+i,&runtime.trig);
            if(!rc)driver.next=i+1;
        }else rc=taxi_reference_driving_step(&driver,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&world);
        if(rc){fprintf(stderr,"TRAFFIC_REPLAY_STOP row=%u tick=%llu reason=%s driver_state=%d,%d,%d speed=%d source=%d,%d,%d speed=%d\n",i,(unsigned long long)driver.steps[i].tick,replay.divergence[0]?replay.divergence:"driver reference mismatch",driver.car.world.height.position.x,driver.car.world.height.position.y,driver.car.world.height.position.z,driver.car.world.speed_544,driver.steps[i].expected.x,driver.steps[i].expected.y,driver.steps[i].expected.z,driver.steps[i].speed);break;}
    }
    unsigned matched=driver.next;taxi_reference_driving_set_post_driver_hook(NULL,NULL);
    printf("TRAFFIC_REPLAY matched=%u total=%u setting=%u active_slots=%u preserveY=%d fixture=%s boundary_mode=%s deferred_phase_rows=%u direct_phase_checkpoint_rows=%u rng_rebases=%u shared_voice_rebases=%u actor_mismatches=%u actor_pool_reseeds=%u source_dispatch_entries=%u/%u skipped_candidate_rows=%u stop=%s scope=%s\n",matched,row_count,setting,taxi_traffic_density(setting),replay.preserve_query_y,dispatch_entry_mode?"source_dispatch_entry_schedule":(event_mode?"vblank_actor_change_candidates":"route_schedule"),replay.source_voice_reset_event?"continuous_rng_source_events_with_voice_reset_and_selection_at_observed_rom_owners":(replay.dispatcher_phase_checkpoint?"continuous_rng_source_events_with_direct_tick415_checkpoint":(replay.dispatcher_phase_probe?"continuous_rng_source_boundaries_with_explicit_pre_dispatch_row85_defer":(replay.source_passenger_events?"continuous_rng_with_source_passenger_state_boundaries":(replay.source_exact_rng_probe?"source_counter4286_collision_plus_vblank150_rng_owner":(replay.source_exact_entry_probe?"single_source_capture_counter4286_exact_entry_probe":(replay.source_max_speed_probe?"single_row_maximum_550_forensic_probe":(replay.source_checkpoints?"source_checkpoints_after_each_row":"strict_continuous"))))))),replay.deferred_phase_rows,replay.phase_checkpoint_rows,replay.rng_boundary_rebases,replay.voice_boundary_rebases,replay.actor_mismatches,replay.actor_checkpoint_rebases,replay.dispatch_next,replay.dispatch_count,replay.dispatch_skipped,replay.divergence[0]?replay.divergence:"complete",dispatch_entry_mode?"offline_native_dispatch_entry_diagnostic_not_playable_port":(event_mode?"offline_vblank_candidate_checkpoint_replay_not_routine_entry_proof_not_playable_port":(replay.source_checkpoints?"offline_source_checkpointed_traffic_diagnostic_not_full_shared_owner_parity_not_playable_port":"offline_native_dispatch_diagnostic_not_playable_port")));
    taxi_reference_driving_free(&driver);taxi_world_asset_free(&world);taxi_traffic_grid_free(&grid);free_roads(roads);free(replay.actors);free(replay.row);free(replay.dispatch_counters);fclose(in);return replay.diverged?10:0;
}
