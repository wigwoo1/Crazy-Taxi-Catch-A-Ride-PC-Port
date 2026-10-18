#ifndef TAXI_AUDIO_VOLUME_RENDER_H
#define TAXI_AUDIO_VOLUME_RENDER_H
#include <stdint.h>
typedef struct {uint8_t descriptor,kind,flags;int32_t x,y;} TaxiAudioVolumeSprite;
/* Source1bbfc..1bc96. Original two32pixel chunks from kind3 descriptors6..10.
   Caller supplies original row anchor after source layout/slide calculation. */
int taxi_audio_volume_render(uint16_t viewport_width,int32_t row_y,uint8_t volume,
    TaxiAudioVolumeSprite output[2]);
/* Native consumer of extracted five32x8 source frames. Palette is explicitly
   caller-owned RGB555; transparent index0 preserves underlying pixels. */
int taxi_audio_volume_compose(uint8_t output[153600],const uint8_t indices[1280],
    const uint16_t palette[16],const TaxiAudioVolumeSprite requests[2]);
#endif
