#ifndef TAXI_NORMAL_RESTART_GRAPHICS_H
#define TAXI_NORMAL_RESTART_GRAPHICS_H
#include <stdint.h>
/* Historical viewport_1780 field name denotes source palette/material
   registration count, not viewport size; caller2CA94 selects count-1. */
/* Ordinary mode0 ACC6..AE48 caller only. Helper effects stay in adapters.
   D764 callback may return1 while the SAME native child is suspended,0 when
   complete,-1 on failure; no next helper/fade check runs while suspended.
   Native phase3 retains that child even if its shared fade changes to0; it
   is a host continuation, not an original game byte. */
typedef struct {uint32_t retained_sl,context_base,driver_graphics_187c,intro_graphics_1804;uint8_t city,viewport_1780,fade_177b,phase;} TaxiRestartGraphics;
typedef int (*TaxiRestartGraphicsCall)(void *,uint32_t,const uint32_t[7]);
int taxi_restart_graphics_begin(TaxiRestartGraphics *,TaxiRestartGraphicsCall,void *);
int taxi_restart_graphics_step(TaxiRestartGraphics *,TaxiRestartGraphicsCall,void *);
/* Shared AD66..AE48 suffix, after ordinary or attract asset/font setup. */
int taxi_restart_graphics_common_begin(TaxiRestartGraphics *,TaxiRestartGraphicsCall,void *);
#endif
