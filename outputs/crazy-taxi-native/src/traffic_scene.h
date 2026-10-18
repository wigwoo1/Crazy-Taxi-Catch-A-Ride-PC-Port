#ifndef TAXI_TRAFFIC_SCENE_H
#define TAXI_TRAFFIC_SCENE_H
#include "traffic_actor.h"
#include "traffic_frame.h"
#include "traffic_presentation.h"
typedef struct {
    /* prepared marks original common presentation gate passing before depth
       projection; visible retains the original latest-tick projection result. */
    int visible,prepared;uint32_t type,scale;TaxiPoint position;TaxiTrafficView view;
    TaxiTrafficBillboard billboard;
    TaxiTrafficScreenBounds screen;unsigned depth_bucket;
    TaxiTrafficActorEffects effects;
} TaxiTrafficSubmission;
typedef struct {
    TaxiAcceleration *player;TaxiPoint player_position,camera;
    int32_t player_sine,player_cosine,camera_yaw,camera_pitch;
    int32_t frame_delta,global_delta;
    int paused,sound_enabled,trip_active,preserve_query_y;
    const TaxiTrafficGrid *grid;const TaxiAiPathAsset *roads;
    const TaxiTrig *trig;const TaxiAngleTable *angles;
    TaxiTrafficGlobals *globals;uint8_t *timer_25d5;
    uint32_t *rng,*selection_counter;uint8_t spacing;
    const uint32_t *scales;
    TaxiObstacleQuery obstacle;TaxiTrafficHeightQuery height;void *terrain;
    TaxiTrafficSubmission *submissions;size_t submission_count;
} TaxiTrafficScene;
/* Normal gameplay composition only. Emits original31a50 arguments and effect
   requests; GPU/audio and shared player duplicate-field synchronization external. */
int taxi_traffic_scene_update(TaxiTrafficActor *actors,size_t count,TaxiTrafficScene *scene);
#endif
