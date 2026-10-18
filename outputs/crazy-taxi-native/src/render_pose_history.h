#ifndef TAXI_RENDER_POSE_HISTORY_H
#define TAXI_RENDER_POSE_HISTORY_H
#include "render_history.h"
#include "render_interpolation.h"
typedef struct {TaxiPoint taxi,camera,vehicle_angles,camera_angles;} TaxiRenderPose;
typedef struct {TaxiRenderHistory time;TaxiRenderPose poses[TAXI_RENDER_HISTORY_CAPACITY];} TaxiRenderPoseHistory;
/* Presentation extension only: angles in original24-bit fixed circle.
   Caller clears on reset/rebase; snapshots copied, no simulation pointers. */
void taxi_render_pose_clear(TaxiRenderPoseHistory *);
int taxi_render_pose_push(TaxiRenderPoseHistory *,uint64_t,const TaxiRenderPose *);
int taxi_render_pose_sample(const TaxiRenderPoseHistory *,uint64_t,TaxiRenderPose *,TaxiRenderBracket *);
#endif
