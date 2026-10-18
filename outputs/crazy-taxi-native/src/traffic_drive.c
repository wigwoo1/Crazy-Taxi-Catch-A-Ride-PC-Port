#include "traffic_drive.h"
static FILE *heading_trace;
void taxi_traffic_heading_trace(FILE *sink){heading_trace=sink;}
static FILE *frame_trace;
static unsigned long long frame_trace_serial;
void taxi_traffic_frame_trace(FILE *sink){frame_trace=sink;frame_trace_serial=0;}
static void trace_frame(const char *phase,TaxiTrafficActor *actors,size_t count,
 const TaxiTrafficScene *s,const TaxiDriveWorld *d,int result){
 if(!frame_trace||frame_trace_serial>=2048)return;
 const TaxiTrafficGlobals *g=s->globals;const TaxiGearState *p=&d->driver.power.gears;
 fprintf(frame_trace,"%s %llu %llu %ld %ld %d %d %d %d %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld %d",
 phase,frame_trace_serial,(unsigned long long)count,(long)s->frame_delta,(long)s->global_delta,
 s->paused,s->sound_enabled,s->trip_active,s->preserve_query_y,
 (long)s->player_position.x,(long)s->player_position.y,(long)s->player_position.z,(long)d->world.heading_1c,
 (long)s->camera.x,(long)s->camera.y,(long)s->camera.z,(long)s->camera_yaw,(long)s->camera_pitch,
 (long)s->player_sine,(long)s->player_cosine,result);
 fprintf(frame_trace," %d %u %u %u %ld %ld %u %u %u",g->cooldown_e4,g->near_miss_25d3,g->voice_25d6,
 g->combo_flag_9aa6,(long)g->combo_9aa8,(long)g->combo_9aac,*s->rng,*s->timer_25d5,*s->selection_counter);
 fprintf(frame_trace," %u %ld %d",p->flags_4ac,(long)p->speed_544,p->gear);
 for(unsigned i=0;i<7;i++)fprintf(frame_trace," %ld",(long)p->ratios[i]);
 fprintf(frame_trace," %ld %ld %ld %ld %d %ld %ld %ld %ld %ld %ld",(long)p->final_ratio,
 (long)p->scale_5b8,(long)p->scale_5cc,(long)p->boost_75c,p->rpm_538,(long)d->driver.power.scale_e84,
 (long)p->timer_e6c,(long)p->timer_e70,(long)p->timer_e78,(long)p->timer_e7c,(long)p->timer_e80);
 for(size_t i=0;i<count;i++){
  const TaxiTrafficActor *a=actors+i;
  fprintf(frame_trace," %u %u %ld %ld %ld %ld %u %ld %ld %ld %ld %ld %ld %ld %llu %ld %ld %u",
  a->flags,a->type,(long)a->countdown,(long)a->x,(long)a->z,(long)a->heading,a->route,(long)a->collision_timer,
  (long)a->impulse_x,(long)a->impulse_z,(long)a->offset_x,(long)a->offset_z,(long)a->y,
  (long)a->path.distance,(unsigned long long)a->path.segment,(long)a->path.x,(long)a->path.z,a->path.heading);
 }
 fputc('\n',frame_trace);
}
int taxi_traffic_drive_update(TaxiTrafficActor *actors,size_t count,
    TaxiTrafficScene *scene,TaxiDriveWorld *drive){
    if(!scene||!drive||!scene->trig)return -1;
    scene->player=&drive->driver.power;scene->player_position=drive->world.height.position;
    /* Source7050 LSL8/LSR20 selects bits12..23 of player heading1c. */
    unsigned heading=((uint32_t)drive->world.heading_1c>>12)&4095u;
    scene->player_sine=scene->trig->sine[heading];
    scene->player_cosine=scene->trig->cosine[heading];
    if(heading_trace)fprintf(heading_trace,"%ld %ld %ld %u %ld %ld\n",
        (long)scene->player_position.x,(long)scene->player_position.z,(long)drive->world.heading_1c,
        heading,(long)scene->player_sine,(long)scene->player_cosine);
    trace_frame("P",actors,count,scene,drive,0);
    int result=taxi_traffic_scene_update(actors,count,scene);
    trace_frame("O",actors,count,scene,drive,result);if(frame_trace)++frame_trace_serial;
    /* Earlier actors may have committed before a later actor fails. */
    drive->world.speed_544=drive->driver.power.gears.speed_544;
    drive->driver.motion.motion.orientation.speed_544=drive->world.speed_544;
    drive->world.height.scale_e84=drive->driver.power.scale_e84;
    drive->driver.motion.motion.scale_e84=drive->driver.power.scale_e84;
    return result;
}
int taxi_traffic_game_update(TaxiTrafficActor *actors,size_t count,
    TaxiTrafficScene *scene,TaxiDriveWorld *drive,TaxiPassengerScene *passengers){
    if(!scene||!drive||!passengers||!actors||!scene->globals||!scene->trig||
       !scene->angles||!scene->grid||!scene->roads||!scene->scales||!scene->height||
       !scene->obstacle||!scene->timer_25d5||!scene->selection_counter||
       !scene->submissions||scene->submission_count<count)return -1;
    scene->rng=&passengers->random_state;
    scene->trip_active=passengers->session.trip.active_1694!=0;
    scene->globals->combo_flag_9aa6=passengers->session.end.flag_9aa6;
    scene->globals->combo_9aa8=passengers->session.combo_whole_9aa8;
    scene->globals->combo_9aac=passengers->session.combo_fraction_9aac;
    /* Original020025D6 is shared with passenger/world voice writers. */
    scene->globals->voice_25d6=passengers->session.trip.voice_25d6;
    int result=taxi_traffic_drive_update(actors,count,scene,drive);
    passengers->session.end.flag_9aa6=scene->globals->combo_flag_9aa6;
    passengers->session.combo_whole_9aa8=scene->globals->combo_9aa8;
    passengers->session.combo_fraction_9aac=scene->globals->combo_9aac;
    if(scene->trip_active&&scene->submissions&&scene->submission_count>=count){
        for(size_t i=0;i<count;i++)if(scene->submissions[i].effects.collision){
            passengers->session.trip.voice_25d6=scene->globals->voice_25d6;
            passengers->pickup.voice_25d6=scene->globals->voice_25d6;
            break;
        }
    }
    return result;
}
