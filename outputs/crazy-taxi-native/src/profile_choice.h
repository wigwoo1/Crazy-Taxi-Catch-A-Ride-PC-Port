#ifndef TAXI_PROFILE_CHOICE_H
#define TAXI_PROFILE_CHOICE_H
#include "profile_edit.h"
#include "menu_input.h"
typedef struct {
    uint8_t option,choice,editing,new_profile,exiting,current;
    TaxiProfileEdit edit;
} TaxiProfileChoice;
enum {TAXI_PROFILE_MOVE=1,TAXI_PROFILE_EDIT_BEGIN=2,TAXI_PROFILE_FADE_OUT=4,
    TAXI_PROFILE_DUPLICATE=8,TAXI_PROFILE_CANCEL_EDIT=16};
unsigned taxi_profile_option_count(int first_exists,int second_exists);
/* Source2468a: use current selection only when both profiles exist. */
void taxi_profile_choice_begin(TaxiProfileChoice *state,int first_exists,
    int second_exists,uint8_t current_profile);
/* Profile existence, three-byte names and input banks are explicit owners.
   Does not commit settings, score rows, or persistent storage. */
unsigned taxi_profile_choice_step(TaxiProfileChoice *state,uint8_t names[2][3],
    const uint8_t chars[43],int first_exists,int second_exists,
    const TaxiMenuInput *input,int fade_gate);
#endif
