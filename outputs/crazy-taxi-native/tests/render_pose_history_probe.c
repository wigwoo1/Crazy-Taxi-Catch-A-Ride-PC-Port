#include "../src/render_pose_history.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
int main(void){TaxiRenderPoseHistory h={0};char cmd;unsigned long long time;while(scanf(" %c",&cmd)==1){if(cmd=='R'){taxi_render_pose_clear(&h);continue;}assert(scanf("%llu",&time)==1);TaxiRenderPose p;TaxiRenderBracket b;int *fields[12]={&p.taxi.x,&p.taxi.y,&p.taxi.z,&p.camera.x,&p.camera.y,&p.camera.z,&p.vehicle_angles.x,&p.vehicle_angles.y,&p.vehicle_angles.z,&p.camera_angles.x,&p.camera_angles.y,&p.camera_angles.z};if(cmd=='P'){for(unsigned n=0;n<12;n++)assert(scanf("%d",fields[n])==1);assert(!taxi_render_pose_push(&h,time,&p));TaxiRenderPoseHistory prior=h;assert(taxi_render_pose_push(&h,time,&p)==-1);assert(!memcmp(&prior,&h,sizeof(h)));}else{assert(cmd=='Q');TaxiRenderPoseHistory prior=h;assert(!taxi_render_pose_sample(&h,time,&p,&b));assert(!memcmp(&prior,&h,sizeof(h)));for(unsigned n=0;n<12;n++)printf("%d ",*fields[n]);puts("");}}return 0;}
