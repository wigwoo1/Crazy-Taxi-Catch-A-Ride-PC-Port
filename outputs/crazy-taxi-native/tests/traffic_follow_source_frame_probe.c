/* Source-ordered callback-window replay, including the 5295-labeled tail. */
#include "../src/traffic_actor.h"
#include "../src/traffic_path.h"
#include "../src/impact.h"
#include "../src/acceleration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { PLAYER_BYTES=0xEA0, ACTOR_BYTES=92, ROW_BYTES=256 };
typedef struct {int32_t slot,route,x,y,z,heading,speed_in,speed_out,delta,cool_in,cool_out,near_in,near_out;uint32_t rng_in,rng_out;int32_t sound,active,collision;uint8_t before[ACTOR_BYTES],after[ACTOR_BYTES];} Event;
static uint32_t u32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t s32(const uint8_t *p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static uint16_t u16(const uint8_t *p){return (uint16_t)p[0]|(uint16_t)p[1]<<8;}
static int32_t arg(const char *s){char *end=NULL;long v=strtol(s,&end,0);if(!s[0]||!end||*end||v<INT32_MIN||v>INT32_MAX){fprintf(stderr,"invalid integer: %s\n",s);exit(2);}return (int32_t)v;}
static int exact_file(const char *path,void *data,size_t n){FILE *f=fopen(path,"rb");if(!f)return -1;int bad=fread(data,1,n,f)!=n||fgetc(f)!=EOF||ferror(f);fclose(f);return bad?-1:0;}
static void decode_actor(const uint8_t *p,TaxiTrafficActor *a,uint32_t route){memset(a,0,sizeof *a);a->flags=u32(p);a->type=u32(p+4);a->countdown=s32(p+0x10);a->x=s32(p+0x14);a->y=s32(p+0x18);a->z=s32(p+0x1c);a->heading=s32(p+0x20);a->route=route;a->path.distance=s32(p+0x28);a->path.segment=(size_t)u32(p+0x2c);a->path.x=s32(p+0x34);a->path.z=s32(p+0x38);a->path.heading=u16(p+0x3c);a->impulse_x=s32(p+0x40);a->offset_x=s32(p+0x44);a->impulse_z=s32(p+0x48);a->offset_z=s32(p+0x4c);a->collision_timer=s32(p+0x58);}
static int same_actor(const TaxiTrafficActor *a,const TaxiTrafficActor *b){return a->flags==b->flags&&a->type==b->type&&a->countdown==b->countdown&&a->x==b->x&&a->y==b->y&&a->z==b->z&&a->heading==b->heading&&a->route==b->route&&a->collision_timer==b->collision_timer&&a->impulse_x==b->impulse_x&&a->impulse_z==b->impulse_z&&a->offset_x==b->offset_x&&a->offset_z==b->offset_z&&a->path.distance==b->path.distance&&a->path.segment==b->path.segment&&a->path.x==b->path.x&&a->path.z==b->path.z&&a->path.heading==b->path.heading;}
int main(int argc,char **argv){
    if(argc!=6){fprintf(stderr,"usage: counter5294-events.bin pre-player.actor pre-ewram.bin post-player.actor post-globals.bin\n");return 2;}
    FILE *f=fopen(argv[1],"rb");if(!f)return 3;uint8_t head[12];if(fread(head,1,12,f)!=12||memcmp(head,"CT5294V1",8)){fclose(f);return 4;}uint32_t count=u32(head+8);if(count!=9){fclose(f);return 5;}
    Event events[9];memset(events,0,sizeof events);
    for(unsigned i=0;i<count;i++){
        uint8_t row[ROW_BYTES];if(fread(row,1,sizeof row,f)!=sizeof row){fclose(f);return 6;}
        int32_t *v=(int32_t *)&events[i];for(unsigned k=0;k<18;k++)v[k]=s32(row+4*k);
        memcpy(events[i].before,row+72,ACTOR_BYTES);memcpy(events[i].after,row+72+ACTOR_BYTES,ACTOR_BYTES);
    }
    if(fgetc(f)!=EOF){fclose(f);return 7;}fclose(f);
    uint8_t *pre=malloc(PLAYER_BYTES),*post=malloc(PLAYER_BYTES),*ewram=malloc(0x40000),expected[22],driving[34504];
    if(!pre||!post||!ewram||exact_file(argv[2],pre,PLAYER_BYTES)||exact_file(argv[3],ewram,0x40000)||exact_file(argv[4],post,PLAYER_BYTES)||exact_file(argv[5],expected,sizeof expected)){fprintf(stderr,"source snapshot file missing or malformed\n");return 8;}
    if(exact_file("outputs/crazy-taxi-native/assets/driving.ctr",driving,sizeof driving)||memcmp(driving,"CTR1",4))return 9;
    TaxiTrig trig;taxi_trig_decode(&trig,driving+4,driving+8196);
    TaxiAcceleration player={0};player.gears.flags_4ac=pre[0x4ac];player.gears.ratios[0]=s32(pre+0x4b8);player.gears.final_ratio=s32(pre+0x4d8);player.gears.scale_5b8=s32(pre+0x5b8);player.gears.scale_5cc=s32(pre+0x5cc);player.gears.boost_75c=s32(pre+0x75c);player.gears.gear=(int8_t)pre[0x4dc];player.gears.rpm_538=(int16_t)u16(pre+0x538);player.gears.speed_544=events[0].speed_in;
    TaxiTrafficGlobals g={.cooldown_e4=(int16_t)u16(ewram+0xe4),.near_miss_25d3=ewram[0x25d3],.voice_25d6=ewram[0x25d6],.combo_flag_9aa6=ewram[0x9aa6],.combo_9aa8=s32(ewram+0x9aa8),.combo_9aac=s32(ewram+0x9aac)};uint32_t rng=u32(ewram+0x3b04);
    unsigned collisions=0;
    for(unsigned i=0;i<count;i++){
        Event *e=events+i;if(player.gears.speed_544!=e->speed_in||g.cooldown_e4!=e->cool_in||g.near_miss_25d3!=e->near_in||rng!=e->rng_in){fprintf(stderr,"state chain diverged before call%u\n",i);return 10;}
        TaxiTrafficActor actor={0},source_after={0};decode_actor(e->before,&actor,(uint32_t)e->route);decode_actor(e->after,&source_after,(uint32_t)e->route);
        char path[256];if(snprintf(path,sizeof path,"outputs/crazy-taxi-native/assets/traffic/city1-road-%03ld.ctrp",(long)e->route)>=(int)sizeof path)return 11;
        TaxiAiPathAsset road={0};if(taxi_traffic_path_load(&road,path)||taxi_traffic_path_initialize(road.points,road.count,actor.path.distance,&actor.path))return 12;
        int index=(e->heading>>12)&4095;TaxiTrafficActorEffects effects={0};
        int rc=taxi_traffic_actor_follow(&actor,&road,&player,e->x,e->z,trig.sine[index],trig.cosine[index],e->delta,e->delta,e->sound,e->active,&g,&rng,&effects);
        int actor_ok=same_actor(&actor,&source_after);free(road.points);
        if(rc||!actor_ok||effects.collision!=e->collision||player.gears.speed_544!=e->speed_out||g.cooldown_e4!=e->cool_out||g.near_miss_25d3!=e->near_out||rng!=e->rng_out){fprintf(stderr,"source callback mismatch at order%u slot%ld route%ld\n",i,(long)e->slot,(long)e->route);return 13;}
        collisions+=(unsigned)effects.collision;
    }
    int player_ok=player.gears.speed_544==s32(post+0x544)&&player.gears.gear==(int8_t)post[0x4dc]&&player.gears.rpm_538==(int16_t)u16(post+0x538)&&player.scale_e84==s32(post+0xe84)&&player.gears.boost_75c==s32(post+0x75c)&&!memcmp(&player.gears.timer_e6c,post+0xe6c,8)&&!memcmp(&player.gears.timer_e78,post+0xe78,12)&&s32(pre+0xe74)==s32(post+0xe74);
    int globals_ok=g.cooldown_e4==(int16_t)u16(expected)&&g.near_miss_25d3==expected[2]&&g.voice_25d6==expected[3]&&g.combo_flag_9aa6==expected[4]&&g.combo_9aa8==s32(expected+6)&&g.combo_9aac==s32(expected+10)&&rng==u32(expected+14);
    printf("SOURCE_CALLBACK_WINDOW_REPLAY callbacks=%u collisions=%u actor_returns_exact=yes powertrain_exact=%s globals_exact=%s speed=%d rpm=%d rng=%u\n",count,collisions,player_ok?"yes":"no",globals_ok?"yes":"no",player.gears.speed_544,player.gears.rpm_538,rng);
    free(pre);free(post);free(ewram);return player_ok&&globals_ok?0:14;
}

