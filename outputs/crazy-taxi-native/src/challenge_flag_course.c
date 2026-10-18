#include "challenge_flag_course.h"
#include <stdio.h>
#include <string.h>
static int path(char out[512],const char *root,const char *suffix){
 int n=snprintf(out,512,"%s/%s",root,suffix);return n<0||n>=512?-1:0;
}
static int read_exact(const char *name,void *data,size_t size){
 FILE *f=fopen(name,"rb");if(!f)return -1;
 int result=fread(data,1,size,f)==size&&fgetc(f)==EOF&&!ferror(f)?0:-1;
 fclose(f);return result;
}
void taxi_challenge_flag_course_free(TaxiChallengeFlagCourse *c){
 if(!c)return;
 taxi_challenge_assets_free(&c->resources);taxi_passenger_asset_free(&c->actors);memset(c,0,sizeof *c);
}
int taxi_challenge_flag_course_load(TaxiChallengeFlagCourse *out,const char *root){
 if(!out||!root)return -1;
 TaxiChallengeFlagCourse c={0};char name[512];
 if(path(name,root,"challenge-worlds")||taxi_challenge_assets_load(&c.resources,name,1))goto invalid;
 if(path(name,root,"challenge-worlds/challenge1-actors.ctp")||taxi_passenger_asset_load(&c.actors,name))goto invalid;
 if(path(name,root,"challenge-flag/flag.indices")||read_exact(name,c.flag_indices,2048))goto invalid;
 if(c.resources.world.nv!=403||c.resources.world.np!=225||c.resources.world.nm!=75||c.resources.textures.count!=45||c.actors.count!=1||c.actors.source[0][3]!=0)goto invalid;
 taxi_challenge_flag_course_free(out);*out=c;return 0;
invalid:
 taxi_challenge_flag_course_free(&c);return -1;
}

int taxi_challenge_flag_course_start(TaxiChallengeFlagCourse *course,TaxiDriveWorld *vehicle,TaxiDriveWorldContext *context,const uint8_t *vehicle_template,size_t size,uint8_t settings){
 if(!course||!course->resources.world.storage||course->resources.submode!=1)return -1;
 return taxi_simulation_start_challenge(vehicle,context,vehicle_template,size,&course->resources.spawn,settings,1,taxi_world_asset_ground,&course->resources.world);
}

static int32_t actor_word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
int taxi_challenge_flag_course_actor(TaxiChallengeFlagCourse *course,TaxiPassengerState *actor){
 if(!course||!actor||!course->resources.world.storage||course->resources.submode!=1||course->actors.count!=1||!course->actors.source)return -1;
 const uint8_t *source=course->actors.source[0];if(source[3]>9)return -1;
 TaxiPoint query={actor_word(source+4),actor_word(actor->bytes+20),actor_word(source+12)};int32_t height;
 int rc=taxi_world_asset_height(&course->resources.world,query,&height);if(rc!=1)return rc==0?0:-1;
 TaxiPassengerResolved resolved={.kind=source[3],.origin={query.x,query.z},.spawn_height=height};
 TaxiPassengerState next=*actor;if(taxi_passenger_init_flag(&next,&resolved))return -1;
 *actor=next;return 1;
}

int taxi_challenge_flag_course_control(const TaxiChallengeFlagCourse *course,TaxiPassengerState *actor,const TaxiDriveWorld *vehicle,TaxiChallengeFlag *timer,const TaxiChallengeFlagInput *gates,TaxiChallengeFlagStatus *status,TaxiChallengeFlagEffects *effects){
 if(!course||!actor||!vehicle||!timer||!gates||!status||!effects||!course->resources.world.storage||course->resources.submode!=1||course->actors.count!=1)return -1;
 TaxiChallengeFlagInput input=*gates;
 input.car_x=vehicle->world.height.position.x;input.car_z=vehicle->world.height.position.z;
 input.flag_x=actor_word(actor->bytes+16);input.flag_z=actor_word(actor->bytes+24);
 taxi_challenge_flag_step(timer,&input,status,effects);
 if(effects->hide_flag)actor->bytes[0]=(uint8_t)((actor->bytes[0]&~1u)|2u);
 return 0;
}
