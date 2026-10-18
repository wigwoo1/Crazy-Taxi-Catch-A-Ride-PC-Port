#include "traffic_scene.h"
#include <string.h>
static int actor_step(void *context,TaxiTrafficActor *a,size_t index,int impact){
    TaxiTrafficScene *s=context;if(a->route>=256)return -1;
    TaxiTrafficActorEffects *effects=&s->submissions[index].effects;
    if(impact)return taxi_traffic_actor_postimpact(a,s->roads+a->route,s->player,
        s->player_position.x,s->player_position.z,s->player_sine,s->player_cosine,
        s->frame_delta,s->global_delta,s->sound_enabled,s->trip_active,s->globals,s->rng,
        s->preserve_query_y,s->obstacle,s->terrain,s->angles,effects);
    return taxi_traffic_actor_follow(a,s->roads+a->route,s->player,
        s->player_position.x,s->player_position.z,s->player_sine,s->player_cosine,
        s->frame_delta,s->global_delta,s->sound_enabled,s->trip_active,s->globals,s->rng,effects);
}
static int follow(void *context,TaxiTrafficActor *a,size_t i){return actor_step(context,a,i,0);}
static int impact(void *context,TaxiTrafficActor *a,size_t i){return actor_step(context,a,i,1);}
static int common(void *context,TaxiTrafficActor *a,size_t i){
    TaxiTrafficScene *s=context;TaxiTrafficSubmission *out=s->submissions+i;
    int visible=taxi_traffic_presentation_prepare(a,s->camera.x,s->camera.z,s->frame_delta,
        s->paused,s->height,s->terrain);
    if(visible<=0)return visible;
    if(a->type>=3||taxi_traffic_view(a,s->camera,s->camera_yaw,s->camera_pitch,s->trig,s->angles,&out->view))return -1;
    out->visible=1;out->prepared=1;out->type=a->type;out->scale=s->scales[a->type];
    out->billboard=taxi_traffic_billboard(out->view.camera_space,out->scale);
    TaxiTrafficProjection projection=taxi_traffic_projection_normal();
    int projected=taxi_traffic_project(&out->billboard,&projection,&out->screen);
    if(projected<0)return -1;
    if(!projected){out->visible=0;return 0;}
    /* Original66b4 brackets its submissions with3201c(144<<5)/3201c(0). */
    if(taxi_traffic_depth_bucket(out->view.camera_space.z,4608,projection.far_plane,
        s->trig,&out->depth_bucket))return -1;
    out->position=(TaxiPoint){a->x,a->y,a->z};return 0;
}
int taxi_traffic_scene_update(TaxiTrafficActor *actors,size_t count,TaxiTrafficScene *s){
    if(!s||!s->player||!s->roads||!s->grid||!s->trig||!s->angles||!s->globals||
       !s->scales||!s->submissions||s->submission_count<count||!s->height||!s->obstacle||
       !s->timer_25d5||!s->rng||!s->selection_counter)return -1;
    memset(s->submissions,0,count*sizeof *s->submissions);
    for(size_t i=0;i<count;i++)s->submissions[i].effects.effects.sound_table_index=-1;
    TaxiTrafficFrameHooks hooks={follow,impact,common,s};
    return taxi_traffic_frame(actors,count,s->frame_delta,s->paused,s->camera.x,s->camera.z,
        s->grid,s->roads,s->spacing,s->rng,s->selection_counter,&s->globals->cooldown_e4,
        s->timer_25d5,&hooks);
}
