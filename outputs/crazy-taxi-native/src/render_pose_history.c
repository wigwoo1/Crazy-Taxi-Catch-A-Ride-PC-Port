#include "render_pose_history.h"
#include <string.h>
void taxi_render_pose_clear(TaxiRenderPoseHistory *h){if(h)memset(h,0,sizeof(*h));}
int taxi_render_pose_push(TaxiRenderPoseHistory *h,uint64_t time,const TaxiRenderPose *pose){
 if(!h||!pose)return -1;unsigned slot=(h->time.begin+h->time.count)%TAXI_RENDER_HISTORY_CAPACITY;
 if(taxi_render_history_push(&h->time,time,slot))return -1;h->poses[slot]=*pose;return 0;
}
static TaxiPoint angles(TaxiPoint a,TaxiPoint b,double alpha){return (TaxiPoint){taxi_render_angle(a.x,b.x,alpha),taxi_render_angle(a.y,b.y,alpha),taxi_render_angle(a.z,b.z,alpha)};}
int taxi_render_pose_sample(const TaxiRenderPoseHistory *h,uint64_t time,TaxiRenderPose *out,TaxiRenderBracket *bracket){
 if(!h||!out||!bracket)return -1;TaxiRenderBracket b;if(taxi_render_history_bracket(&h->time,time,&b))return -1;
 if(b.before_tag>=TAXI_RENDER_HISTORY_CAPACITY||b.after_tag>=TAXI_RENDER_HISTORY_CAPACITY)return -1;
 TaxiRenderPose a=h->poses[b.before_tag],z=h->poses[b.after_tag],result={taxi_render_point(a.taxi,z.taxi,b.alpha),taxi_render_point(a.camera,z.camera,b.alpha),angles(a.vehicle_angles,z.vehicle_angles,b.alpha),angles(a.camera_angles,z.camera_angles,b.alpha)};
 *out=result;*bracket=b;return 0;
}
