#include "traffic_actor.h"
#include "impact.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static uint32_t u32(const unsigned char *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int32_t s32(const unsigned char *p){uint32_t v=u32(p);int32_t r;memcpy(&r,&v,4);return r;}
static long number(const char *s){char *end=NULL;long v=strtol(s,&end,0);if(!s[0]||!end||*end){fprintf(stderr,"invalid integer: %s\n",s);exit(2);}return v;}
static int read_file(const char *path,unsigned char **out,size_t size){FILE *f=fopen(path,"rb");if(!f)return -1;unsigned char *p=malloc(size);if(!p){fclose(f);return -1;}size_t n=fread(p,1,size,f);int bad=n!=size||fgetc(f)!=EOF||ferror(f);fclose(f);if(bad){free(p);return -1;}*out=p;return 0;}
static int read_actor(const char *path,TaxiTrafficActor *a){unsigned char *p=NULL;if(read_file(path,&p,92))return -1;memset(a,0,sizeof *a);a->flags=u32(p);a->type=u32(p+4);a->countdown=s32(p+0x10);a->x=s32(p+0x14);a->y=s32(p+0x18);a->z=s32(p+0x1c);a->heading=s32(p+0x20);a->route=0;a->path.distance=s32(p+0x28);a->path.x=s32(p+0x34);a->path.z=s32(p+0x38);a->path.heading=(uint16_t)u32(p+0x3c);a->impulse_x=s32(p+0x40);a->offset_x=s32(p+0x44);a->impulse_z=s32(p+0x48);a->offset_z=s32(p+0x4c);a->collision_timer=s32(p+0x58);free(p);return 0;}
static int same_actor(const TaxiTrafficActor *a,const TaxiTrafficActor *b){return a->flags==b->flags&&a->type==b->type&&a->countdown==b->countdown&&a->x==b->x&&a->y==b->y&&a->z==b->z&&a->heading==b->heading&&a->route==b->route&&a->collision_timer==b->collision_timer&&a->impulse_x==b->impulse_x&&a->offset_x==b->offset_x&&a->impulse_z==b->impulse_z&&a->offset_z==b->offset_z&&a->path.distance==b->path.distance&&a->path.x==b->path.x&&a->path.z==b->path.z&&a->path.heading==b->path.heading;}
int main(int argc,char **argv){
 if(argc!=25){fprintf(stderr,"usage: probe before.actor after.actor road.ctp driving.ctr route px pz heading speed cooldown near voice comboFlag comboA comboB rng sound active delta expectedSpeed expectedCooldown expectedNear expectedVoice expectedRng\n");return 2;}
 TaxiTrafficActor actor={0},source_after={0};if(read_actor(argv[1],&actor)||read_actor(argv[2],&source_after)){fprintf(stderr,"could not read exact actor records\n");return 3;}uint32_t route=(uint32_t)number(argv[5]);actor.route=route;source_after.route=route;
 TaxiAiPathAsset road={0};if(taxi_traffic_path_load(&road,argv[3])){fprintf(stderr,"could not load traffic route\n");return 4;}
 unsigned char *bundle=NULL;if(read_file(argv[4],&bundle,34504)||memcmp(bundle,"CTR1",4)){fprintf(stderr,"could not read CTR1 driving tables\n");free(road.points);free(bundle);return 5;}TaxiTrig trig;taxi_trig_decode(&trig,bundle+4,bundle+8196);free(bundle);
 if(taxi_traffic_path_initialize(road.points,road.count,actor.path.distance,&actor.path)){fprintf(stderr,"could not initialize native route state\n");free(road.points);return 6;}
 int32_t px=(int32_t)number(argv[6]),pz=(int32_t)number(argv[7]),heading=(int32_t)number(argv[8]),speed=(int32_t)number(argv[9]);int index=(heading>>12)&4095;
 TaxiAcceleration player={0};player.gears.speed_544=speed;player.gears.scale_5cc=60;player.gears.ratios[0]=4096;player.gears.final_ratio=4096;player.gears.boost_75c=-1;
 TaxiTrafficGlobals globals={.cooldown_e4=(int16_t)number(argv[10]),.near_miss_25d3=(uint8_t)number(argv[11]),.voice_25d6=(uint8_t)number(argv[12]),.combo_flag_9aa6=(uint8_t)number(argv[13]),.combo_9aa8=(int32_t)number(argv[14]),.combo_9aac=(int32_t)number(argv[15])};
 uint32_t rng=(uint32_t)number(argv[16]);int sound=(int)number(argv[17]),active=(int)number(argv[18]),delta=(int)number(argv[19]);TaxiTrafficActorEffects effects={0};
 int rc=taxi_traffic_actor_follow(&actor,&road,&player,px,pz,trig.sine[index],trig.cosine[index],delta,delta,sound,active,&globals,&rng,&effects);
 int exact=rc==0&&effects.collision&&same_actor(&actor,&source_after)&&player.gears.speed_544==(int32_t)number(argv[20])&&globals.cooldown_e4==(int16_t)number(argv[21])&&globals.near_miss_25d3==(uint8_t)number(argv[22])&&globals.voice_25d6==(uint8_t)number(argv[23])&&rng==(uint32_t)number(argv[24]);
 printf("route=%u heading_index=%d taxi=(%d,%d) speed=%d->%d source=%ld collision=%d rng=%u source_rng=%ld cooldown=%d source_cooldown=%ld voice=%u source_voice=%ld actor_exact=%s event_match=%s\n",route,index,px,pz,speed,player.gears.speed_544,number(argv[20]),effects.collision,rng,number(argv[24]),globals.cooldown_e4,number(argv[21]),globals.voice_25d6,number(argv[23]),same_actor(&actor,&source_after)?"yes":"no",exact?"yes":"no");
 free(road.points);return exact?0:7;
}
