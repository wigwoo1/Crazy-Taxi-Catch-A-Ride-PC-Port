#ifdef NDEBUG
#undef NDEBUG
#endif
#include "../src/reference_driving.h"
#include <assert.h>
#include <stdio.h>
static TaxiReferencePostDriverHook target_post;
static void *target_context;
static unsigned pre_count,post_count;
static int32_t prior_car[7];
static void car_values(const TaxiReferenceDriving *f,int32_t out[7]){
 out[0]=f->car.world.height.position.x;out[1]=f->car.world.height.position.y;out[2]=f->car.world.height.position.z;
 out[3]=f->car.world.speed_544;out[4]=f->car.driver.power.gears.rpm_538;
 out[5]=f->car.driver.power.gears.steering_578;out[6]=f->car.world.heading_1c;
}
static int before_physics(void *unused,TaxiReferenceDriving *f,const TaxiReferenceDrivingStep *s,const TaxiTrig *trig){
 (void)unused;(void)trig;int32_t current[7];car_values(f,current);
 assert(pre_count==post_count&&f->next==pre_count);
 for(unsigned j=0;j<7;j++)assert(current[j]==prior_car[j]);
 assert(f->input.raw_keys==s->raw);
 if(s->dynamic_owners){assert(f->context.height.flag_1697==s->flag_1697);assert(f->context.driver.freeze_2620==s->freeze_2620);}
 pre_count++;return 0;
}
static int after_physics(void *unused,TaxiReferenceDriving *f,const TaxiReferenceDrivingStep *s,const TaxiTrig *trig){
 (void)unused;assert(pre_count==post_count+1&&f->next==post_count);
 int rc=target_post?target_post(target_context,f,s,trig):0;
 if(rc)return rc;car_values(f,prior_car);post_count++;return 0;
}
static void checked_post_set(TaxiReferencePostDriverHook hook,void *context){
 target_post=hook;target_context=context;
 taxi_reference_driving_set_post_driver_hook(hook?after_physics:NULL,NULL);
}
#define taxi_reference_driving_set_post_driver_hook checked_post_set
#define main preserved_state_fixture_main
#include "continuous_driver_traffic_state_probe.c"
#undef main
#undef taxi_reference_driving_set_post_driver_hook
static int reject_service(void *unused,TaxiReferenceDriving *f,const TaxiReferenceDrivingStep *s,const TaxiTrig *trig){
 (void)unused;(void)trig;assert(f->next==0&&f->input.raw_keys==s->raw);return -1;
}
int main(int argc,char **argv){
 assert(argc==5);TaxiReferenceDriving initial={0};assert(!taxi_reference_driving_load(&initial,argv[3]));
 car_values(&initial,prior_car);
 taxi_reference_driving_set_pre_driver_hook(reject_service,NULL);
 assert(taxi_reference_driving_step(&initial,NULL,NULL,NULL,NULL,NULL,NULL)==-1);
 assert(initial.next==0);int32_t rejected[7];car_values(&initial,rejected);
 for(unsigned j=0;j<7;j++)assert(rejected[j]==prior_car[j]);
 taxi_reference_driving_free(&initial);
 taxi_reference_driving_set_pre_driver_hook(before_physics,NULL);
 int rc=preserved_state_fixture_main(argc,argv);
 taxi_reference_driving_set_pre_driver_hook(NULL,NULL);
 assert(!rc&&pre_count==235&&post_count==235);
 printf("SERVICE_ORDER_PASS pre=%u post=%u reject_before_physics=1\n",pre_count,post_count);
 return rc;
}