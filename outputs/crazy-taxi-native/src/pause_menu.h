#ifndef TAXI_PAUSE_MENU_H
#define TAXI_PAUSE_MENU_H
#include <stdint.h>
#include "input.h"
typedef struct {uint8_t frozen,selection,dialog,no_selected,outer_state,exit_flag,palette_request,view_marker;int16_t zoom,view_mode;} TaxiPauseMenu;
typedef struct {uint8_t start,confirm,cancel,up,down,fade_active,ended_1696,ended_1697;} TaxiPauseInput;
typedef struct {uint8_t pause_audio,resume_audio,art,fade_out,camera_refresh;} TaxiPauseEvents;
/* Original21C5C ordinary pause/Resume/Retry/Quit transitions. Debug camera
   dialog3 is an explicit unresolved path, returns-1. Art/audio/camera are
   caller-owned requests; no original image/palette side effect claim. */
/* Original2277C resets five menu bytes; all other owners are retained. */
void taxi_pause_menu_reset(TaxiPauseMenu *);
/* Consumes an already sampled15C3C input; does not advance input timers. */
int taxi_pause_menu_control_sampled(TaxiPauseMenu *,const TaxiInput *,uint8_t fade,uint8_t intro,uint8_t ended,TaxiPauseEvents *);
int taxi_pause_menu_step(TaxiPauseMenu *,const TaxiPauseInput *,TaxiPauseEvents *);
#endif
