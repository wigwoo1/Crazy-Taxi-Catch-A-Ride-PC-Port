#ifndef TAXI_CHALLENGE_EXIT_H
#define TAXI_CHALLENGE_EXIT_H
#include <stdint.h>
typedef struct {uint8_t outer,fade,frozen,ended,success,main_mode,redraw,results;uint32_t callback;} TaxiChallengeExit;
typedef struct {unsigned count;uint32_t calls[8];} TaxiChallengeExitRequests;
/* Original79D2..7AFA:0 live/fading,1 Quit complete,2 terminal743C Restart transfer.
   External callees are ordered requests; helper bodies and devices are excluded. */
int taxi_challenge_exit_step(TaxiChallengeExit *,TaxiChallengeExitRequests *);
/* Direct original743C..744E status stores before bootstrap helpers. */
typedef struct {uint8_t qualified_1691,success_153c,rank_153d;} TaxiChallengeRestartStatus;
int taxi_challenge_restart_clear_status(TaxiChallengeRestartStatus *);
#endif
