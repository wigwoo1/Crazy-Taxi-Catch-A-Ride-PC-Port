#include "../src/xbox.h"
#include "../src/input.h"
#include <stdio.h>

int main(void) {
    puts("Crazy Taxi native port - controller connection check (not the game)");
    unsigned found = 0;
    for (unsigned index = 0; index < 4; ++index) {
        TaxiXboxSample pad = taxi_xbox_poll(index);
        if (!pad.connected) continue;
        ++found;
        TaxiInput input = {0};
        TaxiInputSample sample = {pad.active_low_keys, 0, 0, 0, 1};
        taxi_input_update(&input, sample);
        printf("Controller %u: connected; buttons=%03X steering=%d RT=%u LT=%u\n",
               index + 1, (unsigned)input.pressed_mask, (int)pad.steering,
               (unsigned)pad.accelerator, (unsigned)pad.brake);
    }
    if (!found) puts("No connected XInput controller detected.");
    return 0;
}
