#include "earned_candidates.h"
int earned_candidates(const EarnedForecastState *initial,const EarnedForecastResources *resources,const TaxiXboxSample *choices,size_t choices_count,const int32_t *delta,size_t steps,EarnedCandidates *out){
 if(!initial||!resources||!choices||!out||!delta||!choices_count||choices_count>7||!steps||steps>60)return -1;
 EarnedCandidates next={0};next.count=choices_count;
 for(size_t i=0;i<choices_count;i++){
  TaxiXboxSample pads[60];for(size_t j=0;j<steps;j++)pads[j]=choices[i];
  EarnedForecastResult result;
  if(earned_forecast(initial,resources,pads,delta,steps,&result))return -1;
  EarnedCandidate *c=next.candidate+i;
  c->completed_steps=result.completed_steps;c->world_hit=result.world_hit;c->traffic_hit=result.traffic_hit;c->unsupported=result.unsupported;c->traffic_index=result.traffic_index;
  c->position=result.state.car.world.height.position;c->speed=result.state.car.world.speed_544;
  if(result.unsupported)c->status=EARNED_CANDIDATE_UNSUPPORTED;
  else if(result.world_hit||result.traffic_hit)c->status=EARNED_CANDIDATE_HAZARD;
  else if(result.completed_steps==steps)c->status=EARNED_CANDIDATE_CLEAR;
  else return -1;
 }
 *out=next;return 0;
}
