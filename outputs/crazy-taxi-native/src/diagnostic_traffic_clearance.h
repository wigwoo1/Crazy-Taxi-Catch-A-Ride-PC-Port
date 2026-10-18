/* Opt-in diagnostic driver only; stationary traffic snapshots.
   Return 0=unchanged clear center, 1=selected alternative, -1=brake request.
   Does not guarantee future moving-traffic safety or braking distance. */
#ifndef TAXI_DIAGNOSTIC_CLEARANCE_H
#define TAXI_DIAGNOSTIC_CLEARANCE_H
#include "traffic_spawn.h"
#include "ground.h"
#include <math.h>
#include "diagnostic_traffic_avoid.h"
static int candidate_traffic_clear(const TaxiTrafficActor *actors,size_t count,TaxiPoint player,double dx,double dz){
 double length2=dx*dx+dz*dz;
 if(length2<=0)return 0;
 for(size_t i=0;i<count;i++){
  const TaxiTrafficActor *a=actors+i;
  if(a->countdown>0||fabs((double)a->y-player.y)>512)continue;
  double ax=(double)a->x-player.x,az=(double)a->z-player.z;
  double t=fmax(0,fmin(1,(ax*dx+az*dz)/length2));
  double ex=ax-t*dx,ez=az-t*dz;
  if(ex*ex+ez*ez<704.0*704)return 0;
 }
 return 1;
}
static int candidate_road_clear(TaxiPoint player,double dx,double dz,double ux,double uz,TaxiDiagnosticRoadCheck check,void *context){
 for(int step=1;step<=16;step++)for(int edge=-1;edge<=1;edge++){
  double t=step/16.0;
  TaxiPoint q={(int32_t)llround(player.x+dx*t+uz*384*edge),player.y,
               (int32_t)llround(player.z+dz*t-ux*384*edge)};
  if(!check(context,q))return 0;
 }
 return 1;
}
static int taxi_diagnostic_traffic_clearance(const TaxiTrafficActor *actors,size_t count,
 TaxiPoint player,double sx,double sz,int64_t *dx,int64_t *dz,TaxiDiagnosticRoadCheck check,void *context){
 double length=hypot(sx,sz);
 if(!actors||!dx||!dz||!check||length<=0)return -1;
 double ux=sx/length,uz=sz/length;
 /* Preserve the original route target when its approach is clear. */
 if(candidate_traffic_clear(actors,count,player,(double)*dx,(double)*dz))return 0;
 const int offsets[]={-768,768,-1024,1024,-1536,1536,-2048,2048};
 double current=-((double)*dx*uz-(double)*dz*ux),best=1e100;int found=0;
 int64_t bx=*dx,bz=*dz;
 for(unsigned i=0;i<sizeof offsets/sizeof offsets[0];i++){
  double cx=(double)*dx+uz*offsets[i],cz=(double)*dz-ux*offsets[i];
  if(!candidate_traffic_clear(actors,count,player,cx,cz))continue;
  if(!candidate_road_clear(player,cx,cz,ux,uz,check,context))continue;
  double score=fabs(offsets[i]-current)+0.01*fabs((double)offsets[i]);
  if(score<best){best=score;found=1;bx=(int64_t)llround(cx);bz=(int64_t)llround(cz);}
 }
 if(!found)return -1;
 *dx=bx;*dz=bz;return 1;
}
#endif
