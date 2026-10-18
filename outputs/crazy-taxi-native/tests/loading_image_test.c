#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "../src/loading_image.h"
int main(void) {
    uint8_t guarded[38402], image[4096];
    memset(guarded, 231, sizeof guarded);
    for (unsigned y = 0; y < 64; ++y)
        for (unsigned x = 0; x < 64; ++x)
            image[y * 64 + x] = x % 3 ? (uint8_t)(y + 1) : 0;
    taxi_loading_image(guarded + 1, image);
    assert(guarded[0] == 231 && guarded[38401] == 231);
    for (unsigned y = 0; y < 160; ++y)
        for (unsigned x = 0; x < 240; ++x) {
            unsigned source_y = y >= 120 ? y - 120 : y >= 60 ? y - 60 : y;
            uint8_t expected = x < 64 && x % 3 ? (uint8_t)(source_y + 1) : 231;
            assert(guarded[1 + y * 240 + x] == expected);
        }
    memset(image, 0, sizeof image);
    uint8_t before[38402]; memcpy(before, guarded, sizeof before);
    taxi_loading_image(guarded + 1, image);
    assert(!memcmp(before, guarded, sizeof before));
    uint16_t palette[256];
    uint8_t rgba[153600];
    for (unsigned i = 0; i < 256; ++i)
        palette[i] = (uint16_t)((i & 31) | (((i / 3) & 31) << 5) | (((i / 7) & 31) << 10));
    image[0] = 255;
    for (unsigned clear = 0; clear < 256; ++clear)
        for (int cap = 0; cap < 32; ++cap) {
            taxi_loading_background(rgba, image, palette, (uint8_t)clear, (int16_t)cap);
            unsigned probes[] = {0, 64, 60 * 240, 120 * 240, 38399};
            for (unsigned p = 0; p < 5; ++p) {
                unsigned pixel = probes[p];
                unsigned index = p == 0 || p == 2 || p == 3 ? 255 : clear;
                for (unsigned c = 0; c < 3; ++c) {
                    unsigned value = (palette[index] >> (5 * c)) & 31;
                    if (value > (unsigned)cap) value = (unsigned)cap;
                    assert(rgba[pixel * 4 + c] == value * 255 / 31);
                }
                assert(rgba[pixel * 4 + 3] == 255);
            }
        }
    puts("Loading indexed image: overlap, transparent zero, bottom clipping and guards PASS");
    return 0;
}
