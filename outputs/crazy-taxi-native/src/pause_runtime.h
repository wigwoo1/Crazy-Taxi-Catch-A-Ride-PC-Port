#ifndef TAXI_PAUSE_RUNTIME_H
#define TAXI_PAUSE_RUNTIME_H
#include "menu_input.h"
#include "pause_render.h"
typedef struct {TaxiMenuInput input;TaxiPauseMenu menu;uint8_t alternate;} TaxiPauseRuntime;
typedef struct {TaxiPauseEvents events;TaxiPauseRenderRequest draw[6];unsigned draw_count;int palette_mode;} TaxiPauseFrame;
/* One original-rate ordinary frame: input ->21C5C ->221F8 ->26318.
   Caller owns scene freezing, audio/art, fades, retry/quit and actual drawing.
   Preserves the source confirmation-Start request2 behavior. */
/* Split host integration: controller before simulation (AFCC), presentation
   after customer/HUD work (B468) and palette service (B57C). */
int taxi_pause_runtime_control(TaxiPauseRuntime *,uint16_t held,uint8_t fade,
 uint8_t intro,uint8_t ended,TaxiPauseFrame *);
/*221F8 draw requests only: retain5F24 request and current alternate.
   Does not sample input or consume palette;26318 does that after16208. */
int taxi_pause_runtime_prepare(TaxiPauseRuntime *,uint8_t fade,uint16_t width,TaxiPauseFrame *);
int taxi_pause_runtime_present(TaxiPauseRuntime *,uint8_t fade,uint16_t width,TaxiPauseFrame *);
int taxi_pause_runtime_frame(TaxiPauseRuntime *,uint16_t held,uint8_t fade,
 uint8_t intro,uint8_t ended,uint16_t width,TaxiPauseFrame *);
#endif
