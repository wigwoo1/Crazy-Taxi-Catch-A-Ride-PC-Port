#ifndef TAXI_TITLE_PARENT_H
#define TAXI_TITLE_PARENT_H
#include <stdint.h>
/* Original1E274 shared parent owners; callback identities are immutable data. */
typedef struct {uint8_t active_3898,flags_3899,menu_3897;uint32_t callback_3878;} TaxiTitleParent;
typedef int (*TaxiTitleParentCall)(void *,TaxiTitleParent *,uint32_t,uint32_t);
int taxi_title_parent_run(TaxiTitleParent *,TaxiTitleParentCall,void *);
/* Suspend at original child boundaries while native host renders frames. */
typedef enum {TAXI_TITLE_PARENT_DONE=0,TAXI_TITLE_PARENT_TITLE=1,TAXI_TITLE_PARENT_INTRO=2,TAXI_TITLE_PARENT_PREPARE_ATTRACT=3,TAXI_TITLE_PARENT_ATTRACT=4} TaxiTitleParentPending;
typedef struct {TaxiTitleParent *state;TaxiTitleParentPending pending;} TaxiTitleParentContinuation;
int taxi_title_parent_begin(TaxiTitleParentContinuation *,TaxiTitleParent *);
int taxi_title_parent_resume(TaxiTitleParentContinuation *);
#endif
