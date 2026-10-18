#ifndef TAXI_NORMAL_MENU_FLOW_H
#define TAXI_NORMAL_MENU_FLOW_H
#include "normal_menu_route.h"
#include "palette_fade.h"
#include "menu_input.h"
/* Normal174dc/17740 callbacks; caller supplies sampled source bank flags.
   Call input before presentation; call present after rendering that frame. */
typedef struct {
    TaxiNormalMenuChoice choice;
    TaxiPaletteFade fade;
    TaxiPaletteMotion motion;
    uint16_t mask;
    int main,finished;
    int32_t idle;
} TaxiNormalMenuFlow;
void taxi_normal_menu_flow_begin(TaxiNormalMenuFlow *s,int main,uint8_t selection,
    int reinitialize,uint8_t interval,uint8_t reload);
void taxi_normal_menu_flow_input(TaxiNormalMenuFlow *s,uint8_t recorded_input_gate,
    int up,int down,int confirm,int back,uint16_t held,int32_t delta);
void taxi_normal_menu_flow_present(TaxiNormalMenuFlow *s,int32_t delta);
unsigned taxi_normal_menu_flow_cap(const TaxiNormalMenuFlow *s);
int32_t taxi_normal_menu_flow_slide(const TaxiNormalMenuFlow *s);
/* Consume retained034f6 repeat bank, then26318 presentation progression
   and15c3c sampling for NEXT iteration. Render before this call. */
void taxi_normal_menu_flow_frame(TaxiNormalMenuFlow *s,TaxiMenuInput *input,
    uint8_t recorded_input_gate,uint16_t held,int32_t delta);
/* Challenge19B7C controller consumes3500 fresh presses and34F6 repeats
   before26318 samples physical keys for the next iteration. Fade progression
   remains the caller's responsibility; no guessed presentation ownership. */
int taxi_challenge_menu_input_frame(TaxiChallengeMenuControl *state,
    TaxiMenuInput *input,int fade177b,uint8_t settings[512],
    uint8_t *profile1538,uint8_t *driver1539,uint16_t held,int32_t delta);

/* Source19B7C/1AB74 callback lifecycle. Resource loading and rendering are
   caller-owned. target becomes valid only after the exit fade completes. */
typedef struct {
    TaxiChallengeMenuControl grid;
    TaxiChallengeBriefing briefing;
    TaxiPaletteFade fade;
    TaxiPaletteMotion motion;
    TaxiNormalMenuTarget target;
    uint16_t mask;
    uint8_t is_briefing,finished;
} TaxiChallengeMenuFlow;
int taxi_challenge_menu_flow_begin(TaxiChallengeMenuFlow *state,int briefing,
    int reinitialize,const uint8_t settings[512],uint8_t *profile1538,
    uint8_t previous_selection,uint8_t interval,uint8_t reload);
int taxi_challenge_menu_flow_frame(TaxiChallengeMenuFlow *state,
    TaxiMenuInput *input,uint8_t settings[512],uint8_t *profile1538,
    uint8_t *driver1539,uint8_t *submode1537,uint16_t held,int32_t delta);
#endif
