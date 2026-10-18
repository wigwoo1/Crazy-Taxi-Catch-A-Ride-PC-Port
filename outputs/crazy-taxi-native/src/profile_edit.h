#ifndef TAXI_PROFILE_EDIT_H
#define TAXI_PROFILE_EDIT_H
#include <stdint.h>
typedef struct {uint8_t column,character,profile;} TaxiProfileEdit;
enum {TAXI_PROFILE_EDIT_ACCEPT=1,TAXI_PROFILE_EDIT_DUPLICATE=2,TAXI_PROFILE_EDIT_CANCEL=4};
/* Source24e78..2516a edit branch only. Caller owns selection/fade/profile
   flags and original three-letter names; chars[42] END is excluded. */
unsigned taxi_profile_edit_step(TaxiProfileEdit *edit,uint8_t names[2][3],
    const uint8_t chars[43],int right,int left,int up,int down,
    int confirm,int back,int gated);
#endif
