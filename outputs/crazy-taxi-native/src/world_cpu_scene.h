#ifndef TAXI_WORLD_CPU_SCENE_H
#define TAXI_WORLD_CPU_SCENE_H
#include "world_indexed.h"
#include "taxi_shadow.h"
#include "taxi_shadow_render.h"
typedef struct {unsigned packets;TaxiPairColumnWork textured;TaxiFlatRowWork flat;} TaxiWorldCpuWork;
/* Source240 native indexed consumer. Caller seeds shadow before scene build.
   Consumes/clears queue; other actors/HUD and display priorities are separate. */
int taxi_world_cpu_scene(uint8_t framebuffer[38400],TaxiWorldFrame *frame,
 int32_t background,const uint8_t lookup[2560],const TaxiShadow *shadow,
 const TaxiShadowRender *shadow_view,unsigned shadow_frame,unsigned shadow_kind,
 TaxiWorldCpuWork *work);
#endif
