#include "../src/input.h"
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>

int main(void) {
    TaxiInput s = {0};
    TaxiInputSample sample = {0x03ff, 0, 0, 0, 1};
    taxi_input_update(&s, sample);
    assert(s.pressed_mask == 0 && s.auxiliary_mask == 0xffff);
    for (unsigned i = 0; i < 10; ++i) assert(!s.held[i]);
    /* Every button: press, hold, repeat threshold, subsequent repeat, release. */
    for (unsigned i = 0; i < 10; ++i) {
        uint16_t bit = (uint16_t)(1u << i);
        sample.active_low_keys = (uint16_t)(0x3ff ^ bit);
        taxi_input_update(&s, sample);
        assert(s.pressed_mask == bit && s.pressed[i] && s.repeat[i] && s.countdown[i] == 60);
        for (int tick = 1; tick < 60; ++tick) {
            taxi_input_update(&s, sample);
            assert(!s.pressed[i] && !s.repeat[i] && s.countdown[i] == 60-tick);
        }
        taxi_input_update(&s, sample);
        assert(s.repeat[i] && s.countdown[i] == 30 && !s.pressed_mask);
        sample.elapsed_ticks = 31;
        taxi_input_update(&s, sample);
        assert(s.repeat[i] && s.countdown[i] == 30);
        sample.elapsed_ticks = 1;
        sample.active_low_keys = 0x3ff;
        taxi_input_update(&s, sample);
        assert(!s.held[i] && s.previous[i] && !s.repeat[i] && !s.countdown[i]);
    }
    /* Physical suppression and override, including A/Start exemptions. */
    sample.active_low_keys = 0;
    sample.suppress_physical = 1;
    sample.override_enabled = 1;
    sample.override_mask = 0x3ff;
    s.auxiliary_mask = 0x1234;
    taxi_input_update(&s, sample);
    assert(s.pressed_mask == (0x3ff ^ 1 ^ 8));
    assert(s.auxiliary_mask == 0x1234 && !s.held[0] && !s.held[3]);
    sample.suppress_physical = 0;
    sample.override_mask = 0;
    taxi_input_update(&s, sample);
    assert(s.held[0] && s.held[3]);
    for (unsigned i = 1; i < 10; ++i) if (i != 3) assert(!s.held[i]);
    /* Tick subtraction preserves original modulo-65536 behavior. */
    s.countdown[0] = 60;
    sample.elapsed_ticks = 65536;
    taxi_input_update(&s, sample);
    assert(s.countdown[0] == 60 && !s.repeat[0]);
    sample.elapsed_ticks = 60;
    taxi_input_update(&s, sample);
    assert(s.countdown[0] == 30 && s.repeat[0]);
    puts("PASS: native input routine boundary and state-transition tests. Not a playable game.");
    return 0;
}
