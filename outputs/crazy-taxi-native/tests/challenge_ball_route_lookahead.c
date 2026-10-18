/* Input-only route exploration over native recovered routines. This driver is
   diagnostic policy, not original game AI or physics. No actor relocation. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "challenge_ball_course.h"
#include "runtime_asset.h"
#include "game_start.h"
typedef struct {TaxiDriveWorld car;TaxiDriveWorldContext context;TaxiPassengerState actors[20];TaxiChallengeBall timer;TaxiBallStatus status;} Sim;
static TaxiChallengeBallCourse course;
static TaxiRuntimeAsset runtime;
static int32_t word(const unsigned char *p){uint32_t u=p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);int32_t s;memcpy(&s,&u,4);return s;}
static void fail(const char *s){fprintf(stderr,"FAIL %s\n",s);exit(1);}
static void begin(Sim *s){
 memset(s,0,sizeof *s);
 if(taxi_challenge_ball_course_start(&course,&s->car,&s->context,runtime.templates[0],584,128)!=1||taxi_challenge_ball_course_actors(&course,s->actors)!=1)fail("source course init");
 taxi_challenge_ball_begin(&s->timer);
}
static void tick(Sim *s,unsigned mask,int analog,int intro){
 TaxiBallEffects ball;TaxiBallInput gates={.intro=(uint8_t)intro,.delta=1};
 if(taxi_challenge_ball_course_control(&course,&s->timer,s->actors,&s->car,gates,&s->status,&ball,&runtime.trig))fail("native Ball controller");
 s->context.height.flag_1697=s->status.ended;s->context.height.flag_153c=s->status.success;
 TaxiBallProjection p[20]={0};uint32_t visible;
 if(taxi_challenge_ball_course_render(&course,s->actors,s->car.world.height.position,s->car.world.height.position,0,0,160,106,0,1,0,&runtime.trig,p,&visible))fail("source tick animation");
 if(s->status.ended)return;
 TaxiCommand command={0};command.bytes[4]=(uint8_t)(mask&15);uint16_t raw=(uint16_t)analog;command.bytes[12]=(uint8_t)raw;command.bytes[13]=(uint8_t)(raw>>8);
 TaxiGearInput input={.delta=1,.frame_delta=1,.settings=128,.action2=!!(mask&4),.action3=!!(mask&8),.action4=!!(mask&16),.action5=!!(mask&32)};
 TaxiDriveWorldEffects effects;
 int rc=intro?taxi_drive_world_start_step(&s->car,&s->context,&input,0,&runtime.trig,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects):taxi_drive_world_step(&s->car,&s->context,&command,&input,0,&runtime.trig,&runtime.traction,&runtime.normals,taxi_world_asset_obstacle,taxi_world_asset_ground,&course.resources.world,&effects);
 if(rc)fail("native car step");
}
static void settle(Sim *s){for(unsigned i=0;i<203;i++)tick(s,4,0,1);}
static unsigned nearest(Sim *s,unsigned last,double penalty,double angle_weight){
 unsigned best=20;double score=1e99;
 for(unsigned i=0;i<20;i++)if((s->actors[i].bytes[0]&1)&&!s->actors[i].bytes[10]){
  double x=(double)word(s->actors[i].bytes+16)-s->car.world.height.position.x,z=(double)word(s->actors[i].bytes+24)-s->car.world.height.position.z;
  double angle=fabs(remainder(-atan2(x,z)*4096/(2*3.141592653589793)-(double)s->car.world.heading_20/4096,4096));
  double value=(x*x+z*z)*(1+angle_weight*angle/512);if(i!=last)value*=penalty;
  if(value<score){score=value;best=i;}
 }
 return best;
}

static int steering(const Sim *s,unsigned target,int speed,double gain,double slow,unsigned *mask){
 double dx=(double)word(s->actors[target].bytes+16)-s->car.world.height.position.x;
 double dz=(double)word(s->actors[target].bytes+24)-s->car.world.height.position.z;
 double desired=-atan2(dx,dz)*4096/(2*3.141592653589793);
 double error=remainder(desired-(double)s->car.world.heading_20/4096,4096);
 int analog=(int)(error*gain);if(analog>192)analog=192;if(analog<-192)analog=-192;
 double limit=speed/(1+fabs(error)*slow/512);*mask=s->car.world.speed_544>limit?8:4;
 return analog;
}
static double forecast_score(const Sim *s,unsigned target,int initial_count,double angular,double reward){
 if(s->actors[target].bytes[10])target=nearest((Sim *)s,20,1,0.1);
 if(target==20)return -1e20;
 double dx=(double)word(s->actors[target].bytes+16)-s->car.world.height.position.x;
 double dz=(double)word(s->actors[target].bytes+24)-s->car.world.height.position.z;
 double desired=-atan2(dx,dz)*4096/(2*3.141592653589793);
 double error=fabs(remainder(desired-(double)s->car.world.heading_20/4096,4096));
 return (dx*dx+dz*dz)*(1+angular*error/512)-reward*(s->timer.popped_1556-initial_count);
}
int main(int argc,char **argv){
 if(taxi_challenge_ball_course_load(&course,"outputs/crazy-taxi-native/assets")||taxi_runtime_asset_load(&runtime,"outputs/crazy-taxi-native/assets/driving.ctr"))fail("original assets");
 Sim initial;begin(&initial);settle(&initial);Sim baseline=initial;
 for(unsigned i=0;i<397;i++)tick(&baseline,4|(i==0?16:0),0,0);
 if(baseline.car.world.height.position.x!=-1||baseline.car.world.height.position.y!=73||baseline.car.world.height.position.z!=19742||baseline.timer.popped_1556!=1||baseline.timer.remaining_1558!=2303)fail("headless baseline differs from final native viewer600");
 puts("PASS native600 baseline position-1,73,19742/pop1/remaining2303; input-only forecast diagnostic.");fflush(stdout);
 unsigned policies=argc>1?(unsigned)atoi(argv[1]):16;if(!policies||policies>256)fail("policy count1..256");
 unsigned best_count=0,best_ticks=2701;unsigned char best_masks[2700];int16_t best_analogs[2700];Sim best=initial;
 const int sticks[]={-192,-128,-64,0,64,128,192};
 for(unsigned policy=0;policy<policies;policy++){
  Sim sim=initial;unsigned elapsed=0,last=20;unsigned char masks[2700];int16_t analogs[2700];
  unsigned horizon=16+(policy%3)*16,commit=4;
  int speed=32000+(int)((policy/3)%4)*5000;double slow=((policy/12)%3)*0.25;
  double angular=0.1+(policy%4)*0.2,reward=2e8+(policy%3)*2e8;
  while(elapsed<2700&&!sim.status.ended){
   unsigned target=nearest(&sim,last,1.37,0.1);if(target==20)break;last=target;
   double score=1e100;int chosen=0;unsigned chosen_mask=4;
   for(unsigned gas=0;gas<2;gas++)for(unsigned stick=0;stick<sizeof sticks/sizeof *sticks;stick++){
    Sim predicted=sim;unsigned action=gas?8:4;
    for(unsigned ahead=0;ahead<horizon&&!predicted.status.ended;ahead++){
     int analog=sticks[stick];unsigned mask=action;
     if(ahead>=commit){unsigned follow=nearest(&predicted,target,1.37,0.1);if(follow==20)break;analog=steering(&predicted,follow,speed,0.262,slow,&mask);}
     if(elapsed+ahead==0)mask|=16;
     tick(&predicted,mask,analog,0);
    }
    double value=forecast_score(&predicted,target,sim.timer.popped_1556,angular,reward);
    if(value<score){score=value;chosen=sticks[stick];chosen_mask=action;}
   }
   for(unsigned n=0;n<commit&&elapsed<2700&&!sim.status.ended;n++){
    unsigned mask=chosen_mask|(elapsed==0?16:0);masks[elapsed]=(unsigned char)mask;analogs[elapsed]=(int16_t)chosen;
    tick(&sim,mask,chosen,0);elapsed++;
   }
  }
  unsigned count=(unsigned)sim.timer.popped_1556;
  printf("POLICY%u popped%u elapsed%u horizon%u speed%d slow%.2f final%d,%d\n",policy,count,elapsed,horizon,speed,slow,sim.car.world.height.position.x,sim.car.world.height.position.z);fflush(stdout);
  if(count>best_count||(count==best_count&&elapsed<best_ticks)){
   best=sim;best_count=count;best_ticks=elapsed;memcpy(best_masks,masks,elapsed);memcpy(best_analogs,analogs,elapsed*sizeof *analogs);
  }
 }
 FILE *f=fopen("work/ball-input-route-lookahead.txt","wb");if(!f)fail("output");
 for(unsigned i=0;i<best_ticks;i++)fprintf(f,"%u %d\n",best_masks[i],best_analogs[i]);if(fclose(f))fail("close");
 printf("LOOKAHEAD CANDIDATE popped%u/20 elapsed%u; actual viewer replay required.\n",best_count,best_ticks);
 for(unsigned i=0;i<20;i++)if(!best.actors[i].bytes[10])printf("REMAIN actor%u X%d Z%d\n",i,word(best.actors[i].bytes+16),word(best.actors[i].bytes+24));
 taxi_challenge_ball_course_free(&course);return 0;
}
