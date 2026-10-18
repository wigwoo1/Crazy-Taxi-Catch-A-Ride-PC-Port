#include "challenge_bound_course.h"
#include <stdio.h>
#include <string.h>
void taxi_challenge_bound_course_free(TaxiChallengeBoundCourse *c){if(!c)return;taxi_challenge_assets_free(&c->resources);taxi_passenger_asset_free(&c->actors);memset(c,0,sizeof *c);}
int taxi_challenge_bound_course_load(TaxiChallengeBoundCourse *out,const char *root){
 if(!out||!root)return -1;
 TaxiChallengeBoundCourse c={0};char path[1024];int n=snprintf(path,sizeof path,"%s/challenge-worlds",root);
 if(n<0||(size_t)n>=sizeof path||taxi_challenge_assets_load(&c.resources,path,5))goto bad;
 n=snprintf(path,sizeof path,"%s/challenge-worlds/challenge5-actors.ctp",root);
 if(n<0||(size_t)n>=sizeof path||taxi_passenger_asset_load(&c.actors,path))goto bad;
 if(c.resources.world.nv!=807||c.resources.world.np!=737||c.resources.world.nm!=75||c.resources.textures.count!=45||c.actors.count!=1||c.actors.route_count!=1||c.actors.destination_count!=1||c.actors.source[0][3]!=5||c.actors.source[0][1]!=0)goto bad;
 taxi_challenge_bound_course_free(out);*out=c;return 0;
 bad:taxi_challenge_bound_course_free(&c);return -1;
}
static int32_t actor_word(const uint8_t *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t v;memcpy(&v,&u,4);return v;}
int taxi_challenge_bound_course_actor(TaxiChallengeBoundCourse *c,TaxiPassengerState *out){
 if(!c||!out||!c->resources.world.storage||c->resources.submode!=5||c->actors.count!=1)return -1;
 const uint8_t *source=c->actors.source[0];TaxiPassengerResolved r={.kind=source[3],.origin={actor_word(source+4),actor_word(source+12)}};
 int rc=taxi_world_asset_height(&c->resources.world,(TaxiPoint){r.origin.x,actor_word(out->bytes+20),r.origin.z},&r.spawn_height);
 if(rc<0)return -1;
 /* Original29B44 no hit returns INTMIN, caller stores it as height. */
 return taxi_passenger_init_challenge(out,&r,5);
}
int taxi_challenge_bound_course_start(TaxiChallengeBoundCourse *c,TaxiDriveWorld *car,TaxiDriveWorldContext *ctx,TaxiChallengeBound *timer,uint8_t *option,const uint8_t *bytes,size_t size,uint8_t settings){
 if(!c||!car||!ctx||!timer||!option||!c->resources.world.storage||c->resources.submode!=5)return -1;
 TaxiDriveWorld v=*car;TaxiDriveWorldContext context=*ctx;TaxiChallengeBound t=*timer;uint8_t value=*option;
 int rc=taxi_simulation_start_challenge(&v,&context,bytes,size,&c->resources.spawn,settings,5,taxi_world_asset_ground,&c->resources.world);if(rc!=1)return rc;
 if(taxi_challenge_bound_begin(&t,&value))return -1;
 *car=v;*ctx=context;*timer=t;*option=value;return 1;
}
int taxi_challenge_bound_course_control(const TaxiChallengeBoundCourse *c,TaxiDriveWorldContext *ctx,TaxiChallengeBound *timer,TaxiChallengeBoundInput gates,TaxiChallengeBoundStatus *status,TaxiChallengeBoundEffects *effects){
 if(!c||!ctx||!timer||!status||!effects||!c->resources.world.storage||c->resources.submode!=5||ctx->driver.motion.orientation.submode!=5||(ctx->driver.motion.orientation.settings&224)!=128)return -1;
 taxi_challenge_bound_step(timer,&gates,status,effects);
 ctx->height.flag_1697=status->ended1697;ctx->height.flag_153c=status->success153c;ctx->driver.motion.orientation.flag_1697=status->ended1697;return 0;
}
