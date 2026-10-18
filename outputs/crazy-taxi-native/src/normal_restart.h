#ifndef TAXI_NORMAL_RESTART_H
#define TAXI_NORMAL_RESTART_H
#include <stdint.h>
/* Original AC70..ACC6 prefix only. These three bytes are actual owners;
   settings/city/driver, fade, outer exit and all later initialization remain
   caller-owned. Helpers are explicit synchronous callbacks, not substitutes. */
typedef struct {uint8_t qualified_1691,flag_153c,flag_153d;} TaxiNormalRestart;
typedef int (*TaxiNormalRestartCall)(void *,uint32_t,uint32_t,uint32_t,uint32_t *);
int taxi_normal_restart_prefix(TaxiNormalRestart *,TaxiNormalRestartCall,void *);
/* Original AE48..AE72: write frame-pair owners before the eight setup
   helpers. Retained sl is supplied from the real source caller frame.
   This does not cover graphics/fade before it or AF80 gameplay entry. */
typedef struct {uint32_t first_34d4,second_34d8;} TaxiNormalRestartFrame;
int taxi_normal_restart_game_setup(TaxiNormalRestartFrame *,uint32_t,TaxiNormalRestartCall,void *);
/* Persistent AE4A..AE6E setup; only actual227AC may suspend. The same
   frame and native helper bindings must be retained through every step. */
typedef struct {uint8_t index,phase;} TaxiNormalRestartSetupPending;
int taxi_normal_restart_setup_begin(TaxiNormalRestartSetupPending *,TaxiNormalRestartFrame *,uint32_t retained_sl,TaxiNormalRestartCall,void *);
int taxi_normal_restart_setup_step(TaxiNormalRestartSetupPending *,TaxiNormalRestartFrame *,TaxiNormalRestartCall,void *);
#endif
