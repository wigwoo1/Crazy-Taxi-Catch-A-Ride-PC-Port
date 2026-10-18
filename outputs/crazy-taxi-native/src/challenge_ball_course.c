#include "challenge_ball_course.h"
#include <stdio.h>
#include <string.h>
static int path(char out[512],const char *root,const char *suffix){
 int n=snprintf(out,512,"%s/%s",root,suffix);return n<0||n>=512?-1:0;
}
static int read_exact(const char *name,void *data,size_t size){FILE *f=fopen(name,"rb");if(!f)return -1;int rc=fread(data,1,size,f)==size&&fgetc(f)==EOF&&!ferror(f)?0:-1;fclose(f);return rc;}
void taxi_challenge_ball_course_free(TaxiChallengeBallCourse *c){
 if(!c)return;
 taxi_challenge_assets_free(&c->resources);taxi_passenger_asset_free(&c->actors);memset(c,0,sizeof *c);
}
int taxi_challenge_ball_course_load(TaxiChallengeBallCourse *out,const char *root){
 if(!out||!root)return -1;
 TaxiChallengeBallCourse c={0};char name[512];
 if(path(name,root,"challenge-worlds")||taxi_challenge_assets_load(&c.resources,name,2))goto invalid;
 if(path(name,root,"challenge-worlds/challenge2-actors.ctp")||taxi_passenger_asset_load(&c.actors,name))goto invalid;
 if(path(name,root,"challenge-ball/balloon.indices")||read_exact(name,c.balloon_indices,8192))goto invalid;
 if(c.resources.world.nv!=510||c.resources.world.np!=217||c.resources.world.nm!=75||c.resources.textures.count!=45||c.actors.count!=20)goto invalid;
 taxi_challenge_ball_course_free(out);*out=c;return 0;
invalid:
 taxi_challenge_ball_course_free(&c);return -1;
}

int taxi_challenge_ball_course_start(TaxiChallengeBallCourse *course,TaxiDriveWorld *vehicle,TaxiDriveWorldContext *context,const uint8_t *vehicle_template,size_t size,uint8_t settings){
 if(!course||!course->resources.world.storage||course->resources.submode!=2)return -1;
 return taxi_simulation_start_challenge(vehicle,context,vehicle_template,size,&course->resources.spawn,settings,2,taxi_world_asset_ground,&course->resources.world);
}

static int32_t actor_word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
int taxi_challenge_ball_course_actors(TaxiChallengeBallCourse *course,TaxiPassengerState actors[20]){
 if(!course||!actors||!course->resources.world.storage||course->resources.submode!=2||course->actors.count!=20||!course->actors.source)return -1;
 TaxiPassengerState next[20];memcpy(next,actors,sizeof next);
 for(unsigned i=0;i<20;i++){
  const uint8_t *source=course->actors.source[i];if(source[3]>9)return -1;
  TaxiPoint query={actor_word(source+4),actor_word(actors[i].bytes+20),actor_word(source+12)};int32_t height;
  int rc=taxi_world_asset_height(&course->resources.world,query,&height);if(rc!=1)return rc==0?0:-1;
  TaxiPassengerResolved resolved={.kind=source[3],.origin={query.x,query.z},.spawn_height=height};
  /* Source mode2 bypasses22C5A600-hold branch, sharing Flag writes. */
  if(taxi_passenger_init_flag(next+i,&resolved))return -1;
 }
 memcpy(actors,next,sizeof next);return 1;
}

#include "integer_root.h"
int taxi_challenge_ball_course_control(const TaxiChallengeBallCourse *c,
 TaxiChallengeBall *state,TaxiPassengerState actors[20],const TaxiDriveWorld *vehicle,
 TaxiBallInput gates,TaxiBallStatus *status,TaxiBallEffects *effects,const TaxiTrig *trig){
 if(!c||!state||!actors||!vehicle||!status||!effects||!trig||
    !c->resources.world.storage||c->resources.submode!=2||c->actors.count!=20||!c->actors.source)return -1;
 /* Explicit copies preserve the caller's44-byte record ownership without
    aliasing an array of structs as a different array type. */
 uint8_t records[20][44];
 for(unsigned i=0;i<20;i++)memcpy(records[i],actors[i].bytes,44);
 gates.car_x=vehicle->world.height.position.x;gates.car_z=vehicle->world.height.position.z;
 int rc=taxi_challenge_ball_step(state,records,&gates,status,effects,
                              trig->sine,trig->cosine,taxi_integer_root);
 if(!rc)for(unsigned i=0;i<20;i++)memcpy(actors[i].bytes,records[i],44);
 return rc;
}

int taxi_challenge_ball_course_render(TaxiChallengeBallCourse *c,
 TaxiPassengerState actors[20],TaxiPoint player,TaxiPoint camera,int32_t yaw,int32_t pitch,
 int32_t horizontal,int32_t vertical,int pause,int32_t delta,uint32_t factor,
 const TaxiTrig *trig,TaxiBallProjection projections[20],uint32_t *visible){
 if(!c||!actors||!trig||!projections||!visible||!c->resources.world.storage||
    c->resources.submode!=2||c->actors.count!=20||!c->actors.source)return -1;
 TaxiPassengerState next[20];memcpy(next,actors,sizeof next);
 TaxiBallProjection prepared[20];memcpy(prepared,projections,sizeof prepared);uint32_t mask=0;
 for(unsigned i=0;i<20;i++){
  uint8_t probe[44];memcpy(probe,next[i].bytes,44);TaxiBallRenderRequest request;
  /* Source distance/kind/flags cull precedes29B44 ground and animation. */
  int rc=taxi_challenge_ball_render_request(probe,player.x,player.z,0,1,0,factor,&request);
  if(rc<0)return -1;if(!rc)continue;
  TaxiPoint query={actor_word(next[i].bytes+16),actor_word(next[i].bytes+20),actor_word(next[i].bytes+24)};
  int32_t ground;rc=taxi_world_asset_height(&c->resources.world,query,&ground);
  if(rc<0)return -1;if(!rc)continue;
  rc=taxi_challenge_ball_render_request(next[i].bytes,player.x,player.z,ground,pause,delta,factor,&request);
  if(rc<0)return -1;if(!rc)continue;
  rc=taxi_challenge_ball_project(&request,camera,yaw,pitch,horizontal,vertical,trig,prepared+i);
  if(rc<0)return -1;if(rc)mask|=1u<<i;
 }
 memcpy(actors,next,sizeof next);memcpy(projections,prepared,sizeof prepared);*visible=mask;return 0;
}
