#include "world_clip.h"
#include <string.h>
#include <limits.h>
_Static_assert(sizeof(TaxiWorldClipVertex)==32,"source vertex layout");
static int32_t bits(uint32_t u){int32_t s;memcpy(&s,&u,4);return s;}
static int divide(int32_t n,int32_t d,int32_t *q){
    if(!d||(n==INT32_MIN&&d==-1))return -1;
    *q=n/d;return 0;
}
static int interpolate(int32_t a,int32_t b,int32_t distance,int32_t span,
    int attribute,int32_t *out){
    int32_t delta=bits((uint32_t)b-(uint32_t)a),q;
    if(attribute){
        if(divide(delta,span,&q))return -1;
        q=bits((uint32_t)q*(uint32_t)distance);
    }else{
        if(divide(bits((uint32_t)delta*(uint32_t)distance),span,&q))return -1;
    }
    *out=bits((uint32_t)a+(uint32_t)q);return 0;
}
static int crossing(const TaxiWorldClipVertex *inside,const TaxiWorldClipVertex *outside,
    int32_t plane,TaxiWorldClipVertex *out){
    TaxiWorldClipVertex v={.z=plane};
    int32_t distance=bits((uint32_t)plane-(uint32_t)inside->z);
    int32_t span=bits((uint32_t)outside->z-(uint32_t)inside->z);
    if(interpolate(inside->x,outside->x,distance,span,0,&v.x)||
        interpolate(inside->y,outside->y,distance,span,0,&v.y)||
        interpolate(inside->u,outside->u,distance,span,1,&v.u)||
        interpolate(inside->v,outside->v,distance,span,1,&v.v)||
        interpolate(inside->attribute,outside->attribute,distance,span,1,&v.attribute))return -1;
    *out=v;return 0;
}
int taxi_world_clip_depth(const TaxiWorldClipVertex *v,unsigned count,int32_t plane,
    int far_clip,TaxiWorldClippedPolygon *out){
    if(!v||!out||(count!=3&&count!=4)||(far_clip!=0&&far_clip!=1))return -1;
    TaxiWorldClippedPolygon result={0};
    const TaxiWorldClipVertex *previous=v+count-1;
    int previous_inside=far_clip?previous->z<plane:previous->z>plane;
    for(unsigned i=0;i<count;i++){
        const TaxiWorldClipVertex *current=v+i;
        int inside=far_clip?current->z<plane:current->z>plane;
        if(inside!=previous_inside){
            if(crossing(inside?current:previous,inside?previous:current,plane,
                result.vertices+result.count))return -1;
            result.count++;
        }
        if(inside)result.vertices[result.count++]=*current;
        previous=current;previous_inside=inside;
    }
    if(result.count<=2)return 0;
    *out=result;return 1;
}
int taxi_world_project_vertices(TaxiWorldClippedPolygon *polygon,const TaxiTrafficProjection *p,
    TaxiTrafficScreenBounds *bounds){
    if(!polygon||!p||!bounds||polygon->count<3||polygon->count>8)return -1;
    TaxiWorldClippedPolygon result=*polygon;
    int32_t minx=INT32_MAX,miny=INT32_MAX,maxx=INT32_MIN,maxy=INT32_MIN;
    for(unsigned i=0;i<result.count;i++){
        TaxiWorldClipVertex *v=result.vertices+i;int32_t x,y;
        if(divide(bits((uint32_t)v->x*(uint32_t)p->focal),v->z,&x)||
            divide(bits((uint32_t)v->y*(uint32_t)p->focal),v->z,&y))return -1;
        v->screen_x=bits((uint32_t)p->center_x+(uint32_t)x);
        v->screen_y=bits((uint32_t)p->center_y-(uint32_t)y);
        if(v->screen_x<minx)minx=v->screen_x;if(v->screen_x>maxx)maxx=v->screen_x;
        if(v->screen_y<miny)miny=v->screen_y;if(v->screen_y>maxy)maxy=v->screen_y;
    }
    *bounds=(TaxiTrafficScreenBounds){minx,miny,maxx,maxy};
    *polygon=result;return 0;
}
int taxi_world_clip_project(TaxiWorldClippedPolygon *polygon,const TaxiTrafficProjection *p,
    int32_t left,int32_t top,int32_t right,int32_t bottom){
    if(!polygon)return -1;
    TaxiWorldClippedPolygon result=*polygon;TaxiTrafficScreenBounds bounds;
    if(taxi_world_project_vertices(&result,p,&bounds))return -1;
    if(bounds.left>=right||bounds.right<left||bounds.top>=bottom||bounds.bottom<top)return 0;
    *polygon=result;return 1;
}
