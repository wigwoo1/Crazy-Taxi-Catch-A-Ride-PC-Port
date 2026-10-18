#ifndef CRAZY_TAXI_ACTIONS_H
#define CRAZY_TAXI_ACTIONS_H
#include "input.h"

/* Recovered query IDs, not a complete driving simulation:
   0/1 use Left/Right in all stock profiles. IDs 2..5 swap A/B and L/R
   across profiles; their downstream vehicle semantics remain under investigation.
   ID 6 queries a new Select press. */
typedef struct {
    uint8_t profile;             /* (original byte 02001eb0 >> 1) & 3; valid stock profiles 0..2 */
    uint8_t vehicle_flags;       /* original byte at 02002634 + 04ac; bit 5 gates profile 0 actions 4/5 */
    uint8_t override_enabled;    /* original byte 02001708 */
    uint8_t override_flags;      /* original byte at 030053d0 + 4 */
} TaxiActionContext;

int taxi_action_query(const TaxiInput *input, TaxiActionContext context, unsigned action);
#endif
