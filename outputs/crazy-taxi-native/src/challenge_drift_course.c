#include "challenge_drift_course.h"
#include <stdio.h>
void taxi_challenge_drift_course_free(TaxiChallengeDriftCourse *c){if(c)taxi_challenge_assets_free(&c->resources);}
int taxi_challenge_drift_course_load(TaxiChallengeDriftCourse *out,const char *root){
 if(!out||!root)return -1;
 TaxiChallengeDriftCourse next={0};char path[1024];int n=snprintf(path,sizeof path,"%s/challenge-worlds",root);
 if(n<0||(size_t)n>=sizeof path||taxi_challenge_assets_load(&next.resources,path,3))return -1;
 if(next.resources.world.nv!=336||next.resources.world.np!=258||next.resources.world.nm!=75||next.resources.textures.count!=45){taxi_challenge_drift_course_free(&next);return -1;}
 taxi_challenge_drift_course_free(out);*out=next;return 0;
}
int taxi_challenge_drift_course_start(TaxiChallengeDriftCourse *c,TaxiDriveWorld *car,TaxiDriveWorldContext *ctx,TaxiChallengeDrift *timer,uint8_t *option,const uint8_t *bytes,size_t size,uint8_t settings){
 if(!c||!car||!ctx||!timer||!option||!c->resources.world.storage||c->resources.submode!=3)return -1;
 TaxiDriveWorld v=*car;TaxiDriveWorldContext context=*ctx;TaxiChallengeDrift state=*timer;uint8_t value=*option;
 int rc=taxi_simulation_start_challenge(&v,&context,bytes,size,&c->resources.spawn,settings,3,taxi_world_asset_ground,&c->resources.world);
 if(rc!=1)return rc;
 if(taxi_challenge_drift_begin(&state,&context.driver.motion.orientation,&value))return -1;
 *car=v;*ctx=context;*timer=state;*option=value;return 1;
}
int taxi_challenge_drift_course_control(const TaxiChallengeDriftCourse *c,TaxiDriveWorldContext *ctx,TaxiChallengeDrift *timer,TaxiChallengeDriftInput gates,TaxiChallengeDriftStatus *status,TaxiChallengeDriftEffects *effects){
 if(!c||!ctx||!timer||!status||!effects||!c->resources.world.storage||c->resources.submode!=3||ctx->driver.motion.orientation.submode!=3||(ctx->driver.motion.orientation.settings&224)!=128)return -1;
 gates.count164c=ctx->driver.motion.orientation.counter_164c;
 taxi_challenge_drift_step(timer,&gates,status,effects);
 ctx->height.flag_1697=status->ended1697;ctx->height.flag_153c=status->success153c;
 ctx->driver.motion.orientation.flag_1697=status->ended1697;return 0;
}
