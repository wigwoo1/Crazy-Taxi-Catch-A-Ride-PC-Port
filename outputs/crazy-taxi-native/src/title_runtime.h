#ifndef TAXI_TITLE_RUNTIME_H
#define TAXI_TITLE_RUNTIME_H
#include "title_callback.h"
#include "title_flow.h"
#include "palette_fade.h"
#include "menu_input.h"
typedef struct {
 TaxiTitleCallback callback;
 TaxiTitleFlow flow;
 TaxiPaletteFade fade;
 TaxiMenuInput input;
 uint32_t clock;
 /* Original state5 descriptor0849d560+5*28 masks +0xc/+0xe. */
 uint16_t bldy,bg_palette_mask,obj_palette_mask;
 uint8_t finished;
} TaxiTitleRuntime;
typedef struct {
 TaxiTitleCallbackOutput callback;
 int confirm_sound;
 /* 0=title;1=original parent main menu;2=original parent attract branch. */
 uint8_t parent;
} TaxiTitleRuntimeOutput;
void taxi_title_runtime_begin(TaxiTitleRuntime *state);
/* Fully drawn normal title frame, delta1, unpaused clock. Rendering/audio/
   parent dispatch remain host responsibilities. Shared RNG is caller-owned. */
int taxi_title_runtime_frame(TaxiTitleRuntime *state,uint32_t *random,
 uint16_t physical,int sound,TaxiTitleRuntimeOutput *output);
#endif
