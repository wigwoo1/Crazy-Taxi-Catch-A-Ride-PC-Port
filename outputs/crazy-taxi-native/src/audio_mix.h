#ifndef TAXI_AUDIO_MIX_H
#define TAXI_AUDIO_MIX_H
#include <stdint.h>
#include <stddef.h>
/* Original ARM0800025c, relocated030059a0: signed8 PCM, nearest sample,
   phase Q8, wrapped16 additive accumulation. No loop/channel scheduling or
   host playback. Invalid bounds retain destination and output phase. */
int taxi_audio_mix(int16_t *destination,size_t count,const uint8_t *samples,
    size_t size,uint32_t phase,uint32_t step,uint32_t volume,uint32_t *next);
/* ARM08000320: selector per16decoded samples; packed nibble low first;
   source signed8 lookup codebook. Same Q8 phase and wrapped16 accumulation. */
int taxi_audio_mix_packed(int16_t *dst,size_t count,const uint8_t *selectors,size_t blocks,
    const uint8_t *packed,size_t bytes,const uint8_t *codebook,size_t codes,
    uint32_t phase,uint32_t step,uint32_t volume,uint32_t *next);
/* Original080001d8: arithmetic shift6, clip[-127,127], signed8 output.
   Source callers use positive multiples4; native guard permits count0. */
int taxi_audio_convert(const int16_t *source,uint8_t *output,size_t count);
/* ARM0800046c: read current sample BEFORE loop reset, then add step.
   size includes source padding; threshold and loop are separate metadata. */
int taxi_audio_mix_loop(int16_t *dst,size_t count,const uint8_t *samples,size_t size,
    uint32_t phase,uint32_t step,uint32_t threshold,uint32_t loop,uint32_t volume,uint32_t *next);
#endif
