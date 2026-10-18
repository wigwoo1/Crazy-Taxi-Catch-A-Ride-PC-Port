#ifndef TAXI_AUDIO_TEST_RENDER_H
#define TAXI_AUDIO_TEST_RENDER_H
#include <stdint.h>
typedef struct {uint8_t descriptor,kind,flags;int32_t x,y;} TaxiAudioTestArrow;
typedef struct {
    char number[4];uint8_t font,text_flags;int32_t x,y;
    unsigned arrows;TaxiAudioTestArrow arrow[2];
} TaxiAudioTestRender;
/* Source1bc98..be06 original valid selectors; source blink bit32 gates
   arrows only, numbers always submitted. Text alignment flag4 is preserved
   for the native text consumer to interpret after source verification. */
int taxi_audio_test_render(uint16_t width,int32_t y,uint8_t row,uint8_t selected,
    int16_t index,uint32_t blink,TaxiAudioTestRender *output);
#endif
