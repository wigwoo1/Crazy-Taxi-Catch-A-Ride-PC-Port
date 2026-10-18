#ifndef TAXI_TRAFFIC_PRESENTATION_H
#define TAXI_TRAFFIC_PRESENTATION_H
#include "traffic_spawn.h"
#include "ground.h"
#include "traction.h"
typedef int (*TaxiTrafficHeightQuery)(void *context,TaxiPoint point,int32_t *height);
/* Source67da..683e. Return1 requests remaining camera/sprite submission;
   return0 skips/despawns. Invalid query returns-1 without committing actor. */
int taxi_traffic_presentation_prepare(TaxiTrafficActor *actor,int32_t camera_x,
    int32_t camera_z,int32_t frame_delta,int paused,TaxiTrafficHeightQuery height,
    void *context);
typedef struct {TaxiPoint camera_space;int32_t direction;unsigned frame;} TaxiTrafficView;
/* Original6840..6898, sequential yaw then pitch with wrapped32 products. */
int taxi_traffic_view(const TaxiTrafficActor *actor,TaxiPoint camera,int32_t yaw,
    int32_t pitch,const TaxiTrig *trig,const TaxiAngleTable *angles,TaxiTrafficView *view);
typedef struct {TaxiPoint top_left,bottom_right;} TaxiTrafficBillboard;
/* Source31aee..31b5e normal128x64 descriptor, r3/stackflags0. */
TaxiTrafficBillboard taxi_traffic_billboard(TaxiPoint camera_space,uint32_t scale);
typedef struct {int32_t center_x,center_y,focal,near_plane,far_plane;} TaxiTrafficProjection;
/* Source2c790 projection fields. Centers use original unaligned edges;
   near/far retain the separate2c784 setter's values. */
void taxi_traffic_projection_viewport(TaxiTrafficProjection *projection,
    int32_t left,int32_t top,int32_t right,int32_t bottom);
/* Normal startupadb0 ->2ac6c ->2c790/2c784, 240x160 viewport. */
TaxiTrafficProjection taxi_traffic_projection_normal(void);
typedef struct {int32_t left,top,right,bottom;} TaxiTrafficScreenBounds;
/* Source31ac0 near+768/far gate and31d34..31dfa wrapped signed projections.
   Returns0 depth-culled,1 projected,-1 invalid division, retaining output. */
int taxi_traffic_project(const TaxiTrafficBillboard *billboard,
    const TaxiTrafficProjection *projection,TaxiTrafficScreenBounds *bounds);
/* Source31e86..31eca: nonlinear queue bucket, not a lighting coefficient.
   Original consumer visits127..0, newest packet first within each bucket. */
int taxi_traffic_depth_bucket(int32_t depth,int32_t depth_offset,int32_t far_plane,
    const TaxiTrig *trig,unsigned *bucket);
#endif
