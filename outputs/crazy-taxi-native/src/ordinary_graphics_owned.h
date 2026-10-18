#ifndef TAXI_ORDINARY_GRAPHICS_OWNED_H
#define TAXI_ORDINARY_GRAPHICS_OWNED_H
#include "attract_graphics_owned.h"
#include "palette_fade_setup.h"
#include "scene_registration.h"
/* Same asset/font/object owner layout; ordinary070E50 resource prefix. */
typedef TaxiAttractGraphicsOwners TaxiOrdinaryGraphicsOwners;
int taxi_ordinary_graphics_owned(TaxiOrdinaryGraphicsOwners *,uint32_t retained_sl);
/* Bound743C graphics prefix: reset registrations, package, slot0 font and
   background palette and normal mode1 fade initialization. Caller supplies
   extracted256color data, not code. */
int taxi_bound_graphics_owned(TaxiOrdinaryGraphicsOwners *,uint32_t,const uint16_t [256],uint32_t);
/* Retains caller interval/reload and hardware fields; publishes the fade owner. */
int taxi_bound_graphics_start_owned(TaxiOrdinaryGraphicsOwners *,uint32_t,const uint16_t [256],uint32_t,TaxiPaletteFadeStart *);
/* Publish shared ordinary OBJ owners, restoring original frame templates. */
int taxi_ordinary_objects_publish(TaxiOrdinaryGraphicsOwners *);
/* Append the scene descriptor's original BG palette after existing OBJ/HUD
   records, preserving their source order, fonts, objects and live queues.
   Decode/verify the expected asset against owned ROM bytes before mutation.
   Returns original registration index or-1; errors leave owners/request intact.
   Called once per source scene registration; existing BG allocation is retained
   and rejected rather than silently reset or registered a second time. */
int taxi_ordinary_scene_palette_owned(TaxiOrdinaryGraphicsOwners *,
 const TaxiSceneRegistration *,const uint16_t expected[256],TaxiPaletteTransfer *);
#endif
