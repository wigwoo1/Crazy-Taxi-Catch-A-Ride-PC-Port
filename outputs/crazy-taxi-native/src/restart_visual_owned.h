#ifndef TAXI_RESTART_VISUAL_OWNED_H
#define TAXI_RESTART_VISUAL_OWNED_H
#include "normal_restart_flow.h"
#include "ordinary_loading_buffers.h"
#include "graphics_bootstrap_owned.h"
#include "graphics_memory_base.h"
#include "display_mode.h"
#include "destination_guidance.h"
#include "hud_start_owned.h"
#include "session_init.h"
#include "pause_menu.h"
typedef struct {
 TaxiOrdinaryLoadingPending *loading;const TaxiLoadingFrameBuffers *buffers;
 TaxiDisplayMode *display;TaxiGraphicsMemoryBase *bases;
 TaxiGraphicsBootstrapPrefix *bindings;TaxiGraphicsBootstrapTail *bootstrap;
 TaxiRestartRenderContext *render;TaxiSceneRegistration *registration;
 const TaxiGraphicsBootstrapAssets *assets;TaxiGuidanceRoute *guidance;
 TaxiHudStartOwners *hud;const TaxiHudStartInput *hud_input;TaxiSessionInit *session;TaxiPauseMenu *pause;
 const uint16_t *alpha_00e0,*alpha_00e2;
 uint32_t *driver_graphics_187c,*intro_graphics_1804;
 uint32_t *fill_06900,*frame_34d4,*frame_34d8;uint16_t *scroll_0010,*scroll_0012;
 TaxiNormalRestartFlow *flow;uint8_t graphics_index,entry_index,executing,aborted;
} TaxiRestartVisualOwners;
/* Native synchronous GRAPHICS/ENTRY requests only. D764/26318 belong to the
   existing persistent presentation bridge, never rendered/waited here.
   Each fresh restart has a zero-initialized coordinator over retained game
   objects. No graphics/font prefix replay; unused route/graphics bytes retain
   source semantics. Actual source descriptor/ROM data and current transform,
   HUD inputs, clock/alpha/register aliases must remain alive across waits.
   HUD settings read canonical session.settings[0] at actual1EE28, including
   transient16174 event updates; an initial settings snapshot is not a gate.
  0 returned,-1 rejected/failed. Native partial effects retained on failure. */
int taxi_restart_visual_owned(void *,TaxiNormalRestartFlow *,uint32_t,const uint32_t[7],uint32_t *);
#endif
