/* Diagnostic input-target selection only. No game-owner writes. */
#ifndef TAXI_DIAGNOSTIC_TRAFFIC_AVOID_H
#define TAXI_DIAGNOSTIC_TRAFFIC_AVOID_H
#include "traffic_spawn.h"
#include "ground.h"
#include <math.h>
typedef int (*TaxiDiagnosticRoadCheck)(void *,TaxiPoint);
static int taxi_diagnostic_traffic_target(const TaxiTrafficActor *actors,size_t count,
 TaxiPoint player,double sx,double sz,int64_t *dx,int64_t *dz,
 TaxiDiagnosticRoadCheck check,void *context){
 double length=hypot(sx,sz),weight=0;
 if(!actors||!dx||!dz||!check||length<=0)return 0;
 double ux=sx/length,uz=sz/length;
 for(size_t i=0;i<count;i++){
  const TaxiTrafficActor *a=actors+i;
  if(a->countdown>0||fabs((double)a->y-player.y)>512)continue;
  double ax=(double)a->x-player.x,az=(double)a->z-player.z;
  double ahead=ax*ux+az*uz,lateral=(ax-(double)*dx)*uz-(az-(double)*dz)*ux;
  if(ahead<=-1536||ahead>=6144||fabs(lateral)>768)continue;
  double w=ahead>4096?(6144-ahead)/2048:ahead<0?(ahead+1536)/1536:1;
  if(w>weight)weight=w;
 }
 if(weight<=0)return 0;
 double cx=(double)*dx-uz*1024*weight,cz=(double)*dz+ux*1024*weight;
 /* Sample the intended approach and a 384-unit footprint on both sides.
    Rejection retains the original target; no assertion of swept safety. */
 for(int step=1;step<=8;step++)for(int edge=-1;edge<=1;edge++){
  double t=step/8.0;
  TaxiPoint q={(int32_t)llround(player.x+cx*t+uz*384*edge),player.y,
               (int32_t)llround(player.z+cz*t-ux*384*edge)};
  if(!check(context,q))return 0;
 }
 *dx=(int64_t)llround(cx);*dz=(int64_t)llround(cz);return 1;
}
#endif
