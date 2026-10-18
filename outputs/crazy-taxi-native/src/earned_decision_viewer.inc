/* Bounded input-driver evidence, flushed once on exit. No control decision. */
typedef struct {uint32_t counter;unsigned waypoint,chosen;int32_t delta,clock,trip,speed;TaxiPoint position;TaxiRoutePoint target;TaxiXboxSample pads[7];EarnedCandidates candidates;} EarnedDecisionRecord;
static EarnedDecisionRecord earned_decisions[12000];static unsigned earned_decision_count;
static void earned_decision_record(const EarnedForecastState *state,const TaxiXboxSample *pads,const EarnedCandidates *candidates,unsigned chosen,int32_t dt){
 if(!earned_decision_path)return;if(earned_decision_count>=12000)fail("Earned decision trace capacity exceeded.");
 EarnedDecisionRecord *r=&earned_decisions[earned_decision_count++];r->counter=live_host_time.counter;r->waypoint=live_delivery_waypoint;r->chosen=chosen;r->delta=dt;r->clock=passenger_scene.session.clock.remaining_16a8;r->trip=passenger_scene.session.trip.time_16ac;r->position=state->car.world.height.position;r->speed=state->car.world.speed_544;r->target=live_delivery_route->points[live_delivery_waypoint];memcpy(r->pads,pads,sizeof r->pads);r->candidates=*candidates;
}
static void earned_decision_write(void){
 if(!earned_decision_path)return;FILE *f=fopen(earned_decision_path,"wb");if(!f){fputs("Decision trace open failed.\n",stderr);return;}
 fputs("counter,delta,waypoint,chosen,clock,trip,x,y,z,speed,target_x,target_z,candidate,steering,accelerator,brake,status,steps,world_hit,traffic_hit,unsupported,traffic_index,predicted_x,predicted_y,predicted_z,predicted_speed\n",f);
 for(unsigned i=0;i<earned_decision_count;i++){
  const EarnedDecisionRecord *r=&earned_decisions[i];
  for(unsigned j=0;j<7;j++){const EarnedCandidate *c=&r->candidates.candidate[j];const TaxiXboxSample *p=&r->pads[j];
   fprintf(f,"%u,%ld,%u,%u,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%u,%d,%u,%u,%u,%u,%u,%u,%u,%llu,%ld,%ld,%ld,%ld\n",r->counter,(long)r->delta,r->waypoint,r->chosen,(long)r->clock,(long)r->trip,(long)r->position.x,(long)r->position.y,(long)r->position.z,(long)r->speed,(long)r->target.x,(long)r->target.z,j,p->steering,p->accelerator,p->brake,c->status,c->completed_steps,c->world_hit,c->traffic_hit,c->unsupported,(unsigned long long)c->traffic_index,(long)c->position.x,(long)c->position.y,(long)c->position.z,(long)c->speed);
  }
 }
 int bad=ferror(f);if(fclose(f))bad=1;if(bad)fputs("Decision trace write failed.\n",stderr);
}
