#include "movement.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
typedef struct {int height,wall;unsigned count;TaxiPoint queries[64];} Fixture;
static unsigned events;static TaxiPoint observed_start,observed_move;static int observed_preserve;static TaxiObstacleQuery observed_query;static void *observed_context;static TaxiMovementResult observed_result;
static int query(void *context,TaxiPoint point,TaxiObstacleHit *hit){Fixture *f=context;assert(f->count<64);f->queries[f->count++]=point;hit->height=f->height;if(f->wall){hit->edge_count=3;hit->vertices[0]=(TaxiPoint){0,0,0};hit->vertices[1]=(TaxiPoint){1000,0,0};hit->vertices[2]=(TaxiPoint){0,0,1000};return 2;}return f->height==INT_MIN?0:1;}
static void observe(TaxiPoint start,TaxiPoint move,int preserve,TaxiObstacleQuery callback,void *context,const TaxiMovementResult *result){events++;observed_start=start;observed_move=move;observed_preserve=preserve;observed_query=callback;observed_context=context;observed_result=*result;}
static int same_point(TaxiPoint a,TaxiPoint b){return a.x==b.x&&a.y==b.y&&a.z==b.z;}
static int same_result(TaxiMovementResult a,TaxiMovementResult b){return same_point(a.position,b.position)&&same_point(a.normal,b.normal)&&a.queries==b.queries&&a.collided==b.collided;}
int main(void){TaxiPoint moves[]={{0,0,0},{-18,24,163},{500,0,-900},{-1000,100,700},{255,0,0},{256,0,0}};int heights[]={INT_MIN,0,3000,3654,3655,4000};unsigned cases=0;
 for(unsigned m=0;m<6;m++)for(unsigned h=0;h<6;h++)for(int preserve=0;preserve<2;preserve++)for(int wall=0;wall<2;wall++){
  TaxiPoint start={1000,3000,-500};Fixture a={.height=heights[h],.wall=wall},b=a;TaxiMovementResult baseline,observed;taxi_movement_observer(NULL);assert(!taxi_move_position(start,moves[m],preserve,query,&a,&baseline));unsigned prior=events;taxi_movement_observer(observe);assert(!taxi_move_position(start,moves[m],preserve,query,&b,&observed));assert(events==prior+1);assert(same_result(baseline,observed)&&same_result(observed,observed_result));assert(same_point(observed_start,start)&&same_point(observed_move,moves[m])&&observed_preserve==preserve&&observed_query==query&&observed_context==&b);assert(a.count==b.count);for(unsigned i=0;i<a.count;i++)assert(same_point(a.queries[i],b.queries[i]));cases++;
 }
 taxi_movement_observer(NULL);printf("PASS movement observer invariance: %u cases; exact outputs, query sequence and callback arguments.\n",cases);return 0;}
