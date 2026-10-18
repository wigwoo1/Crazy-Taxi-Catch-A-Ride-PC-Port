#ifndef TAXI_NORMAL_RESTART_ENTRY_H
#define TAXI_NORMAL_RESTART_ENTRY_H
#include <stdint.h>
/* Ordinary mode0 AE72..AF80 only; graphics/helpers remain caller adapters. */
typedef struct {uint32_t frame_34d4,frame_34d8,retained_sl;uint16_t width_7542,height_7544,scroll_x,scroll_y;uint8_t fade_177b,phase;} TaxiRestartEntry;
typedef int (*TaxiRestartEntryCall)(void *,uint32_t,const uint32_t[4]);
/* begin emits source initial26440/fadeOUT, then runs until waiting or ready.
   step resumes D764 while waiting; caller owns fade progress.
   D764/26318 callbacks may return1 while the SAME child is suspended,0 on
   completion,-1 on failure. Every other callback must complete with0.
   Native phase3 retains26318 and phase4 retainsD764 even if shared fade
   changes to0; preceding clears/setup are never repeated;
   these are not original game bytes. Existing synchronous callbacks are valid.
   Return0 pending,1 source AF80 boundary,-1 contract error. */
int taxi_restart_entry_begin(TaxiRestartEntry *,TaxiRestartEntryCall,void *);
int taxi_restart_entry_step(TaxiRestartEntry *,TaxiRestartEntryCall,void *);
#endif
