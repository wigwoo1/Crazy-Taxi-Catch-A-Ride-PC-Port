#ifndef TAXI_NORMAL_EXIT_H
#define TAXI_NORMAL_EXIT_H
#include <stdint.h>
typedef struct {uint8_t outer,fade,frozen,ended,mode,qualified,exit_flag,main_mode,redraw,results,phase;uint32_t callback;} TaxiNormalExit;
typedef struct {unsigned count;uint32_t calls[10];} TaxiNormalExitRequests;
/* Ordinary B580..B6CE caller. begin returns0 while live/fading,1 ready,
   or2 transferring to AC70 initialization in the existing game frame. Audio/music/setup calls are explicit
   caller requests, not implemented side effects. */
int taxi_normal_exit_begin(TaxiNormalExit *,TaxiNormalExitRequests *);
/* Phase2 is a terminal Retry transfer for this coordinator: finish/prepare
   never complete it. The host must run AC70 initialization and replace this
   coordinator on entry. The legacy child_returned argument is ignored.
   Quit credits/name/result helpers remain explicit requests. */
int taxi_normal_exit_finish(TaxiNormalExit *,int child_returned,TaxiNormalExitRequests *);
/* Live host continuation at original B5E6 call: prepare returns2 once with
   the credits request and retains frozen/callback/results until resume.
   resume returns0 unchanged while credits run,1 after actual screen return.
   Phase3 is host continuation state, not an original game byte. Result/name
   helpers remain synchronous requests; this API does not implement them. */
int taxi_normal_exit_prepare(TaxiNormalExit *,int child_returned,TaxiNormalExitRequests *);
int taxi_normal_exit_resume(TaxiNormalExit *,int credits_returned,TaxiNormalExitRequests *);
/* Original B5C0..B6CE asynchronous ordinary child caller. Begin only after
   normal_exit_begin returned1 (cleanup done, no Retry transfer). Each return2
   emits exactly ONE child request;0 means waiting;1 means final callback ready.
   Poll with returned_target0 while waiting, or the actual requested callee only
   after its real return. Never acknowledge a child merely because it was begun.
   Parent phase4 and flow stages are native continuation metadata, not game
   bytes. Live shared fields may change inside children and are reread at their
   original boundaries. Mode/override1708!=0 remains unsupported explicitly. */
typedef struct {
 TaxiNormalExit *state;uint32_t pending,last_return;uint8_t stage;
} TaxiNormalExitChildren;
int taxi_normal_exit_children_begin(TaxiNormalExitChildren *,TaxiNormalExit *,TaxiNormalExitRequests *);
int taxi_normal_exit_children_poll(TaxiNormalExitChildren *,uint32_t returned_target,TaxiNormalExitRequests *);
#endif
