#include "input.h"
#include <string.h>

void taxi_input_startup_reset(TaxiInput *s) {
    if (!s) return;
    memset(s->held, 0, sizeof s->held);
    memset(s->previous, 0, sizeof s->previous);
    memset(s->repeat, 0, sizeof s->repeat);
    memset(s->countdown, 0, sizeof s->countdown);
}

void taxi_input_update(TaxiInput *s, TaxiInputSample sample) {
    s->raw_keys = sample.active_low_keys;
    if (!sample.suppress_physical || s->auxiliary_mask == 0)
        s->auxiliary_mask = UINT16_MAX;

    for (unsigned button = 0; button < 10; ++button) {
        const uint16_t bit = (uint16_t)(1u << button);
        s->previous[button] = s->held[button];
        s->pressed[button] = 0;
        s->pressed_mask &= (uint16_t)~bit;
        s->held[button] = sample.suppress_physical ? 0 : !(sample.active_low_keys & bit);
        /* The original override exempts A and Start. Preserve that distinction. */
        if (sample.override_enabled && button != 0 && button != 3)
            s->held[button] = !!(sample.override_mask & bit);

        if (s->held[button] && !s->previous[button]) {
            s->countdown[button] = 60;
            s->repeat[button] = 1;
            s->pressed[button] = 1;
            s->pressed_mask |= bit;
        } else if (!s->held[button] && s->previous[button]) {
            s->countdown[button] = 0;
            s->repeat[button] = 0;
        } else if (s->held[button]) {
            /* Original stores low 16 bits then tests the signed halfword.
               Use explicit sign-bit logic to avoid implementation-defined casts. */
            const uint16_t remaining = (uint16_t)((uint32_t)s->countdown[button] - sample.elapsed_ticks);
            s->countdown[button] = remaining;
            if (remaining == 0 || (remaining & 0x8000u)) {
                s->repeat[button] = 1;
                s->countdown[button] = 30;
            } else {
                s->repeat[button] = 0;
            }
        }
    }
}
