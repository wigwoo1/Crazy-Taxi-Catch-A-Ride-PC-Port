/* Isolated counter5294 source-input replay with ROM-derived gear parameters.
   This checks one recovered traffic callback, not a full gameplay frame. */
#include "../src/traffic_actor.h"
#include "../src/traffic_path.h"
#include "../src/impact.h"
#include "../src/acceleration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { PLAYER_BYTES = 0xEA0, ACTOR_BYTES = 92 };
static uint32_t u32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t s32(const uint8_t *p){uint32_t u=u32(p);int32_t v;memcpy(&v,&u,4);return v;}
static int32_t number(const char *s){char *end=NULL;long v=strtol(s,&end,0);if(!s[0]||!end||*end||v<INT32_MIN||v>INT32_MAX){fprintf(stderr,"invalid integer: %s\n",s);exit(2);}return (int32_t)v;}
static uint8_t *read_exact_file(const char *path,size_t size){FILE *f=fopen(path,"rb");if(!f)return NULL;uint8_t *p=malloc(size);if(!p){fclose(f);return NULL;}int bad=fread(p,1,size,f)!=size||fgetc(f)!=EOF||ferror(f);fclose(f);if(bad){free(p);return NULL;}return p;}
static int read_actor(const uint8_t *p,TaxiTrafficActor *a,uint32_t route){
    if(!p||!a)return -1;memset(a,0,sizeof *a);
    a->flags=u32(p);a->type=u32(p+4);a->countdown=s32(p+0x10);a->x=s32(p+0x14);a->y=s32(p+0x18);a->z=s32(p+0x1c);a->heading=s32(p+0x20);a->route=route;
    a->path.distance=s32(p+0x28);a->path.segment=(size_t)u32(p+0x2c);a->path.x=s32(p+0x34);a->path.z=s32(p+0x38);a->path.heading=(uint16_t)u32(p+0x3c);
    a->impulse_x=s32(p+0x40);a->offset_x=s32(p+0x44);a->impulse_z=s32(p+0x48);a->offset_z=s32(p+0x4c);a->collision_timer=s32(p+0x58);return 0;
}
static int same_actor(const TaxiTrafficActor *a,const TaxiTrafficActor *b){
    return a->flags==b->flags&&a->type==b->type&&a->countdown==b->countdown&&a->x==b->x&&a->y==b->y&&a->z==b->z&&a->heading==b->heading&&a->route==b->route&&a->collision_timer==b->collision_timer&&a->impulse_x==b->impulse_x&&a->impulse_z==b->impulse_z&&a->offset_x==b->offset_x&&a->offset_z==b->offset_z&&a->path.distance==b->path.distance&&a->path.segment==b->path.segment&&a->path.x==b->path.x&&a->path.z==b->path.z&&a->path.heading==b->path.heading;
}
int main(int argc,char **argv){
    if(argc!=14){fprintf(stderr,"usage: before.actor after.actor road.ctrp driving.ctr pre-player.actor post-player.actor pre-ewram.bin post-globals.bin x z heading speed delta\n");return 2;}
    uint8_t *before=read_exact_file(argv[1],ACTOR_BYTES),*after=read_exact_file(argv[2],ACTOR_BYTES),*pre=read_exact_file(argv[5],PLAYER_BYTES),*post=read_exact_file(argv[6],PLAYER_BYTES),*ewram=read_exact_file(argv[7],0x40000),*globals_after=read_exact_file(argv[8],22),*driving=read_exact_file(argv[4],34504);
    if(!before||!after||!pre||!post||!ewram||!globals_after||!driving||memcmp(driving,"CTR1",4)){fprintf(stderr,"could not read exact source fixtures\n");return 3;}
    TaxiTrafficActor actor={0},source_after={0};if(read_actor(before,&actor,134)||read_actor(after,&source_after,134))return 4;
    TaxiAiPathAsset road={0};if(taxi_traffic_path_load(&road,argv[3])||taxi_traffic_path_initialize(road.points,road.count,actor.path.distance,&actor.path))return 5;
    TaxiTrig trig;taxi_trig_decode(&trig,driving+4,driving+8196);
    TaxiAcceleration player={0};
    player.gears.flags_4ac=pre[0x4ac];player.gears.ratios[0]=s32(pre+0x4b8);player.gears.final_ratio=s32(pre+0x4d8);
    player.gears.scale_5b8=s32(pre+0x5b8);player.gears.scale_5cc=s32(pre+0x5cc);player.gears.boost_75c=s32(pre+0x75c);player.gears.speed_544=number(argv[12]);
    player.gears.gear=(int8_t)pre[0x4dc];player.gears.rpm_538=(int16_t)(pre[0x538]|((uint16_t)pre[0x539]<<8));
    player.gears.timer_e6c=s32(pre+0xe6c);player.gears.timer_e70=s32(pre+0xe70);player.gears.timer_e78=s32(pre+0xe78);player.gears.timer_e7c=s32(pre+0xe7c);player.gears.timer_e80=s32(pre+0xe80);
    TaxiTrafficGlobals globals={.cooldown_e4=(int16_t)(ewram[0xe4]|((uint16_t)ewram[0xe5]<<8)),.near_miss_25d3=ewram[0x25d3],.voice_25d6=ewram[0x25d6],.combo_flag_9aa6=ewram[0x9aa6],.combo_9aa8=s32(ewram+0x9aa8),.combo_9aac=s32(ewram+0x9aac)};
    uint32_t rng=u32(ewram+0x3b04);TaxiTrafficActorEffects effects={0};
    int32_t heading=number(argv[11]),index=(heading>>12)&4095,delta=number(argv[13]);
    int rc=taxi_traffic_actor_follow(&actor,&road,&player,number(argv[9]),number(argv[10]),trig.sine[index],trig.cosine[index],delta,delta,1,1,&globals,&rng,&effects);
    int actor_match=same_actor(&actor,&source_after);
    int power_match=player.gears.speed_544==s32(post+0x544)&&player.gears.gear==(int8_t)post[0x4dc]&&player.gears.rpm_538==(int16_t)(post[0x538]|((uint16_t)post[0x539]<<8))&&player.scale_e84==s32(post+0xe84)&&player.gears.boost_75c==s32(post+0x75c)&&!memcmp(&player.gears.timer_e6c,post+0xe6c,8)&&!memcmp(&player.gears.timer_e78,post+0xe78,12)&&s32(pre+0xe74)==s32(post+0xe74);
    int globals_match=globals.cooldown_e4==(int16_t)(globals_after[0]|((uint16_t)globals_after[1]<<8))&&globals.near_miss_25d3==globals_after[2]&&globals.voice_25d6==globals_after[3]&&globals.combo_flag_9aa6==globals_after[4]&&globals.combo_9aa8==s32(globals_after+6)&&globals.combo_9aac==s32(globals_after+10)&&rng==u32(globals_after+14);
    printf("SOURCE_POWERTRAIN_EVENT route=134 collision=%d actor_exact=%s powertrain_exact=%s globals_exact=%s speed=%d rpm=%d source_speed=%d source_rpm=%d ratios0=%d final=%d scales=%d/%d\n",effects.collision,actor_match?"yes":"no",power_match?"yes":"no",globals_match?"yes":"no",player.gears.speed_544,player.gears.rpm_538,s32(post+0x544),(int16_t)(post[0x538]|((uint16_t)post[0x539]<<8)),player.gears.ratios[0],player.gears.final_ratio,player.gears.scale_5b8,player.gears.scale_5cc);
    free(before);free(after);free(pre);free(post);free(ewram);free(globals_after);free(driving);free(road.points);
    return !rc&&effects.collision&&actor_match&&power_match&&globals_match?0:6;
}
