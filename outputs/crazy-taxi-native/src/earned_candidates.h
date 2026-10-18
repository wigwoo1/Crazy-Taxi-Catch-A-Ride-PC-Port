#ifndef EARNED_CANDIDATES_H
#define EARNED_CANDIDATES_H
#include "earned_forecast.h"
/* Diagnostic fixed-input alternatives, NOT original navigation AI. CLEAR
   means this bounded model completed every requested step without a hazard;
   it is not a guarantee about future gameplay, passengers or phase changes.
   No steering choice is selected here. Resources/callbacks must be read-only. */
enum {EARNED_CANDIDATE_CLEAR=1,EARNED_CANDIDATE_HAZARD=2,EARNED_CANDIDATE_UNSUPPORTED=3};
typedef struct {unsigned status,completed_steps,world_hit,traffic_hit,unsupported;size_t traffic_index;TaxiPoint position;int32_t speed;} EarnedCandidate;
typedef struct {size_t count;EarnedCandidate candidate[7];} EarnedCandidates;
/* At most seven constant-pad alternatives, each with the same explicit
   delta sequence (1..60 steps). Errors preserve *out. Input state is copied
   independently for every alternative; output contains no reusable game state. */
int earned_candidates(const EarnedForecastState *,const EarnedForecastResources *,const TaxiXboxSample *,size_t,const int32_t *,size_t,EarnedCandidates *);
#endif
