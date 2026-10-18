#ifndef TAXI_NORMAL_MENU_ROUTE_H
#define TAXI_NORMAL_MENU_ROUTE_H
#include <stdint.h>
/* Callback targets from normal parent 0800b6a8/b6c4 and menus174dc/17740.
   This is routing only; rendering/input/audio ownership remain separate. */
typedef enum {TAXI_MENU_MAIN,TAXI_MENU_MODE,TAXI_MENU_CHALLENGE,
    TAXI_MENU_RECORDS,TAXI_MENU_OPTIONS,TAXI_MENU_EXTRA,TAXI_MENU_SESSION,
    TAXI_MENU_DEMO,TAXI_MENU_CHALLENGE_BRIEFING} TaxiNormalMenuTarget;
TaxiNormalMenuTarget taxi_normal_return(int city_cancelled,int demo);
TaxiNormalMenuTarget taxi_normal_main_route(uint8_t selection,uint8_t exit_reason);
int taxi_normal_mode_route(uint8_t selection,uint8_t exit_reason,
                          uint8_t *settings0,TaxiNormalMenuTarget *target);
/* 1AAF8..1AB60: exit routing only. local_continue is original sp+38;
   it is not inferred from the physical Back key. */
int taxi_challenge_menu_route(uint8_t selection3886,uint8_t exit3895,
                             int local_continue,uint8_t *settings0,
                             uint8_t *submode1537,TaxiNormalMenuTarget *target);

/* Briefing1AB74: sampled original bank3500 offsets0/3 toggle page;
   offset1 exits. tick3B00 owns elapsed time, fade177B is external. */
typedef struct {uint8_t page3875,exit3895,blink;uint16_t elapsed;uint16_t fade_flags;} TaxiChallengeBriefing;
void taxi_challenge_briefing_begin(TaxiChallengeBriefing *state);
void taxi_challenge_briefing_step(TaxiChallengeBriefing *state,int fade177b,
                                 int bank0,int bank3,int bank1,uint32_t tick3b00);

/* Grid branch1A544..1A71C only: caller owns fade/exit/profile gates.
   Samples bank3500 offsets6,7,5,4 in original priority order. Returns1
   when a direction branch ran; profile rows0/1 handled separately. */
int taxi_challenge_grid_move(uint8_t *selection3886,uint8_t *remembered_sp34,
                            int name_edit_sp40,const uint8_t bank3500[8]);

/* Vertical profile/driver rows1A724..1A808 only. On driver changes,
   original blinksp3C and elapsedsp44 reset. Horizontal rows separate. */
int taxi_challenge_profile_vertical(uint8_t selection3886,int name_edit_sp40,
                                   uint8_t *profile1538,uint8_t *driver1539,
                                   uint8_t *blink_sp3c,uint16_t *elapsed_sp44,
                                   const uint8_t bank3500[8]);

/* Profile horizontal branch1A408..1A53E. Return-1 is original error
   sound path for an unfinished name;1 moved;0 no applicable input. */
int taxi_challenge_profile_horizontal(uint8_t *selection3886,uint8_t *remembered_sp34,
                                     int name_edit_sp40,const uint8_t profile_group[12],
                                     const uint8_t bank3500[8]);

typedef struct {uint8_t selection3886,exit3895,name_edit_sp40,
    name_cursor388f,name_character3890;uint16_t fade_flags;} TaxiChallengeAction;
/* Name edit1A896..1AAC4. Repeat bank34F6 cursor offsets4/5 and
   character6/7; confirm bank3500 offsets0/3. Returns1 accepted name,
   -1 duplicate, -2 invalid native state,0 continues. No persistence. */
int taxi_challenge_name_edit(TaxiChallengeAction *state,uint8_t settings[512],
                            uint8_t profile1538,const uint8_t bank34f6[8],
                            const uint8_t bank3500[8]);


/* Input prefix1A284..1A3FE only. Return1 consumes this menu iteration;
   return-1 reports original error sound;0 continues navigation/name edit.
   Entry fade/exit gates included. settings_group is current profile12bytes. */
int taxi_challenge_action_prefix(TaxiChallengeAction *state,int fade177b,
                                uint8_t settings_group[12],const uint8_t bank3500[8]);

typedef struct {TaxiChallengeAction action;uint8_t remembered_sp34,briefing_sp38,
    blink_sp3c;uint16_t elapsed_sp44;} TaxiChallengeMenuControl;
/* Entry19B7C local reset and19C8E..19CA4 profile-selection gate.
   Resource reset3896/3897/fades are external to this typed control state. */
int taxi_challenge_menu_begin(TaxiChallengeMenuControl *state,const uint8_t settings[512],
                              uint8_t *profile1538,uint8_t previous_selection3886);
/* Original active input branch1A238..1AAC4 composition. Caller owns
   entry/resource/render/yield/fade/callback/audio operations. */
int taxi_challenge_menu_control(TaxiChallengeMenuControl *state,int fade177b,
                               uint8_t settings[512],uint8_t *profile1538,
                               uint8_t *driver1539,uint32_t tick3b00,
                               const uint8_t bank3500[8],const uint8_t bank34f6[8]);

/* Render decision1B65C..1B6A0, not a gameplay unlock gate. Any valid
   record among3 entries substitutes group3/frame13 for number text. */
int taxi_challenge_record_badge(const uint8_t settings[512],uint8_t profile1538,
                                uint8_t submode1537);

typedef struct {uint8_t selection,exiting;uint16_t fade_flags;} TaxiNormalMenuChoice;
/* Already sampled original bank flags, not held physical keys. main=1 uses
   owner387d/count5, main=0 owner387e/count4. Other shared menus excluded. */
void taxi_normal_menu_choice(TaxiNormalMenuChoice *state,int main,int fade_active,
                            uint8_t recorded_input_gate,int up,int down,int confirm,int back);
#endif
